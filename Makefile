CC = g++
OBJ = *.o
CPP = *.cpp 
PROGRAM = ParserVolvo

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^

$(OBJ) : $(CPP) 
	$(CC) -c $@ $^
clear:
	rm -f main.o
	rm -f Parse.o