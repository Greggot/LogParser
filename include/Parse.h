//**************************************************************************************************
// Includes
//**************************************************************************************************
#include <fstream>
#include <stdint.h>
#include <string>
#include <cmath>
#include <stdio.h>
#include <iostream>

//**************************************************************************************************
// Definitions
//**************************************************************************************************

#define MAX_BUFFER_SIZE 0x2FFFFFFF   //767 MB - максимальный размер буфера
#define STRING_LENGTH_ASCII 24      //Длина строки в файле *.txt
#define STRING_LENGTH_BYTE 8        //Количество байт информации в строке

#define MAX_SUBSTRING_NUMBER 0x20
#define MAX_SUBSTRING_LENGTH 0x20
#define MAX_STRING_LENGTH_ASCII 0xFF

#define BYTE_NUMBER_POSITION 4
#define ID_POSITION 3
#define DATA_START_POSITION 6

#define SPACES_BEFORE_AFTER_DATA 9
#define COLUMNS_BEFORE_TIME 8

#define WHOLE_ID_LENGTH 8
#define ID_LENGTH 4
//**************************************************************************************************
// Structures and Data Types
//**************************************************************************************************

struct CommandIndex                 //Структура хранит положение элементов запроса-ответа в строках и занчение SID
{
    uint8_t SID_VALUE;
    uint8_t SIZE, SID, SUB_FUNC;
    uint8_t SIZE_RESPONCE, SID_RESPONCE, NRC_RESPONCE, SUB_FUNC_RESPONCE;
};

struct byteString           //Байт-строка
{
    uint8_t bytes[STRING_LENGTH_BYTE];
};

struct dataString
{
    uint8_t DataLen;
    std::string ID;
    std::string * Data;
    std::string TIME;
};


class Parser
{
    protected:

        //**************************************************************************************************
        // Declarations
        //**************************************************************************************************
        static void byteStringOut(byteString Out);
        static byteString StringToByte(std::string Input);
        static bool IsCommand (byteString Input, CommandIndex cmnd);

};

class LogParser
{
    private:
        static void dataStringOut(dataString out);

        static std::string* ListID(std::string IDsourse[], std::string ID, uint8_t& IDsNumber);
        dataString* ProcessLog (const char* path);

        dataString* LOG; 
        uint32_t logLength;
        std::string * IDs;
        uint8_t IDlen;
    public:
        dataString* getLOG(); 
        uint32_t getlogLength();
        std::string * getIDs();

        LogParser(const char* path);   //Обработать *.log или *.txt

        void outputIntoFile(std::string server, std::string client);
        void outputIntoFile();
};

class ParserDAF : public Parser
{
    private:
        static byteString* FromTxtToByte(const char* Path, uint32_t& DataLength);
        static bool checkForReset(byteString responceString, CommandIndex cmnd);
        static bool checkFor0x23(byteString responceString, CommandIndex cmnd);
    public:
        static int FromTxtToBin(const char* TxtPath, const char* BinPath);
};

class ParserVolvo : public Parser
{
    private:

        //**************************************************************************************************
        // Declarations
        //**************************************************************************************************

        static uint16_t countLength(uint8_t one, uint8_t two);
        static bool CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd);
        static bool CheckFor_0x36(byteString* commandString, CommandIndex cmnd);
        static bool CheckFor_0x76 (byteString* commandString, CommandIndex cmnd);
        static bool CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd);
        static byteString* FromTxtTobyteString(const char* Path, uint32_t& DataStringNumber);
    public:
        static int FromTxtToBin(const char* TxtPath, const char* BinPath);
};