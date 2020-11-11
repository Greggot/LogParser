CC = g++
OBJ = *.o
CPP = *.cpp 
CLEAR = rm -f
PROGRAM = ParserVolvo
MAIN_PROGRAM = Parser

$(MAIN_PROGRAM) :  gui.o
	$(CC) -o $@ $^
	
$(PROGRAM) : $(OBJ)
	$(CC) -o $@ $^

$(OBJ) : $(CPP) 
	$(CC) -c $@ $^
	
objects : main.cpp Parse.cpp
	$(CC) main.cpp -c
	$(CC) Parse.cpp -c
	$(CC) gui.cpp -c

clear:
	$(CLEAR) main.o
	$(CLEAR) Parse.o
