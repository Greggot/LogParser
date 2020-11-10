CC = g++
OBJ = *.o
CPP = *.cpp 
CLEAR = rm -f
PROGRAM = ParserVolvo

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^

$(OBJ) : $(CPP) 
	$(CC) -c $@ $^
	
objects : main.cpp Parse.cpp
	$(CC) main.cpp -c
	$(CC) Parse.cpp -c 

clear:
	$(CLEAR) main.o
	$(CLEAR) Parse.o
