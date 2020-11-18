#include "include/Parse.h"

CommandIndex _0x23 {0x23,
                    0, 1, 5,    //subfunction - размер пакета
                    0, 2, 2, 1};

CommandIndex _0x11 {0x11,
                    0, 1, 2,    //subfunction - размер пакета
                    0, 1, 2, 3};

byteString emptyByteString = {{0, 0, 0, 0, 0, 0, 0, 0}};

//**************************************************************************************************
// Procedure FromTxtToBin()
//**************************************************************************************************

// Выборка полезных данных //

bool ParserDAF::checkFor0x23(byteString responceString, CommandIndex cmnd)
{
    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40;
}

//**************************************************************************************************
// Procedure checkForReset()
//**************************************************************************************************

// Проверка на hard reset //

bool ParserDAF::checkForReset(byteString responceString, CommandIndex cmnd)
{
    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&
    responceString.bytes[cmnd.SIZE] <= 3; 
}

//**************************************************************************************************
// Procedure FromTxtToByte()
//**************************************************************************************************

// Выборка строк после последнего ресета //

byteString* ParserDAF::FromTxtToByte(const char* Path, uint32_t& DataLength)
{
    const uint32_t DataSize = MAX_BUFFER_SIZE / STRING_LENGTH_BYTE;

    byteString* Data = new byteString [DataSize];
    for (uint32_t i = 0; i < DataSize; i++)
        Data[i] = emptyByteString;

    char* Buffer = new char[MAX_BUFFER_SIZE] {'0'};     //Буфер с символами из входного файла
    char* BuffPtr = Buffer;                   //Указател на текущее положение в буфере

    std::fstream File;
    File.open(Path);

    uint32_t StringCount = 0;          //Количество строк в файле

    while (File.getline(BuffPtr, STRING_LENGTH_ASCII, '\n'))  //Считываем до конца строки, пока не конец файла
    {
        BuffPtr += STRING_LENGTH_ASCII;               //Свдиг позиции на длину строки в текстовом файле
        StringCount++;
    }
    File.close();

    BuffPtr = Buffer;

    uint32_t dataCount = 0;

    for (uint32_t i = 0; i < StringCount; i++)
    {
        byteString temp = StringToByte(BuffPtr);

        if (IsCommand(temp, _0x11))
        {
            if (checkForReset(StringToByte(BuffPtr + STRING_LENGTH_ASCII), _0x11))
            {
                for (uint32_t i = 0; i < dataCount; i++)
                    Data[i] = emptyByteString;
                
                dataCount = 0;
            }    
        }
        else
        {
            Data[dataCount] = temp;
            dataCount++;
        }
        BuffPtr += STRING_LENGTH_ASCII;
    }

    delete Buffer;

    DataLength = dataCount;
    return Data;
}

//**************************************************************************************************
// Procedure FromTxtToBin()
//**************************************************************************************************

// Выборка полезных данных //

int ParserDAF :: FromTxtToBin(const char* TxtPath, const char* BinPath)
{
    uint32_t DataLength = 0;
    byteString* byteData = FromTxtToByte(TxtPath, DataLength);

    uint32_t PackageLength = 0;
    uint32_t PackageCount = 0;
    uint32_t StringCount = 0;
    uint8_t index = 0;

    FILE* out;
    out = fopen(BinPath, "wb+");        //Если бинарный файл есть, то он перезапишется, если нет - создастся

    bool IsDataTransferActive = false;

    for (uint32_t i = 0; i < DataLength; i++)
    {
        if(IsCommand(byteData[i], _0x23))
        {
            if(checkFor0x23(byteData[i + 1], _0x23))
            {
                IsDataTransferActive = true;

                PackageLength = byteData[i].bytes[_0x23.SUB_FUNC];
                PackageCount = 0;
                StringCount = 0;
            }
        }

        if (IsDataTransferActive)
        {
            if (PackageCount < PackageLength)
            {
                switch (StringCount)                        //Структура полезных данных команды 0х36 в строке зависит от номера строки:
                {                                           // ??     MS+1    SID_RES D       D       D       D       D  -  0 строка 
                    case 0:                                 // 30     0       0       0       0       0       0       0  -  1 строка
                        index = 3;                          // K      D       D       D       D       D       D       D  -  k-ая строка
                        break;                              //, где N - номер пакета, K - номер строки в пакете, D - любые данные
                    case 1:
                        index = STRING_LENGTH_BYTE;
                        break;
                    default:
                        index = 1;
                        break;
                }

                for(; index < STRING_LENGTH_BYTE; index++)
                {
                    if(PackageCount == PackageLength)       //Нумерация текущих пакетов начинается с нуля, общего количества - с единицы
                    {                                           //Потому если следущая строка является концом пакета, то закончить передачу
                        IsDataTransferActive = false;
                        break;
                    }
                    //std::cout << PackageCount << " - " << (int) byteData[i+1].bytes[index] << '\n';
                    fwrite(&byteData[i + 1].bytes[index], 1, 1, out);
                    PackageCount++;
                }
                StringCount++;

            }           
        }
    }

    delete byteData;

    return 0;
}