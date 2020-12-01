//**************************************************************************************************
// Includes
//**************************************************************************************************

#include "include/Parse.h"

//**************************************************************************************************
// Procedure main() 
//**************************************************************************************************

//   Вывести в ./temp/ таблицу и список ID    //
int main(int argc, char *argv[])
{ 
    char DvividionSymbol = ' ';
    if(std::stoi(argv[2], 0, 10) != 0)
        DvividionSymbol = '\t'; 

    LogParser Log (argv[1], DvividionSymbol, std::stoi(argv[3], 0, 10), std::stoi(argv[4], 0, 10), std::stoi(argv[5], 0, 10), std::stoi(argv[6], 0, 10));

    Log.outputIDIntoFile();
    Log.outputTableIntoFile();

    return 0;
}