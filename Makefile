#!/usr/bin/make

CC = gcc
SRC = *.c
PROG = unix-shell.out

# Execute program (the default task when calling `make`)
run: build
	./$(PROG)

# Clean project and compile program
build: clean $(PROG)

# Clean project by removing existing executable file
clean:
	rm -f $(PROG)

# Compile the program from the latest source
$(PROG): $(SRC)
	$(CC) $(SRC) -o $(PROG)
