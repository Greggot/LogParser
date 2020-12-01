//**************************************************************************************************
// Includes
//**************************************************************************************************

#include "include/Parse.h"

//**************************************************************************************************
// Global Variables
//**************************************************************************************************

dataString emptydataString = {0, {0}, {0}, {0}};    // Пустая дата-строка 

//**************************************************************************************************
// Procedure LogParser() 
//**************************************************************************************************

//   Конструктор, задающий буфер дата-строк, список идентификаторов   //

LogParser::LogParser(const char* path)
{ 
    this->LOG = ProcessLog(path);   // Процедура считывает лог, вовращает буффер структур, внутри неё инициалиируются остальные элементы класса
}

//**************************************************************************************************
// Procedure ListID() 
//**************************************************************************************************

//   Инициализация множества {ID}   //

std::string* LogParser::ListID(std::string IDsourse[], std::string ID, uint8_t& IDsNumber)  // Процедура рекурсивно принимает множество и добавляет в него новый элемент
{
    bool IsAlreadyHere = false;     // Флаг повторяюшегося значения

    for (uint8_t i = 0; i < IDsNumber; i++)     // Пройти все элементы. Если найдётся такое же ID, уствновить флаг
    {
        if (IDsourse[i] == ID)
            IsAlreadyHere = true;
    }

    if(!IsAlreadyHere)      // Если не установлен флфг, значит, нужно добавить новый ID в уже существующее множество 
    {
        IDsNumber++;        // Увеличить значение ссылочного типа на один, чтобы извне можно было видеть количество ID
        std::string * newIDsource = new std::string [IDsNumber] {""};   // Создать новый буфер 
        newIDsource = IDsourse;                                         // Скопировтаь в него старый
        newIDsource[IDsNumber - 1] = ID;                                // Добавить в конец текущий проверенный ID

        return newIDsource;
    }
    else
        return IDsourse;
}

//**************************************************************************************************
// Procedure dataStringOut() 
//**************************************************************************************************

//   Вывод структуры в консоль   //

void LogParser::dataStringOut(dataString out)   // Аналогичным образом структура выводится в файл
{
    std::cout << out.ID << "\t";        // Вывести ID

    for (uint8_t i = 0; i < out.DataLen; i++)   // Вывести все полезные данные
        std::cout << out.Data[i] << "\t";
    if(out.DataLen < STRING_LENGTH_BYTE)        // Если количество байт меньше образцовой строки...
    {
        uint8_t differense = STRING_LENGTH_BYTE - out.DataLen;
        for (uint8_t i = 0; i < differense; i++)
            std::cout << "\t";                  //...заполнить оставшееся место символами табуляци
    }

    std::cout << out.TIME << std::endl;     // Вывести время
}

//**************************************************************************************************
// Notation
//**************************************************************************************************

/*   Последующие функции именуют файлы для сохранения во временной папке ./temp/ последующим образом:
-data-"LOG"-temp-save-".txt", где  data - метка о том, что в файле находятся только байты данных. Без неё в файле вся таблица
                                            temp - метка нахождения в файле только некоторых выбранных ID
                                            save - метка формата, подходящего для парсинга. Файлы бе неё используются для отображения 
*/

//**************************************************************************************************
// Procedure outputTableIntoFile() 
//**************************************************************************************************

//   Вывод таблицы для n ID-шников   //

