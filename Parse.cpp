#include "Parse.h"

byteString emptybyteString {{0,0,0,0,0,0,0,0}};

        CommandIndex _0x34 {0x34,
                            0, 2, 3,
                            0, 1, 2, 3};
        CommandIndex _0x36 {0x36,
                            0, 2, 3,
                            0, 1, 2, 3};
        CommandIndex _0x37 {0x37,
                            0, 1, 2,
                            0, 1, 2, 3};

void Parse::byteStringOut(byteString Out)
{
    for(uint8_t i = 0; i<STRING_LENGTH_BYTE; i++)
        std::cout<<std::hex<<(int)Out.bytes[i]<<'\t';
    std::cout<<std::endl;
}

uint16_t Parse::countLength(uint8_t one, uint8_t two)
{
    return (uint32_t)((one<<8 | two) & 0xFFF);
}

byteString Parse::StringToByte(std::string Input)          //Строка разбивается на подстроки, а потом при помощи std::stoi переводится в 0xuint8_t
{
        byteString Output = emptybyteString;

        uint8_t Count = 0;
        std::string buff = "";

        uint8_t cycleLength = Input.size()+1;
        for(uint8_t i = 0; i<cycleLength; i++)
        {
            if(Input[i]=='\t' || Input[i]=='\x0')
            {
                Output.bytes[Count] = (uint8_t)std::stoi(buff, 0, 16);
                buff = "";
                Count++;
            }
            buff+=Input[i];
        }
        return Output;
}

bool Parse::IsCommand (byteString Input, CommandIndex cmnd)
{
    return Input.bytes[cmnd.SID] == cmnd.SID_VALUE;
}

bool Parse::CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd)      //Является ли строка началом серии пакетов
{

    uint16_t msgLength = countLength(commandString.bytes[cmnd.SIZE], commandString.bytes[cmnd.SIZE+1]);       //Задать в CommandIndex длину адреса, чтобы унифицировать для всех команд
    if(msgLength>6)
        msgLength = ceil ((msgLength-6)/7) + 3;     //3, т.к. не считаем эту строку, с 30 0 0 0 0 0 0 0
    else
        msgLength = 1;

    Input+= msgLength*STRING_LENGTH_ASCII;
    byteString responceString = StringToByte(Input);

    return (responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&
            responceString.bytes[cmnd.SIZE]<7);
}

bool Parse::CheckFor_0x36(byteString* commandString, CommandIndex cmnd)      //Является ли строка началом серии пакетов
{
    uint16_t msgLength = countLength(commandString->bytes[cmnd.SIZE], commandString->bytes[cmnd.SIZE+1]);       //Задать в CommandIndex длину адреса, чтобы унифицировать для всех команд
    if(msgLength>6)
        msgLength = ceil ((msgLength-6)/7) + 3;     //3, т.к. не считаем эту строку, с 30 0 0 0 0 0 0 0
    else
        msgLength = 1;

    commandString+= msgLength;
    byteString* responceString = commandString+1;

    return (commandString->bytes[cmnd.SID_RESPONCE] == 0x7F ||
            responceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40);
}

bool Parse::CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd)     //Является ли концом серии пакетов
{
    byteString responceString = StringToByte(Input+STRING_LENGTH_ASCII);

    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&
    responceString.bytes[cmnd.SIZE]<7;
}

byteString* Parse::FromTxtTobyteString(const char* Path, uint32_t& DataStringNumber)
{
    char* Buffer = new char[MAX_BUFFER_SIZE] {'0'};
    char* BufferPtr = Buffer;

    std::fstream File;
    File.open(Path);

    uint32_t StringNumber = 0;

    while(File.getline(BufferPtr, STRING_LENGTH_ASCII, '\n'))
    {
        BufferPtr+=STRING_LENGTH_ASCII;
        StringNumber++;
    }
    File.close();

    byteString* Data = new byteString[StringNumber] {emptybyteString};
    byteString tempData {emptybyteString};
    BufferPtr=Buffer;

    bool IsDataTransferActive = false;
    uint32_t countData = 0;

    for(uint32_t i = 0; i<StringNumber; i++)
    {
        tempData = StringToByte(BufferPtr);
        if(!IsDataTransferActive)
        {
            if(IsCommand(tempData, _0x34))
                IsDataTransferActive = CheckFor_0x34(BufferPtr, tempData, _0x34);
        }
        else
        {
            if(IsCommand(tempData, _0x37))
                IsDataTransferActive = !CheckFor_0x37(BufferPtr, tempData, _0x37);
        }


        if(IsDataTransferActive)
        {
            Data[countData] = tempData;
            countData++;
        }

        BufferPtr+=STRING_LENGTH_ASCII;
    }

    delete Buffer;

    DataStringNumber = countData;
    return Data;
}

int Parse::FromTxtToBin(const char* TxtPath, const char* BinPath)
{
    uint32_t DataStrLen = 0; uint32_t& DataStrLenPtr = DataStrLen;
    byteString* Data = FromTxtTobyteString(TxtPath, DataStrLenPtr);
    byteString* DataPtr = Data;

    bool IsDataTransferActive = false;

    uint32_t StringCount = 0;
    uint32_t PackageCount = 0;
    uint32_t PackageLength = 0;

    FILE* out;
    out = fopen(BinPath, "wb+");

    for(uint32_t i = 0; i<DataStrLen; i++)
    {
        if(!IsDataTransferActive)
        {
            if(IsCommand(*DataPtr, _0x36))
            {
                IsDataTransferActive = CheckFor_0x36(DataPtr, _0x36);
                if(IsDataTransferActive)
                {
                    StringCount = 0;
                    PackageCount = 0;
                    PackageLength = countLength(DataPtr->bytes[_0x36.SIZE], DataPtr->bytes[_0x36.SIZE+1]);
                    DataPtr--;
                }
            }
        }
        else
        {
            if(PackageCount < PackageLength)
            {
                uint8_t index;

                switch(StringCount)
                {
                    case 0:
                        index = 4;
                        break;
                    case 1:
                        index = STRING_LENGTH_BYTE;
                        break;
                    default:
                        index = 1;
                        break;
                }

                for(; index<STRING_LENGTH_BYTE; index++)
                {
                    if(PackageCount+2 == PackageLength)
                    {
                        IsDataTransferActive = false;
                        break;
                    }
                    fwrite(&DataPtr->bytes[index], 1, 1, out);
                    PackageCount++;
                }
                StringCount++;
            }
        }
        DataPtr++;
    }

    delete Data;

    fclose(out);
    return 0;
}
