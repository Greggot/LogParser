CC = g++
PROGRAM = LogParser
PARSE_DAF = ParserDelphiReadMemoryByAddress
PARSE_VOLVO = ParserTRWTransferData
PARSE = Parse
OBJ = *.o
CPP = *.cpp
CLEAR = rm -f

LogParser : mainLOG.o LogParser.o
	$(CC) -o LogParser mainLOG.o LogParser.o
IDLogParser:
	$(CC) -o IDLogParser IDlogmain.o LogParser.o

$(PARSE) : mainParser.o
	$(CC) -o $@ $^

$(PARSE_DAF) : $(OBJ)
	$(CC) -o $@ mainDAF.o ParserDAF.o Parse.o 

$(PARSE_VOLVO) : $(OBJ)
	$(CC) -o $@ mainVolvo.o ParserVolvo.o Parse.o 

objects : $(CPP)
	$(CC) mainVolvo.cpp -c
	$(CC) mainDAF.cpp -c
	$(CC) mainParser.cpp -c
	$(CC) mainLOG.cpp -c
	$(CC) ParserVolvo.cpp -c
	$(CC) ParserDAF.cpp -c
	$(CC) LogParser.cpp -c
	$(CC) IDlogmain.cpp -c
	$(CC) Parse.cpp -c

clear :
	$(CLEAR) $(OBJ)