#include "include\Parse.h"

//**************************************************************************************************
// Global variable
//**************************************************************************************************

byteString emptybyteString {{0,0,0,0,0,0,0,0}};         //Пустая байт-строка для инициализации байт-строк

        CommandIndex _0x34 {0x34,                       //Индексы и SID для запросов 0х34, 0х36, 0х37
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
        byteString Output = emptybyteString;    //Выходная байт-строка

        uint8_t Count = 0;      //Номер байт-строки
        std::string buff = "";  //Буфер, в который заносятся символы

        uint8_t cycleLength = Input.size() + 1;
        for (uint8_t i = 0; i < cycleLength; i++)
        {
            if (Input[i] == '\t' || Input[i] == '\x0')       //После каждого таба или конца строки 
            {
                Output.bytes[Count] = (uint8_t) std::stoi (buff, 0, 16);  //буфер переводится в шестнадцатеричное число и помещается в байт,
                buff = "";                                              //а затем очищается
                Count++;
            }
            buff += Input[i];
        }
        return Output;
}

//**************************************************************************************************
// Procedure IsCommand()
//**************************************************************************************************

// Проверка определённого командным индексом байта строки на наличие в нём SID //

bool Parse::IsCommand(byteString Input, CommandIndex cmnd)
{
    return Input.bytes[cmnd.SID] == cmnd.SID_VALUE;
}

//**************************************************************************************************
// Procedure CheckFor_0x34()
//**************************************************************************************************

// Проверка ответа после нахождения в строке 0х34 //

bool Parse::CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd)      
{

    uint16_t msgLength = countLength(commandString.bytes[cmnd.SIZE], commandString.bytes[cmnd.SIZE + 1]);    
    if (msgLength > 6)                             //Считаем на сколько больше индекс строки с ответом, чем индекс строки с запросом по формуле: 
        msgLength = ceil((msgLength - 6) / 7) + 3; //(Длина_в_байтах - количество_полезных_данных_в_первой_строке)/количество_полезных_байт_в_остальных_строках + неучтённые_строки
    else                                        //Если длина меньше количества полезных байт в строке, то ответ будет на следующей строке
        msgLength = 1;

    Input += msgLength * STRING_LENGTH_ASCII;
    byteString responceString = StringToByte(Input);    //Перевели в байт-строку строку с ответом

    return (responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&
            responceString.bytes[cmnd.SIZE] < 7);         //Примечание: важное добавочное условие, т.к. основное условие, как оказалось, выполняется даже
}                                                       //данных внутри пакетов других команд 

//**************************************************************************************************
// Procedure CheckFor_0x36()
//**************************************************************************************************

// Аналогичная проверка ответа после нахождения в строке 0х36 //

bool Parse::CheckFor_0x36(byteString* commandString, CommandIndex cmnd)      
{
    uint16_t msgLength = countLength(commandString->bytes[cmnd.SIZE], commandString->bytes[cmnd.SIZE+1]);   
    if (msgLength > 6)
        msgLength = ceil((msgLength - 6) / 7) + 3;    
    else
        msgLength = 1;

    if((Length - 6) % 7 == 0)   //Если данные заканчиваются вплотную, то перейти на строку вверх
        msgLength -= 1;

                      
    byteString* responceString = commandString + msgLength + 1;      //строка с Positive Response
    byteString* subResponceString = responceString - 1;              //строка с запросом подождать
    return (responceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&        //0х36 заканчивается положительным ответом
            subResponceString->bytes[cmnd.SID_RESPONCE] == 0x7F);                       //Перед которым сервер просит подождать
}

//**************************************************************************************************
// Procedure CheckFor_0x76()
//**************************************************************************************************

// Проверка строки на конец пакета //

bool Parse::CheckFor_0x76 (byteString* responceString, CommandIndex cmnd)
{
    byteString* subResponceString = responceString + 1;
    return responceString->bytes[cmnd.SID_RESPONCE] == 0x7F &&
           subResponceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40;
}

//**************************************************************************************************
// Procedure CheckFor_0x37()
//**************************************************************************************************

// Проверка ответа после нахождения в строке 0х37 //

bool Parse::CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd)     
{
    byteString responceString = StringToByte(Input + STRING_LENGTH_ASCII);       //Адрес не высчитывается, т.к. на практике ответ сразу находится на следующей строке

    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&    
    responceString.bytes[cmnd.SIZE] < 7;
}

//**************************************************************************************************
// Procedure FromTxtTobyteString()
//**************************************************************************************************

// Выделение из файла *.txt структуры 0х34...0х37, помещение её в буфер, передача количества исходных строк через левостороннюю ссылку //

