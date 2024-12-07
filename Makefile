CC = gcc
CFLAGS = -Wall -std=c99 -pedantic


all : 
	$(CC) $(CFLAGS) main.c -o main.out -g

test : 
	./main.out county_demographics.csv task2_1.txt
	./main.out county_demographics.csv task2_2.txt
	./main.out county_demographics.csv task2_3.txt
	./main.out county_demographics.csv task2_4.txt

test2 :
	./main.out small.csv task2_3.txt
	./main.out small.csv task2_4.txt
	./main.out small.csv ops.txt

valgrind :
	valgrind --leak-check=yes ./main.out small.csv ops.txt