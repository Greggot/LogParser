CC = g++
PARSE_DAF = ParserDAF
PARSE_TRW = ParserTRW
PARSE = Parse
OBJ = *.o
CPP = *.cpp
CLEAR = rm -f

$(PARSE) : mainParser.o
	$(CC) -o $@ $^

$(PARSE_DAF) : $(OBJ)
	$(CC) -o $@ mainDAF.cpp ParserDAF.cpp Parse.cpp 

$(PARSE_TRW) : $(OBJ)
	$(CC) -o $@ mainTRW.cpp ParserVolvo.cpp Parse.cpp 

$(OBJ) : $(CPP) 
	$(CC) -c $@ $^

objects : 
	$(CC) mainTRW.cpp -c
	$(CC) mainDAF.cpp -c
	$(CC) mainParser.cpp -c
	$(CC) ParserVolvo.cpp -c
	$(CC) ParserDAF.cpp -c
	$(CC) Parse.cpp -c

clear :
	$(CLEAR) $(OBJ)