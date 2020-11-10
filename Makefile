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
	$(CC) -c main.o main.cpp
	$(CC) -c Parse.o Parse.cpp

clear:
	$(CLEAR) main.o
	$(CLEAR) Parse.o
