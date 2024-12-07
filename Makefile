CC = gcc
CFLAGS = -Wall -std=c99 -pedantic


all : 
	$(CC) $(CFLAGS) main.c -o main.out

test : 
	./main.out small.csv ops.text