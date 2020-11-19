CC = g++
PROGRAM = LogParser
OBJ = *.o

$(PARSE_DAF) : $(OBJ)
	$(CC) -o $@ main.o LogParser.o

obj :
	$(CC) -c main.cpp
	$(CC) -c LogParser.cpp