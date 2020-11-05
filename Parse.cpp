#include "Parse.h"
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
    for(uint8_t i = 0; i<STRING_LENGTH_BYTE; i++)
        std::cout<<std::hex<<(int)Out.bytes[i]<<'\t';
    std::cout<<std::endl;
}
//**************************************************************************************************
// Procedure countLength()
//**************************************************************************************************

// Подсчёт длины сообщения //
uint16_t Parse::countLength(uint8_t one, uint8_t two)
{
    return (uint16_t)((one<<8 | two) & 0xFFF);  //Свдиг старшего значащего байта влево на байт и применение маски для стирания первого пол-байта
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

        uint8_t cycleLength = Input.size()+1;
        for(uint8_t i = 0; i<cycleLength; i++)
        {
            if(Input[i]=='\t' || Input[i]=='\x0')       //После каждого таба или конца строки 
            {
                Output.bytes[Count] = (uint8_t)std::stoi(buff, 0, 16);  //буфер переводится в шестнадцатеричное число и помещается в байт,
                buff = "";                                              //а затем очищается
                Count++;
            }
            buff+=Input[i];
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

    uint16_t msgLength = countLength(commandString.bytes[cmnd.SIZE], commandString.bytes[cmnd.SIZE+1]);    
    if(msgLength>6)                             //Считаем на сколько больше индекс строки с ответом, чем индекс строки с запросом по формуле: 
        msgLength = ceil ((msgLength-6)/7) + 3; //(Длина_в_байтах - количество_полезных_данных_в_первой_строке)/количество_полезных_байт_в_остальных_строках + неучтённые_строки
    else                                        //Если длина меньше количества полезных байт в строке, то ответ будет на следующей строке
        msgLength = 1;

    Input+= msgLength*STRING_LENGTH_ASCII;
    byteString responceString = StringToByte(Input);    //Перевели в байт-строку строку с ответом

    return (responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&
            responceString.bytes[cmnd.SIZE]<7);         //Примечание: важное добавочное условие, т.к. основное условие, как оказалось, выполняется даже
}                                                       //данных внутри пакетов других команд      
//**************************************************************************************************
// Procedure CheckFor_0x36()
//**************************************************************************************************

// Аналогичная проверка ответа после нахождения в строке 0х36 //
bool Parse::CheckFor_0x36(byteString* commandString, CommandIndex cmnd)      
{
    uint16_t msgLength = countLength(commandString->bytes[cmnd.SIZE], commandString->bytes[cmnd.SIZE+1]);   
    if(msgLength>6)
        msgLength = ceil ((msgLength-6)/7) + 3;    
    else
        msgLength = 1;

    commandString+= msgLength;                          //Ради экономии памяти сдвигаем командную строку на количество строк до строки с ответом
    byteString* responceString = commandString+1;       //Создаём вторую байт-строку, что на одну ниже строки с ответом т.к. обычно там находится
                                                        //положительный ответ на запрос
    return (commandString->bytes[cmnd.SID_RESPONCE] == 0x7F ||                  //Примечание: ИЛИ стоит потому, что, как оказалось, не все 0х36-запросы кончаются  
            responceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40);   //запросом подождать
}
//**************************************************************************************************
// Procedure CheckFor_0x37()
//**************************************************************************************************

// Проверка ответа после нахождения в строке 0х37 //
bool Parse::CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd)     
{
    byteString responceString = StringToByte(Input+STRING_LENGTH_ASCII);       //Адрес не высчитывается, т.к. на практике ответ сразу находится на следующей строке

    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&    
    responceString.bytes[cmnd.SIZE]<7;
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

    while(File.getline(BufferPtr, STRING_LENGTH_ASCII, '\n'))
    {
        BufferPtr+=STRING_LENGTH_ASCII;
        StringNumber++;
    }
    File.close();

    byteString* Data = new byteString[StringNumber] {emptybyteString};  //Возвращаемый буфер с данными,удовлетворяющими стркутуре 0х34...0х37
    byteString tempData {emptybyteString};      //Текущая строка, добавлена,т.к. часто используется в цикле ниже 
    BufferPtr=Buffer;   //Проходим буфер с символами с самого начала 

    bool IsDataTransferActive = false;  //Флаг, отвечающий за передачу строк из буфера в возвращаемый буфер
    uint32_t countData = 0;     //Количество строк в возвращаемом буфере

    for(uint32_t i = 0; i<StringNumber; i++)
    {
        tempData = StringToByte(BufferPtr);     //не рационально постоянно вызывать процедуру для перевода текущей строки в байт-строку, это делается заранее
        if(!IsDataTransferActive)               //Если ещё не начата передача, проверять на наличие структуры начала передачи 0х34...0х74
        {
            if(IsCommand(tempData, _0x34))
                IsDataTransferActive = CheckFor_0x34(BufferPtr, tempData, _0x34);
        }
        else                                    //Если передача начата, то проверять на наличие структуры окончания передачи 0х37...0х77
        {
            if(IsCommand(tempData, _0x37))
                IsDataTransferActive = !CheckFor_0x37(BufferPtr, tempData, _0x37);
        }


        if(IsDataTransferActive)                //Если передача активна, то считывать данные в возвращаемый буфер
        {
            Data[countData] = tempData;
            countData++;
        }

        BufferPtr+=STRING_LENGTH_ASCII; 
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
    uint32_t DataStrLen = 0; uint32_t& DataStrLenPtr = DataStrLen;      //Количество строк в структурах 0х34...0х37
    byteString* Data = FromTxtTobyteString(TxtPath, DataStrLenPtr);     //Сами структуры
    byteString* DataPtr = Data;         //Указатель на текущую строку структур

    bool IsDataTransferActive = false;  //Флаг передачи данных по команде 0х36

    uint32_t StringCount = 0;   //Количество переданных строк за команду 0х36
    uint32_t PackageCount = 0;  //Количество переданных байт за команду 0х36
    uint32_t PackageLength = 0; //Общее количество передаваемых байт за команду 0х36

    FILE* out;                          //Объект выходного бинарного файла
    out = fopen(BinPath, "wb+");        //Файл перезаписывается, если существует, создаётся, если его нет

    for(uint32_t i = 0; i<DataStrLen; i++)       
    {
        if(!IsDataTransferActive)       //Если не передаются данные по 0х36, то проверяем на наличие команды-ответа
        {
            if(IsCommand(*DataPtr, _0x36))
            {
                IsDataTransferActive = CheckFor_0x36(DataPtr, _0x36);
                if(IsDataTransferActive)              //Если найдена команда, обнулить счётчики, посчитать длину сообщения
                {
                    StringCount = 0;
                    PackageCount = 0;
                    PackageLength = countLength(DataPtr->bytes[_0x36.SIZE], DataPtr->bytes[_0x36.SIZE+1]);
                    DataPtr--;  //после нахождения запроса, отвечающий за его обработку else пропустится одну итерацию, 
                }               //так что нужно заранее сдвинуться на строку назад
            }
        }
        else    //Если всёже идёт передача
        {
            if(PackageCount < PackageLength)    //Пока количество переданных на момент итерации данных меньше общего количества
            {
                uint8_t index;  //Байт, с которого считываются полезные данные до конца строки

                switch(StringCount) //index зависит от строки, с которой считываютсяданные
                {
                    case 0:     //Если это первая строка, то считываем всё, что дальше SubFunction
                        index = 4;
                        break;
                    case 1:     //Если вторая строка (30 0 0 0 0 0 0 0), то не считываем вообще
                        index = STRING_LENGTH_BYTE;
                        break;
                    default:    //Если все остальные строки пакета, то считываем со второго байта
                        index = 1;
                        break;
                }
                      
                for(; index<STRING_LENGTH_BYTE; index++) 
                {
                    if(PackageCount+2 == PackageLength)         //Нумерация текущих пакетов начинается с нуля, общего количества - с единицы
                    {                                           //Потому если следущая строка является концом пакета, то закончить передачу
                        IsDataTransferActive = false;
                        break;
                    }
                    fwrite(&DataPtr->bytes[index], 1, 1, out);  //Передача - помещение полезных байтов в бинарный файл
                    PackageCount++;
                }
                StringCount++;
            }
        }
        DataPtr++;
    }

    delete Data;        //После записи в файл структура 0х34...0х37 уже не нужна

    fclose(out);        //Файл закрывается после записи данных из всех сессий
    return 0;
}