void LogParser::outputTableIntoFile(std::string* argv, uint8_t argc)    //Передаётся список ID, который проходится полностью. В другой векте передаются только два ID, в этой - n
{
    std::string outPath = "temp/LOGtemp.txt";        // Создаются два временных файла, один и которых нужен для отображения на экране с корректной табуляцией
    std::string outPathSave =  "temp/LOGtempsave.txt";   // Второй для сохранения, потому что каталог временный и очищается после завершения программы

    std::ofstream LogFile;      
    LogFile.open(outPath);

    std::ofstream LogFileSave;
    LogFileSave.open(outPathSave);

    for (uint32_t i = 0; i < logLength; i++)    // Пройти весь лог и объекта класса
    {       
        for (uint8_t j = 0; j < argc; j++)      // Пройти все заданные ID
        {
            if (LOG[i].ID == argv[j])     // Если ID лога равно одному из заданных ID, то...
            {
                LogFileSave << LOG[i].ID << '\t';      // Записать в файл сохранения 
                LogFile << LOG[i].ID << '\t' << '\t';   // Записать в файл отображения с одной лишней табуляцией, ибо иначе никак не получется отобразить 
                                                        //в виджете корректно без лишнего символа
                for (uint8_t j = 0; j < LOG[i].DataLen; j++)
                {
                    LogFile << LOG[i].Data[j] << "\t";  // В остальном запись в оба файла аналогична выводу структуры в консоль
                    LogFileSave << LOG[i].Data[j] << "\t";
                }
                if(LOG[i].DataLen < STRING_LENGTH_BYTE)
                {
                    uint8_t differense = STRING_LENGTH_BYTE - LOG[i].DataLen;
                    for (uint8_t j = 0; j < differense; j++)
                    {
                        LogFile << "\t";
                        LogFileSave << '\t';
                    }
                }

                LogFile << LOG[i].TIME << "\n";
                LogFileSave << LOG[i].TIME << "\n";
            }
        }
    }
    LogFileSave.close();
    LogFile.close();
}

//**************************************************************************************************
// Procedure outputTableIntoFile() 
//**************************************************************************************************

//   Вывод всего лога в файл   //

void LogParser::outputTableIntoFile()       // Функция, аналогично, описанной выше, но...
{
    std::ofstream LogFile;
    LogFile.open("temp/LOG.txt");

    std::string outPathSave =  "temp/LOGsave.txt";

    std::ofstream LogFileSave;
    LogFileSave.open(outPathSave);

    for (uint32_t i = 0; i < logLength; i++)
    {
        LogFileSave << LOG[i].ID << '\t';       // ...нет проверки на соответствие ID
        LogFile << LOG[i].ID << '\t' << '\t';

        for (uint8_t j = 0; j < LOG[i].DataLen; j++)
            {
                LogFile << LOG[i].Data[j] << "\t";
                LogFileSave << LOG[i].Data[j] << "\t";
            }
            if(LOG[i].DataLen < STRING_LENGTH_BYTE)
            {
                uint8_t differense = STRING_LENGTH_BYTE - LOG[i].DataLen;
                for (uint8_t j = 0; j < differense; j++)
                {
                    LogFile << "\t";
                    LogFileSave << '\t';
                }
            }

        LogFile << LOG[i].TIME << "\n";
        LogFileSave << LOG[i].TIME << "\n";
    }

    LogFile.close();
}

//**************************************************************************************************
// Procedure oututDataIntoFile() 
//**************************************************************************************************

//   Вывод байт данных в файл   //

void LogParser::oututDataIntoFile(std::string* argv, uint8_t argc)       // Аналогично выводу в файл таблицы взаимодействия сервера-клиента, но
{         
                                                                        // в сам файл не выводятся ID и TIME
    std::string outPath = "temp/dataLOGtemp.txt";

    std::ofstream LogFile;
    LogFile.open(outPath);

    for (uint32_t i = 0; i < logLength; i++)
    { 
        for (uint8_t j = 0; j < argc; j++)
        {
            if (LOG[i].ID == argv[j])      
            {                                                   
                for (uint8_t j = 0; j < LOG[i].DataLen; j++)
                {
                    if (LOG[i].Data[j][0] != '0')       // Первый незначащий нуль игнорируется, удобно для чтения, похоже на формат, выдаваемый Excel 
                        LogFile << LOG[i].Data[j];
                    else 
                        LogFile << LOG[i].Data[j][1];
                    
                    if (j != LOG[i].DataLen - 1)
                        LogFile <<"\t";
                    
                }
                if(LOG[i].DataLen < STRING_LENGTH_BYTE)
                {
                    uint8_t differense = STRING_LENGTH_BYTE - LOG[i].DataLen;
                    for (uint8_t j = 0; j < differense; j++)
                        LogFile << "\t";
                }

                LogFile << '\n';
            }
        }
    }

    LogFile.close();
}

//**************************************************************************************************
// Procedure outputIDIntoFile() 
//**************************************************************************************************

//   Вывод списка ID в файл   //

