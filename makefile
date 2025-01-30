# Makefile

SRC = ./src/*.cpp
OUT = ./main

# Default target
all:
		g++ $(SRC) -o $(OUT) -lncurses

# Clean target
clean:
		rm -f $(OUT)