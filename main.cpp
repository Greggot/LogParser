#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

#define MAX_BUFFER_SIZE 0x2FFFFFFF
#define MAX_SUBSTRING_NUMBER 0x20
#define MAX_SUBSTRING_LENGTH 0x20
#define MAX_STRING_LENGTH_ASCII 0xFF

#define BYTE_NUMBER_POSITION 4
#define ID_POSITION 3
#define DATA_START_POSITION 6

#define SPACES_BEFORE_AFTER_DATA 9
#define COLUMNS_BEFORE_TIME 8

int main()
{  
    char *Buffer = new char[MAX_BUFFER_SIZE] {0};
    char* BuffPtr = Buffer;

    uint32_t stringCount = 0;
    std::string buff = "";

    std::fstream File;
    File.open("Log.log");

    std::string ** Log = new std::string * [MAX_BUFFER_SIZE];
    uint8_t subStringCounter = 0;

    while (File.getline(BuffPtr, MAX_STRING_LENGTH_ASCII, '\n'))
    {
        std::string * String = new std::string [MAX_SUBSTRING_NUMBER];
        subStringCounter = 0;

        uint16_t i = 0;
        bool isSpace = false;

        while (BuffPtr[i] != '\x0')
        {
            isSpace = BuffPtr[i] == ' ';
            
            while(BuffPtr[i] == ' ' || BuffPtr[i] == '\x0' || BuffPtr[i] == '\t')
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

        Log[stringCount] = String;
        
        stringCount++;
        BuffPtr += MAX_STRING_LENGTH_ASCII;
    }
    
    for (uint32_t i = 0; i < stringCount; i++)  //Всё вывести вприложение
    {
        uint8_t DataLength = std::stoi(Log[i][BYTE_NUMBER_POSITION], 0, 10);

        std::cout << Log[i][ID_POSITION] << " ";
        for (uint8_t j = 0; j < DataLength; j++)    //Это в файл
            std::cout << Log[i][DATA_START_POSITION + j] << " ";

        std::cout << Log[i][DataLength + COLUMNS_BEFORE_TIME];
        std::cout << std::endl;
    }

    system("pause");

    return 0;
}