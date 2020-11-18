CC = g++
PROGRAM = FromLogToTxt
OBJ = *.o

$(PARSE_DAF) : $(OBJ)
	$(CC) -o $@ $^

obj :
	$(CC) -c main.cpp
	$(CC) -c WaitForPythonWin.cpp