#include "include/Parse.h"

int main()
{  
    const char* path  = "Log.log";
    LogParser Log (path);

    Log.outputIntoFile();
    Log.outputIntoFile("F9", "00");

    return 0;
}