byteString* Parse::FromTxtTobyteString(const char* Path, uint32_t& DataStringNumber)
{
    char* Buffer = new char[MAX_BUFFER_SIZE] {'0'};     //Буфер для считывания символов из *.txt
    char* BufferPtr = Buffer;                           //Указатель на текущую позицию в буфере

    std::fstream File;
    File.open(Path);

    uint32_t StringNumber = 0;          //Количество строк в исходном файле

    while (File.getline(BufferPtr, STRING_LENGTH_ASCII, '\n'))
    {
        BufferPtr += STRING_LENGTH_ASCII;
        StringNumber++;
    }
    File.close();

    byteString* Data = new byteString[StringNumber] {emptybyteString};  //Возвращаемый буфер с данными,удовлетворяющими стркутуре 0х34...0х37
    byteString tempData {emptybyteString};      //Текущая строка, добавлена,т.к. часто используется в цикле ниже 
    BufferPtr = Buffer;   //Проходим буфер с символами с самого начала 

    bool IsDataTransferActive = false;  //Флаг, отвечающий за передачу строк из буфера в возвращаемый буфер
    uint32_t countData = 0;     //Количество строк в возвращаемом буфере

    for (uint32_t i = 0; i < StringNumber; i++)
    {
        tempData = StringToByte(BufferPtr);     //не рационально постоянно вызывать процедуру для перевода текущей строки в байт-строку, это делается заранее
        if (!IsDataTransferActive)               //Если ещё не начата передача, проверять на наличие структуры начала передачи 0х34...0х74
        {
            if (IsCommand(tempData, _0x34))
                IsDataTransferActive = CheckFor_0x34(BufferPtr, tempData, _0x34);
        }
        else                                    //Если передача начата, то проверять на наличие структуры окончания передачи 0х37...0х77
        {
            if (IsCommand(tempData, _0x37))
                IsDataTransferActive = !CheckFor_0x37(BufferPtr, tempData, _0x37);
        }


        if(IsDataTransferActive)                //Если передача активна, то считывать данные в возвращаемый буфер
        {
            Data[countData] = tempData;
            countData++;
        }

        BufferPtr += STRING_LENGTH_ASCII; 
    }

    delete Buffer;      //символьные строки больше нигде не используются

    DataStringNumber = countData;       //Передачадлины строки в переменную извне тела функции
    return Data;
}

//**************************************************************************************************
// Procedure FromTxtToBin()
//**************************************************************************************************

// Выбор полезных данныхизструктуры 0х34...0х37 //

int Parse::FromTxtToBin(const char* TxtPath, const char* BinPath)
{
    uint32_t DataStrLen = 0;
    uint32_t& DataStrLenPtr = DataStrLen;      //Количество строк в структурах 0х34...0х37
    byteString* Data = FromTxtTobyteString(TxtPath, DataStrLenPtr);     //Сами структуры
    byteString* DataPtr = Data;         //Указатель на текущую строку структур

    bool IsDataTransferActive = false;  //Флаг передачи данных по команде 0х36

    uint32_t StringCount = 0;   //Количество переданных строк за команду 0х36
    uint32_t PackageCount = 0;  //Количество переданных байт за команду 0х36
    uint32_t PackageLength = 0; //Общее количество передаваемых байт за команду 0х36

    FILE* out;                          //Объект выходного бинарного файла
    out = fopen(BinPath, "wb+");        //Файл перезаписывается, если существует, создаётся, если его нет

    for (uint32_t i = 0; i<DataStrLen; i++)
    {
        if (!IsDataTransferActive)
        {
            if (IsCommand(*DataPtr, _0x36))
                IsDataTransferActive = CheckFor_0x36(DataPtr, _0x36);
            if (IsDataTransferActive)
            {
                StringCount = 0;
                PackageCount = 0;
                PackageLength = countLength(DataPtr->bytes[_0x36.SIZE], DataPtr->bytes[_0x36.SIZE+1]);
                std::cout<<"SIZE: "<<PackageLength<<std::endl;
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

            switch (StringCount)      //Структура полезных данных команды 0х36 в строке зависит от номера строки:
            {                         // MS     MS      SID     N       D       D       D       D  -  0 строка 
                case 0:               // 30     0       0       0       0       0       0       0  -  1 строка
                    index = 4;        // K      D       D       D       D       D       D       D  -  k-ая строка
                    break;            //, где N - номер пакета, K - номер строки в пакете, D - любые данные
                case 1:
                    index = STRING_LENGTH_BYTE;
                    break;
                default:
                    index = 1;
                    break;
            }

            for(; index < STRING_LENGTH_BYTE; index++)  //Считываение пакетов в зависимотси от структуры выше
            {
                if(PackageCount + 2 == PackageLength)         //Нумерация текущих пакетов начинается с нуля, общего количества - с единицы
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

    delete Data;        //После записи в файл структура 0х34...0х37 уже не нужна

    fclose(out);        //Файл закрывается после записи данных из всех сессий
    return 0;
}
