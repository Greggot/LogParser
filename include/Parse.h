#include <fstream>
#include <stdint.h>
#include <string>
#include <cmath>
#include <stdio.h>
#include <iostream>

#define MAX_BUFFER_SIZE 0xFFFFFFF   //255 MB
#define STRING_LENGTH_ASCII 24
#define STRING_LENGTH_BYTE 8

struct CommandIndex
{
    uint8_t SID_VALUE;
    uint8_t SIZE, SID, SUB_FUNC;
    uint8_t SIZE_RESPONCE, SID_RESPONCE, NRC_RESPONCE, SUB_FUNC_RESPONCE;
};

struct byteString
{
    uint8_t bytes[STRING_LENGTH_BYTE];
};

class Parse
{
    private:
        void byteStringOut(byteString Out);

        static uint16_t countLength(uint8_t one, uint8_t two);
        static byteString StringToByte(std::string Input);
        static bool IsCommand (byteString Input, CommandIndex cmnd);
        static bool CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd);
        static bool CheckFor_0x36(byteString* commandString, CommandIndex cmnd);
        static bool CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd);
        static byteString* FromTxtTobyteString(const char* Path, uint32_t& DataStringNumber);
    public:
        static int FromTxtToBin(const char* TxtPath, const char* BinPath);
};
