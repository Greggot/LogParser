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

LogParser::LogParser(const char* path, char DividionSymbol, uint8_t timePos, uint8_t IDpos, uint8_t dataLenPos, uint8_t dataPos)
{ 

    this->LOG = ProcessLog(path, DividionSymbol, timePos, IDpos, dataLenPos, dataPos);   // Процедура считывает лог, вовращает буффер структур, внутри неё инициалиируются остальные элементы класса

    Parser::dataStringOut(LOG[0]);
    std::cout << "...\n";
    Parser::dataStringOut(LOG[logLength - 1]);
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

    Parser::byteStringOut(out.Data);
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
                    LogFile << std::uppercase << std::hex << (int) LOG[i].Data.bytes[j] << "\t";  // В остальном запись в оба файла аналогична выводу структуры в консоль
                    LogFileSave << std::uppercase << std::hex << (int) LOG[i].Data.bytes[j] << "\t";
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
                LogFile << std::uppercase << std::hex << (int) LOG[i].Data.bytes[j] << "\t";
                LogFileSave << std::uppercase << std::hex << (int) LOG[i].Data.bytes[j] << "\t";
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
                    LogFile << std::uppercase << std::hex << (int) LOG[i].Data.bytes[j];
                    
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

dataString* LogParser::ProcessLog(const char* path, char DividionSymbol, uint8_t timePos, uint8_t IDpos, uint8_t dataLenPos, uint8_t dataPos)     // На выход подаётся структура, ID-DATA-TIME
{  
    uint32_t stringCount = 0;
    dataString* LOG  = Parser::ReadDataString(path, DividionSymbol, timePos, IDpos, dataLenPos, dataPos, stringCount);
    uint8_t idCount = 0;
    std::vector <std::string> IDs;

    for (uint32_t i = 0; i < stringCount; i++)
    {
        bool isAlreadyhere = false;
        for (uint8_t j = 0; j < IDs.size(); j++)
        {
            if (LOG[i].ID == IDs[j])
                isAlreadyhere = true;
        }
        if(!isAlreadyhere)
        {
            IDs.push_back(LOG[i].ID);
            idCount++;
        }

    }
    std::string* ID = new std::string[idCount] {""};
    
    for (uint8_t i = 0; i < idCount; i++)
    {
        std::cout << IDs[i] << std::endl;
        ID[i] = IDs[i];
    }

    this->logLength = stringCount;    // Инициализировать количество строк лога
    this->IDs = ID;                        // список ID
    this->IDlen = idCount;             // и количество ID

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
