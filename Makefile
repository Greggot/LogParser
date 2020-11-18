CC = g++
PARSE_DAF = ParserDelphiReadMemoryByAddress
PARSE_VOLVO = ParserTRWTransferData
PARSE = Parse
OBJ = *.o
CPP = *.cpp
CLEAR = rm -f

$(PARSE) : mainParser.o
	$(CC) -o $@ $^

$(PARSE_DAF) : $(OBJ)
	$(CC) -o $@ mainDAF.cpp ParserDAF.cpp Parse.cpp 

$(PARSE_VOLVO) : $(OBJ)
	$(CC) -o $@ mainVolvo.cpp ParserVolvo.cpp Parse.cpp 

$(OBJ) : $(CPP) 
	$(CC) -c $@ $^

objects : 
	$(CC) mainVolvo.cpp -c
	$(CC) mainDAF.cpp -c
	$(CC) mainParser.cpp -c
	$(CC) ParserVolvo.cpp -c
	$(CC) ParserDAF.cpp -c
	$(CC) Parse.cpp -c

clear :
	$(CLEAR) $(OBJ)