#include "Parse.h"

//**************************************************************************************************
// Global variable
//**************************************************************************************************

byteString emptybyteString {{0,0,0,0,0,0,0,0}}; //ѕуста€ байт-строка дл€ инициализации объектов

        CommandIndex _0x34 {0x34,               // омандные индексы и значение SID дл€ команд
                            0, 2, 3,            //0х34, 0х36, 0х37
                            0, 1, 2, 3};
        CommandIndex _0x36 {0x36,
                            0, 2, 3,
                            0, 1, 2, 3};
        CommandIndex _0x37 {0x37,
                            0, 1, 2,
                            0, 1, 2, 3};


//**************************************************************************************************
// Procedure ADC_IRQHandler()
//**************************************************************************************************

//* ¬ывод байт-строки в консоль *//
void Parse::byteStringOut(byteString Out)
{
    for(uint8_t i = 0; i<STRING_LENGTH_BYTE; i++)
        std::cout<<std::hex<<(int)Out.bytes[i]<<'\t';
    std::cout<<std::endl;
}

//**************************************************************************************************
// Procedure ADC_IRQHandler()
//**************************************************************************************************

//* —читывание длины сообщени€ из двух байт *//
uint16_t Parse::countLength(uint8_t one, uint8_t two)
{
    return (uint32_t)((one<<8 | two) & 0xFFF);  //—двиг первого байта на байт влево и применение маски, чтобы
}                                               //убрать первые пол-байта

//**************************************************************************************************
// Procedure ADC_IRQHandler()
//**************************************************************************************************

//* ѕеревод строки в подстроку *//
byteString Parse::StringToByte(std::string Input)
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

bool Parse::CheckFor_0x34(char* Input, byteString commandString, CommandIndex cmnd)      //явл€етс€ ли строка началом серии пакетов
{

    uint16_t msgLength = countLength(commandString.bytes[cmnd.SIZE], commandString.bytes[cmnd.SIZE+1]);       //«адать в CommandIndex длину адреса, чтобы унифицировать дл€ всех команд
    if(msgLength>6)
        msgLength = ceil ((msgLength-6)/7) + 3;     //3, т.к. не считаем эту строку, с 30 0 0 0 0 0 0 0
    else
        msgLength = 1;

    Input+= msgLength*STRING_LENGTH_ASCII;
    byteString responceString = StringToByte(Input);

    if(responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&
            responceString.bytes[cmnd.SIZE]<7)
            std::cout<<"SESSION\n";

    return (responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&
            responceString.bytes[cmnd.SIZE]<7);
}

bool Parse::CheckFor_0x36(byteString* commandString, CommandIndex cmnd)      //явл€етс€ ли строка началом серии пакетов
{
    uint16_t Length = countLength(commandString->bytes[cmnd.SIZE], commandString->bytes[cmnd.SIZE+1]);
    uint16_t msgLength = 0;       //«адать в CommandIndex длину адреса, чтобы унифицировать дл€ всех команд
    if(Length>6)
        msgLength = ceil ((Length-6)/7) + 3;     //3, т.к. не считаем эту строку, с 30 0 0 0 0 0 0 0
    else
        msgLength = 1;

    if((Length - 6) % 7 == 0)
        msgLength -= 1;

                      //–ади экономии пам€ти сдвигаем командную строку на количество строк до строки с ответом
    byteString* responceString = commandString + msgLength + 1;       //—оздаЄм вторую байт-строку, что на одну ниже строки с ответом т.к. обычно там находитс€
                                                        //положительный ответ на запрос
    byteString* subResponceString = responceString - 1;
    return (responceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE + 0x40 &&
            subResponceString->bytes[cmnd.SID_RESPONCE] == 0x7F);   //запросом подождать
}

bool Parse::CheckFor_0x76 (byteString* responceString, CommandIndex cmnd)
{
    byteString* subResponceString = responceString + 1;
    return responceString->bytes[cmnd.SID_RESPONCE] == 0x7F &&
           subResponceString->bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40;
}

bool Parse::CheckFor_0x37 (char* Input, byteString commandString, CommandIndex cmnd)     //явл€етс€ ли концом серии пакетов
{
    byteString responceString = StringToByte(Input+STRING_LENGTH_ASCII);

    return responceString.bytes[cmnd.SID_RESPONCE] == cmnd.SID_VALUE+0x40 &&
    responceString.bytes[cmnd.SIZE]<3;
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
                IsDataTransferActive = CheckFor_0x36(DataPtr, _0x36);
            if(IsDataTransferActive)
            {
                StringCount = 0;
                PackageCount = 0;
                PackageLength = countLength(DataPtr->bytes[_0x36.SIZE], DataPtr->bytes[_0x36.SIZE+1]);
                std::cout<<"SIZE: "<<PackageLength<<std::endl;
            }
        }
        else
        {
            if(DataPtr->bytes[0]<STRING_LENGTH_BYTE)
                IsDataTransferActive = !CheckFor_0x76(DataPtr, _0x37);
        }


        if(IsDataTransferActive && PackageCount < PackageLength)
        {
            uint8_t index = 0;

            switch (StringCount)
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

            for(; index < STRING_LENGTH_BYTE; index++)
            {
                if(PackageCount + 2 == PackageLength)         //Ќумераци€ текущих пакетов начинаетс€ с нул€, общего количества - с единицы
                {                                           //ѕотому если следуща€ строка €вл€етс€ концом пакета, то закончить передачу
                    IsDataTransferActive = false;
                    break;
                }
                fwrite(&DataPtr->bytes[index], 1, 1, out);  //ѕередача - помещение полезных байтов в бинарный файл
                PackageCount++;
            }

            StringCount++;
        }

        DataPtr++;
    }

    delete Data;

    fclose(out);
    return 0;
}
