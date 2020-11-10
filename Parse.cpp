#include "Parse.h"

//**************************************************************************************************
// Global variable
//**************************************************************************************************

byteString emptybyteString {{0,0,0,0,0,0,0,0}}; //Пустая байт-строка для инициализации байт-строк

        CommandIndex _0x34 {0x34,               //Индексы и SID для запросов 0х34, 0х36, 0х37
                            0, 2, 3,            
                            0, 1, 2, 3};
        CommandIndex _0x36 {0x36,
                            0, 2, 3,
                            0, 1, 2, 3};
        CommandIndex _0x37 {0x37,
                            0, 1, 2,
                            0, 1, 2, 3};


//**************************************************************************************************
// Procedure byteStringOut()
//**************************************************************************************************

// Вывод байт-строки в консоль //

void Parse::byteStringOut(byteString Out)
{
    for (uint8_t i = 0; i < STRING_LENGTH_BYTE; i++)
        std::cout << std::hex << (int) Out.bytes[i] << '\t';
    std::cout << std::endl;
}

//**************************************************************************************************
// Procedure countLength()
//**************************************************************************************************

// Подсчёт длины сообщения //

uint16_t Parse::countLength(uint8_t one, uint8_t two)
{
    return (uint16_t) ((one << 8 | two) & 0xFFF);  //Свдиг старшего значащего байта влево на байт и применение маски для стирания первого пол-байта
}                                               

//**************************************************************************************************
// Procedure StringToByte()
//**************************************************************************************************

// Перевод строки в байт-строку //

byteString Parse::StringToByte(std::string Input)
{
        byteString Output = emptybyteString;    //Создаём пустую строку
        
        uint8_t Count = 0;              //Номер байта в строке
        std::string buff = "";          //Буфер с подстрокой, полученной разделением входной табами

        uint8_t cycleLength = Input.size() + 1;         
        for (uint8_t i = 0; i < cycleLength; i++)       
        {
            if (Input[i] == '\t' || Input[i] == '\x0')          //Если встречается табили конец строки
            {
                Output.bytes[Count] = (uint8_t) std::stoi(buff, 0, 16); //Перевести подстроку из буфера в шестнадцатеричное число 
                buff = "";                                              //Очистить буфер
                Count++;
            }
            buff += Input[i];           //Буфер заполняется по одному символу
        }
        return Output;
}

//**************************************************************************************************
// Procedure IsCommand()
//**************************************************************************************************

// Проверка определённого командным индексом байта строки на наличие в нём SID //

bool Parse::IsCommand (byteString Input, CommandIndex cmnd)
{
    return Input.bytes[cmnd.SID] == cmnd.SID_VALUE;
}

//**************************************************************************************************
// Procedure CheckFor_0x34()
//**************************************************************************************************

// Проверка ответа после нахождения в строке 0х34 //

bool Parse::CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd) 
{

    uint16_t msgLength = countLength(commandString.bytes[cmnd.SIZE], commandString.bytes[cmnd.SIZE + 1]);     //Длина в байтах
    if (msgLength > 6)          //Если длина в байтах > 6, то потребуется больше, чем одна строчка на передачу. Длина в строках рассчитывается по формуле
        msgLength = ceil ((msgLength - 6) / 7) + 3;     //(Длина_в_байтах - количество_полезных_данных_в_первой_строке)/количество_полезных_байт_в_остальных_строках + неучтённые_строки
    else
        msgLength = 1;          

    Input += msgLength * STRING_LENGTH_ASCII;   //msgLength говорит на сколько индекс строки с ответом больше индекса входной строки, свдигаемся на столько строк
    byteString responceString = StringToByte(Input);    

    return (responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&         //Если ответ на 0х40 больше и длина ответа меньше длины строки
            responceString.bytes[cmnd.SIZE] < 7);
}

//**************************************************************************************************
// Procedure CheckFor_0x36()
//**************************************************************************************************

// Аналогичная проверка ответа после нахождения в строке 0х36 //

bool Parse::CheckFor_0x36(byteString* commandString, CommandIndex cmnd)      
{
    uint16_t Length = countLength(commandString->bytes[cmnd.SIZE], commandString->bytes[cmnd.SIZE + 1]);
    uint16_t msgLength = 0;       
    if (Length > 6)
        msgLength = ceil ((Length - 6) / 7) + 3;     
    else
        msgLength = 1;

    if ((Length - 6) % 7 == 0)  //Если последняя строка пакета полностью заполнена полезными байтами, то нужно сдвинуться на одну строку вверх
        msgLength -= 1;

    byteString* responceString = commandString + msgLength + 1;                                                      
    byteString* subResponceString = responceString - 1;
        
    return (responceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&
            subResponceString->bytes[cmnd.SID_RESPONCE] == 0x7F);   //Если присутствует связка-ответ на 0х36
}

//**************************************************************************************************
// Procedure CheckFor_0x76()
//**************************************************************************************************

// Проверка строки на конец пакета //

