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

class Parse
{
    private:

        //**************************************************************************************************
        // Declarations
        //**************************************************************************************************

        static void byteStringOut(byteString Out);

        static uint16_t countLength(uint8_t one, uint8_t two);
        static byteString StringToByte(std::string Input);
        static bool IsCommand (byteString Input, CommandIndex cmnd);
        static bool CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd);
        static bool CheckFor_0x36(byteString* commandString, CommandIndex cmnd);
        static bool CheckFor_0x76 (byteString* commandString, CommandIndex cmnd);
        static bool CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd);
        static byteString* FromTxtTobyteString(const char* Path, uint32_t& DataStringNumber);
    public:
        static int FromTxtToBin(const char* TxtPath, const char* BinPath);
};
