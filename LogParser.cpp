#include "include/Parse.h"

LogParser::LogParser(const char* path)
{ 
    this->LOG = ProcessLog(path);

    std::cout << this->logLength << " " << (int) this->IDlen << std::endl;

    system("pause");
}

std::string* LogParser::ListID(std::string IDsourse[], std::string ID, uint8_t& IDsNumber)
{
    bool IsAlreadyHere = false;

    for (uint8_t i = 0; i < IDsNumber; i++)
    {
        if (IDsourse[i] == ID)
            IsAlreadyHere = true;
    }

    if(!IsAlreadyHere)
    {
        IDsNumber++;
        std::string * newIDsource = new std::string [IDsNumber] {""};
        newIDsource = IDsourse;
        newIDsource[IDsNumber - 1] = ID;

        return newIDsource;
    }
    else
        return IDsourse;
}

void LogParser::dataStringOut(dataString out)
{
    std::cout << out.ID << "\t";

    for (uint8_t i = 0; i < out.DataLen; i++)
        std::cout << out.Data[i] << "\t";
    if(out.DataLen < STRING_LENGTH_BYTE)
    {
        uint8_t differense = STRING_LENGTH_BYTE - out.DataLen;
        for (uint8_t i = 0; i < differense; i++)
            std::cout << "\t";
    }

    std::cout << out.TIME << std::endl;
}

void LogParser::outputIntoFile(std::string server, std::string client)
{
    std::string outPath = "temp/LOG" + server + client + ".txt";

    std::ofstream LogFile;
    LogFile.open(outPath);

    for (uint32_t i = 0; i < logLength; i++)
    {  
        size_t positionServer = LOG[i].ID.find(server);
        size_t positionClient = LOG[i].ID.find(client);
        
        if (positionServer != std::string::npos && positionClient != std::string::npos
        && positionServer > 3 && positionClient > 3)
        {
            LogFile << LOG[i].ID << '\t';

            for (uint8_t j = 0; j < LOG[i].DataLen; j++)
                LogFile << LOG[i].Data[j] << "\t";

            if(LOG[i].DataLen < STRING_LENGTH_BYTE)
            {
                uint8_t differense = STRING_LENGTH_BYTE - LOG[i].DataLen;
                for (uint8_t j = 0; j < differense; j++)
                    LogFile << "\t";
            }

            LogFile << LOG[i].TIME << "\n";
        }
    }

    LogFile.close();
}


void LogParser::outputIntoFile()
{
    std::ofstream LogFile;
    LogFile.open("temp/LOG.txt");

    for (uint32_t i = 0; i < logLength; i++)
    {
        LogFile << LOG[i].ID << '\t';

        for (uint8_t j = 0; j < LOG[i].DataLen; j++)
            LogFile << LOG[i].Data[j] << "\t";

        if(LOG[i].DataLen < STRING_LENGTH_BYTE)
        {
            uint8_t differense = STRING_LENGTH_BYTE - LOG[i].DataLen;
            for (uint8_t j = 0; j < differense; j++)
                LogFile << "\t";
        }

        LogFile << LOG[i].TIME << "\n";
    }

    LogFile.close();
}

dataString emptydataString = {0, {0}, {0}, {0}};

dataString* LogParser::ProcessLog(const char* path)
{  
    char *Buffer = new char[MAX_BUFFER_SIZE] {0};
    char* BuffPtr = Buffer;

    dataString* LOG = new dataString [MAX_BUFFER_SIZE / MAX_STRING_LENGTH_ASCII];
    for (uint32_t i = 0; i < MAX_BUFFER_SIZE / MAX_STRING_LENGTH_ASCII; i++)
        LOG[i] = emptydataString;

    std::string * IDs = new std::string [0xFF] {""};

    uint8_t subStringCounter = 0;
    uint8_t DataLength = 0;
    uint16_t i = 0;
    uint32_t stringCount = 0;
    uint8_t idCount = 0; 
    uint8_t& idCountRef = idCount;
    std::string buff = "";

    std::fstream File;
    File.open(path);

    while (File.getline(BuffPtr, MAX_STRING_LENGTH_ASCII, '\n'))
    {
        std::string * String = new std::string [MAX_SUBSTRING_NUMBER] {""};
        subStringCounter = 0;
        i = 0;
        bool isSpace = false;

        while (BuffPtr[i] != '\x0')
        {
            isSpace = BuffPtr[i] == ' ';
            while(BuffPtr[i] == ' ')
            {
                i++;
            }
            if(isSpace)
            {
                String[subStringCounter] = buff;
                subStringCounter++;
                buff = "";
            }
            buff += BuffPtr[i];
            i++;
        }
        buff = "";

        DataLength = std::stoi(String[BYTE_NUMBER_POSITION], 0, 10);
        std::string * temp = new std::string [DataLength] {""};

        for (uint8_t j = 0; j < DataLength; j++)    //Это в файл
            temp[j] = String[DATA_START_POSITION + j];

        LOG[stringCount] = {DataLength, String[ID_POSITION], temp,  String[DataLength + COLUMNS_BEFORE_TIME]};
        IDs = LogParser::ListID(IDs, LOG[stringCount].ID, idCountRef);

        stringCount++;
        BuffPtr += MAX_STRING_LENGTH_ASCII;
    }
    std::ofstream IDsFile;
    IDsFile.open("temp/ID.txt");

    for (uint8_t i = 0; i < idCount; i++)
        IDsFile << IDs[i] << '\n';

    IDsFile.close();
    File.close();

    delete Buffer;

    this->logLength = (int) stringCount;
    this->IDs = IDs;
    this->IDlen = (int)idCount;

    return LOG;
}


dataString* LogParser::getLOG()
{
    return LOG;
} 
uint32_t LogParser::getlogLength()
{
    return logLength;
}
std::string * LogParser::getIDs()
{
    return IDs;
}