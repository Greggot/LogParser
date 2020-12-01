#include "include/Parse.h"

//**************************************************************************************************
// Procedure main()
//**************************************************************************************************

//   Вывод файлов, содержащих данные обмена сообщениями между двумя ID   //

int main(int argc, char *argv[])
{  
    LogParser Log (argv[1]);    // Создание объекта, состоящего из dataString и списка ID

    uint8_t stringNumber = argc - 1;
    std::string* argvStrings = new std::string [stringNumber];
    for (uint8_t i = 0; i < stringNumber; i++)
    {
        argvStrings[i] = argv[i + 1];
    }

    Log.outputTableIntoFile(argvStrings, stringNumber);  // Вывести таблицу ID-DATA-TIME в виде текстового файла 
    Log.oututDataIntoFile(argvStrings, stringNumber);    // Вывести текстовый файл, формата, подходящего для парсинга

    return 0;
}