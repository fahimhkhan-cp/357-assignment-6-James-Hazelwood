CC = gcc
CFLAGS = -Wall -std=c99 -pedantic


all : 
	$(CC) $(CFLAGS) main.c -o main.out -g

test : 
	./main.out small.csv ops.txt

valgrind :
	valgrind --leak-check=yes ./main.out small.csv ops.txt