bool Parse::CheckFor_0x76 (byteString* responceString, CommandIndex cmnd)
{
    byteString* subResponceString = responceString + 1;
    return responceString->bytes[cmnd.SID_RESPONCE] == 0x7F &&          //Если строка является запросо подождать, а следующая - положительным ответом
           subResponceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40;
}

//**************************************************************************************************
// Procedure CheckFor_0x37()
//**************************************************************************************************

// Проверка ответа после нахождения в строке 0х37 //

bool Parse::CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd)
{
    byteString responceString = StringToByte(Input+STRING_LENGTH_ASCII);

    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&
    responceString.bytes[cmnd.SIZE] < 3;                        //Длина передачи всегда меньше трёх, нужно, поскольку мусор может соответствовать формату команды
}

//**************************************************************************************************
// Procedure FromTxtTobyteString()
//**************************************************************************************************

// Выделение из файла *.txt структуры 0х34...0х37, помещение её в буфер, передача количества исходных строк через левостороннюю ссылку //

byteString* Parse::FromTxtTobyteString(const char* Path, uint32_t& DataStringNumber)
{
    char* Buffer = new char[MAX_BUFFER_SIZE] {'0'};
    char* BufferPtr = Buffer;

    std::fstream File;
    File.open(Path);

    uint32_t StringNumber = 0;

    while (File.getline(BufferPtr, STRING_LENGTH_ASCII, '\n'))
    {
        BufferPtr += STRING_LENGTH_ASCII;
        StringNumber++;
    }
    File.close();

    byteString* Data = new byteString[StringNumber];
    for (uint32_t i = 0; i < StringNumber; i++)
            Data[i] = {emptybyteString};
        
    byteString tempData {emptybyteString};
    BufferPtr = Buffer;

    bool IsDataTransferActive = false;
    uint32_t countData = 0;

    for (uint32_t i = 0; i<StringNumber; i++)
    {
        tempData = StringToByte(BufferPtr);
        if (!IsDataTransferActive)
        {
            if (IsCommand(tempData, _0x34))
                IsDataTransferActive = CheckFor_0x34(BufferPtr, tempData, _0x34);
        }
        else
        {
            if (IsCommand(tempData, _0x37))
                IsDataTransferActive = !CheckFor_0x37(BufferPtr, tempData, _0x37);
        }


        if (IsDataTransferActive)
        {
            Data[countData] = tempData;
            countData++;
        }

        BufferPtr += STRING_LENGTH_ASCII;
    }

    delete Buffer;

    DataStringNumber = countData;
    return Data;
}

//**************************************************************************************************
// Procedure FromTxtToBin()
//**************************************************************************************************

// Выбор полезных данныхизструктуры 0х34...0х37 //

int Parse::FromTxtToBin(const char* TxtPath, const char* BinPath)
{
    uint32_t DataStrLen = 0; uint32_t& DataStrLenPtr = DataStrLen;
    byteString* Data = FromTxtTobyteString(TxtPath, DataStrLenPtr);
    byteString* DataPtr = Data;

    bool IsDataTransferActive = false;

    uint32_t StringCount = 0;
    uint32_t PackageCount = 0;
    uint32_t PackageLength = 0;

    FILE* out;
    out = fopen(BinPath, "wb+");

    for (uint32_t i = 0; i < DataStrLen; i++)
    {
        if (!IsDataTransferActive)
        {
            if (IsCommand(*DataPtr, _0x36))
                IsDataTransferActive = CheckFor_0x36(DataPtr, _0x36);
            if (IsDataTransferActive)
            {
                StringCount = 0;
                PackageCount = 0;
                PackageLength = countLength(DataPtr->bytes[_0x36.SIZE], DataPtr->bytes[_0x36.SIZE + 1]);
            }
        }
        else
        {
            if (DataPtr->bytes[0] < STRING_LENGTH_BYTE)
                IsDataTransferActive = !CheckFor_0x76(DataPtr, _0x37);
        }


        if (IsDataTransferActive && PackageCount < PackageLength)
        {
            uint8_t index = 0;

            switch (StringCount)
            {
                case 0:
                    index = 4;
                    break;
                case 1:
                    index = STRING_LENGTH_BYTE;
                    break;
                default:
                    index = 1;
                    break;
            }

            for(; index < STRING_LENGTH_BYTE; index++)
            {
                if(PackageCount + 2 == PackageLength)         //Íóìåðàöèÿ òåêóùèõ ïàêåòîâ íà÷èíàåòñÿ ñ íóëÿ, îáùåãî êîëè÷åñòâà - ñ åäèíèöû
                {                                           //Ïîòîìó åñëè ñëåäóùàÿ ñòðîêà ÿâëÿåòñÿ êîíöîì ïàêåòà, òî çàêîí÷èòü ïåðåäà÷ó
                    IsDataTransferActive = false;
                    break;
                }
                fwrite(&DataPtr->bytes[index], 1, 1, out);  //Ïåðåäà÷à - ïîìåùåíèå ïîëåçíûõ áàéòîâ â áèíàðíûé ôàéë
                PackageCount++;
            }

            StringCount++;
        }

        DataPtr++;
    }

    delete Data;

    fclose(out);
    return 0;
}
