#!/usr/bin/make

CC = g++
SRC = main.cpp
PROG = unix-shell.out

# Clean project and compile program
build: clean $(PROG)

# Clean project by removing existing executable file
clean:
	rm -f $(PROG)

# Compile the program from the latest source
$(PROG): $(SRC)
	$(CC) $(SRC) -o $(PROG)
