#include "include/Parse.h"

//**************************************************************************************************
// Procedure main()
//**************************************************************************************************

//   Вывод файлов, содержащих данные обмена сообщениями между двумя ID   //

int main(int argc, char *argv[])
{  
    char DvividionSymbol = ' ';
    if(std::stoi(argv[2], 0, 10) != 0)
        DvividionSymbol = '\t'; 
    
    LogParser Log (argv[1], DvividionSymbol, std::stoi(argv[3], 0, 10), std::stoi(argv[4], 0, 10), std::stoi(argv[5], 0, 10), std::stoi(argv[6], 0, 10));

    uint8_t stringNumber = argc - 6;
    std::string* argvStrings = new std::string [stringNumber];
    for (uint8_t i = 0; i < stringNumber; i++)
    {
        argvStrings[i] = argv[i + 6];
    }

    Log.outputTableIntoFile(argvStrings, stringNumber);  // Вывести таблицу ID-DATA-TIME в виде текстового файла 
    Log.oututDataIntoFile(argvStrings, stringNumber);    // Вывести текстовый файл, формата, подходящего для парсинга

    return 0;
}