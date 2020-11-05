#include "include/Parse.h"

int main()
{
    if(Parse::FromTxtToBin("Log.txt", "Log.bin"))
        std::cout<<"ERROR\n";

    return 0;
}
