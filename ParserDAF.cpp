#include "include/Parse.h"

CommandIndex _0x23 {0x23,
                    0, 1, 5,    //subfunction - размер пакета
                    0, 2, 2, 1};

CommandIndex _0x11 {0x11,
                    0, 1, 2,    
                    0, 1, 2, 3};

byteString emptyByteString = {{0, 0, 0, 0, 0, 0, 0, 0}};

//**************************************************************************************************
// Procedure checkFor0x23()
//**************************************************************************************************

// Проверка ответа на запрос 0х23 //

bool ParserDAF::checkFor0x23(byteString responceString, CommandIndex cmnd)  // Ответ на команду 0х23 содержится в следующей же строке
{
    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40;
}

//**************************************************************************************************
// Procedure checkForReset()
//**************************************************************************************************

// Проверка на hard reset //

bool ParserDAF::checkForReset(byteString responceString, CommandIndex cmnd)
{
    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&  // Ответ содержится в следующей же строке, имеет длину равную трём или даже меньше
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
    char* BuffPtr = Buffer;                   //Указатель на текущее положение в буфере

    std::fstream File;
    File.open(Path);

    uint32_t StringCount = 0;          //Количество строк в файле

    while (File.getline(BuffPtr, STRING_LENGTH_ASCII, '\n'))  //Считываем до конца строки, пока не конец файла
    {
        BuffPtr += STRING_LENGTH_ASCII;               //Свдиг позиции на длину строки в текстовом файле
        StringCount++;
    }
    File.close();

    BuffPtr = Buffer;       // На начало буфера

    uint32_t dataCount = 0;

    for (uint32_t i = 0; i < StringCount; i++)
    {
        byteString temp = StringToByte(BuffPtr);    // Перевести часто используемую строку в структуру

        if (IsCommand(temp, _0x11))     // Если встретилась команда hard reset...
        {
            if (checkForReset(StringToByte(BuffPtr + STRING_LENGTH_ASCII), _0x11))  // ..убедиться, что действительно команда и дан положительный ответ
            {
                for (uint32_t i = 0; i < dataCount; i++)    // Обнулить уже считанные данные
                    Data[i] = emptyByteString;
                
                dataCount = 0;                          // Обнулить счётчик
            }    
        }
        else    // Если hard reset не проишёл, продолжать считывать данные
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
    uint32_t DataLength = 0;    // Счётчик считанных данных после последнего hard reset
    byteString* byteData = FromTxtToByte(TxtPath, DataLength);

    uint32_t PackageLength = 0;     // Количество передаваемых пакетов
    uint32_t PackageCount = 0;      // Количество переданных пакетов
    uint32_t StringCount = 0;       // Количество обработанных строк
    uint8_t index = 0;              // Индекс, с какого элемента начинать считывать байты

    FILE* out;
    out = fopen(BinPath, "wb+");        //Если бинарный файл есть, то он перезапишется, если нет - создастся

    bool IsDataTransferActive = false;

    for (uint32_t i = 0; i < DataLength; i++)
    {
        if(IsCommand(byteData[i], _0x23))       // Если встречена команда...
        {
            if(checkFor0x23(byteData[i + 1], _0x23))    // ...и дан положительный ответ
            {
                IsDataTransferActive = true;

                PackageLength = byteData[i].bytes[_0x23.SUB_FUNC];  // Посчитать длину пакета
                PackageCount = 0;                                   // обнулить счётчики
                StringCount = 0;
            }
        }

        if (IsDataTransferActive)   // Если идёт передача данных...
        {
            if (PackageCount < PackageLength)   //...и количество переданных байтов меньше длины пакета
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
                    fwrite(&byteData[i + 1].bytes[index], 1, 1, out);   // Вывод - запиь в бинарный файл
                    PackageCount++;
                }
                StringCount++;

            }           
        }
    }

    delete byteData;

    return 0;
}