void LogParser::outputIDIntoFile()
{
    std::ofstream IDsFile;
    IDsFile.open("temp/ID.txt");    // Единственный файл с оригинальным названием

    for (uint8_t i = 0; i < IDlen; i++)     // Вывод всего элемента class LogParser::IDs 
        IDsFile << IDs[i] << '\n';

    IDsFile.close();
}

//**************************************************************************************************
// Procedure ProcessLog() 
//**************************************************************************************************

//   Обработка фалйа *.log   //

dataString* LogParser::ProcessLog(const char* path)     // На выход подаётся структура, ID-DATA-TIME
{  
    char *Buffer = new char[MAX_BUFFER_SIZE] {0};       // Буфер под содержимое файла
    char* BuffPtr = Buffer;

    dataString* LOG = new dataString [MAX_BUFFER_SIZE / MAX_STRING_LENGTH_ASCII];   // Выходная структура
    for (uint32_t i = 0; i < MAX_BUFFER_SIZE / MAX_STRING_LENGTH_ASCII; i++)
        LOG[i] = emptydataString;

    std::string * IDs = new std::string [0xFF] {""};    // Выходной список ID

    uint8_t subStringCounter = 0;       // Счётчик элементов подстроки
    uint8_t DataLength = 0;             // Счётчик количества байт даннх в строке
    uint16_t i = 0;                     // Счётчик количества пройденных символов в строке ASCII
    uint32_t stringCount = 0;           // Счётчик обработанных строк
    uint8_t idCount = 0;                // Мощность множества {ID}
    uint8_t& idCountRef = idCount;      // Ссылка, передаваемая процедуре ListID
    std::string buff = "";              // Буфер, в который помещается элемент строки

    std::fstream File;
    File.open(path);

    while (File.getline(BuffPtr, MAX_STRING_LENGTH_ASCII, '\n'))    // Считать строку
    {
        std::string * String = new std::string [MAX_SUBSTRING_NUMBER] {""};
        subStringCounter = 0;
        i = 0;
        bool isSpace = false;   // Флаг наличия пробела, окончания элемента строки

        while (BuffPtr[i] != '\x0') // Пока строка не закончилась
        {
            isSpace = BuffPtr[i] == ' ';    
            while(BuffPtr[i] == ' ')    // Пропуск последовательных пробелов
            {
                i++;
            }
            if(isSpace)     // Если текущий элемент закончился пробелом
            {
                String[subStringCounter] = buff;    // Занести его в буфер строки
                subStringCounter++;                 
                buff = "";                      // Обнулить буфер элемента
            }   
            buff += BuffPtr[i];         // Считать следующий символ элемента 
            i++;
        }
        buff = "";      // После окончания строки нужно очистить буфер

        DataLength = std::stoi(String[BYTE_NUMBER_POSITION], 0, 10);    // Перевести строковый элемент с количеством байт данных в число
        std::string * temp = new std::string [DataLength] {""};         // Буфер байт данных строки

        for (uint8_t j = 0; j < DataLength; j++)    // Заполнить его байтами
            temp[j] = String[DATA_START_POSITION + j];

        LOG[stringCount] = {DataLength, String[ID_POSITION], temp,  String[DataLength + COLUMNS_BEFORE_TIME]};  // Инициализировать структуру строки
        IDs = LogParser::ListID(IDs, LOG[stringCount].ID, idCountRef);      // Добавить новый встретившийся ID в строке

        stringCount++;
        BuffPtr += MAX_STRING_LENGTH_ASCII;     // Сместить буфер на строку
    }
    File.close();
    delete Buffer;

    this->logLength = (int) stringCount;    // Инициализировать количество строк лога
    this->IDs = IDs;                        // список ID
    this->IDlen = (int)idCount;             // и количество ID

    return LOG;
}

//**************************************************************************************************
// Procedure getLOG() 
//**************************************************************************************************

//   Возвращает буффер структур   //

dataString* LogParser::getLOG()
{
    return LOG;
}

//**************************************************************************************************
// Procedure getlogLength() 
//**************************************************************************************************

//   Возвращает количество строк лога   //

uint32_t LogParser::getlogLength()
{
    return logLength;
}

//**************************************************************************************************
// Procedure getIDs() 
//**************************************************************************************************

//   Возвращает список ID   //

std::string * LogParser::getIDs()
{
    return IDs;
}
