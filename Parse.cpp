#include "include\Parse.h"

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
    char* Buffer = new char[MAX_BUFFER_SIZE] {'0'};     //Буфер с символами из входного файла
    char* BufferPtr = Buffer;                   //Указател на текущее положение в буфере

    std::fstream File;
    File.open(Path);

    uint32_t StringNumber = 0;          //Количество строк в файле

    while (File.getline(BufferPtr, STRING_LENGTH_ASCII, '\n'))  //Считываем до конца строки, пока не конец файла
    {
        BufferPtr += STRING_LENGTH_ASCII;               //Свдиг позиции на длину строки в текстовом файле
        StringNumber++;
    }
    File.close();

    byteString* Data = new byteString[StringNumber];    //Буфер байт-строк
    for (uint32_t i = 0; i < StringNumber; i++)
            Data[i] = {emptybyteString};
        
    byteString tempData {emptybyteString};      //Нынешняя строка, нужна для оптимизации в цикле ниже
    BufferPtr = Buffer;

    bool IsDataTransferActive = false;          //Флаг передачи серии пакетов
    uint32_t countData = 0;                     //Длина переанных данных в байт-строках

    for (uint32_t i = 0; i < StringNumber; i++)
    {
        tempData = StringToByte(BufferPtr);     //Чтобы не вызывать несколько раз функцию перевода в байт-строку для указателя на текущее положение в буфере символов
        if (!IsDataTransferActive)              //Если передача не идёт, то искать начало
        {
            if (IsCommand(tempData, _0x34))
                IsDataTransferActive = CheckFor_0x34(BufferPtr, tempData, _0x34);
        }
        else            //Если всё же идёт,то искать конец сессии
        {
            if (IsCommand(tempData, _0x37))
                IsDataTransferActive = !CheckFor_0x37(BufferPtr, tempData, _0x37);
        }


        if (IsDataTransferActive)       //Пока идёт передача, считывать строки в буфер байт-строк
        {
            Data[countData] = tempData; 
            countData++;
        }

        BufferPtr += STRING_LENGTH_ASCII;       //Сдвинуться на длину строки в текстовом файле
    }

    delete Buffer;      //Буфер с символами больше не нужен

    DataStringNumber = countData;       //Передать количество строк функции, вызвавшей извне, через ссылку
    return Data;                        
}

//**************************************************************************************************
// Procedure FromTxtToBin()
//**************************************************************************************************

// Выбор полезных данныхизструктуры 0х34...0х37 //

int Parse::FromTxtToBin(const char* TxtPath, const char* BinPath)
{
    uint32_t DataStrLen = 0; uint32_t& DataStrLenPtr = DataStrLen;      //Количество обрабатываемых строк
    byteString* Data = FromTxtTobyteString(TxtPath, DataStrLenPtr);     //Буфер с байт-строками
    byteString* DataPtr = Data;                                         //Указатель на текующую байт-строку
    
    bool IsDataTransferActive = false;          //Флаг передачи пакета

    uint32_t StringCount = 0;           //Количество обработанных строк
    uint32_t PackageCount = 0;          //Количество переданных байт
    uint32_t PackageLength = 0;         //Общее количество полезных байт в пакете

    FILE* out;
    out = fopen(BinPath, "wb+");        //Если бинарный файл есть, то он перезапишется, если нет - создастся 

    for (uint32_t i = 0; i < DataStrLen; i++)
    {
        if (!IsDataTransferActive)      //Если не идёт передача пакета, то...
        {
            if (IsCommand(*DataPtr, _0x36))     //Если найден запрос в строке
                IsDataTransferActive = CheckFor_0x36(DataPtr, _0x36);   //Проверить ответ
            if (IsDataTransferActive)   //Если подтвердился запрос, обнулить счётчики, посчитать длину сообщения
            {
                StringCount = 0;
                PackageCount = 0;
                PackageLength = countLength(DataPtr->bytes[_0x36.SIZE], DataPtr->bytes[_0x36.SIZE + 1]);
            }
        }
        else    //Если идёт передача пакета, то проверять на конец пакета 
        {
            if (DataPtr->bytes[0] < STRING_LENGTH_BYTE)         //При условии, что первый байт строки с сообщение содержит длину ответа, которая меньше длины строки
                IsDataTransferActive = !CheckFor_0x76(DataPtr, _0x37);
        }


        if (IsDataTransferActive && PackageCount < PackageLength)       //Если идёт передача пакета и ещё не все байты переданы 
        {
            uint8_t index = 0;          //Индекс, с которого надо начинать считывать байты в строке

            switch (StringCount)                        //Структура полезных данных команды 0х36 в строке зависит от номера строки:
            {                                           // MS     MS      SID     N       D       D       D       D  -  0 строка 
                case 0:                                 // 30     0       0       0       0       0       0       0  -  1 строка
                    index = 4;                          // K      D       D       D       D       D       D       D  -  k-ая строка
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
                if(PackageCount + 2 == PackageLength)       //Нумерация текущих пакетов начинается с нуля, общего количества - с единицы
                {                                           //Потому если следущая строка является концом пакета, то закончить передачу
                    IsDataTransferActive = false;
                    break;
                }
                fwrite(&DataPtr->bytes[index], 1, 1, out);  //Передача - помещение полезных байтов в бинарный файл
                PackageCount++;
            }

            StringCount++;
        }

        DataPtr++;
    }

    delete Data;                //После записи в файл структура 0х34...0х37 уже не нужна

    fclose(out);                //Файл закрывается после записи данных из всех сессий
    return 0;
}
