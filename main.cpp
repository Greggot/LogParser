#include "include/Parse.h"

int main(int argc, char *argv[])
{
    if(Parse::FromTxtToBin(argv[1], argv[2]))
        std::cout<<"ERROR\n";
    system("pause");
    return 0;
}
