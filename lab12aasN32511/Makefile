CC=gcc
CFLAGS=-O3 -Wall -Wextra -Werror

./PHONY: all

all: lab12aasN32511

clean: 
	@rm -rf *.o lab12aasN32511

lab12aasN32511: lab12aasN32511.o LAB12DEBUG.o
	$(CC) $(CFLAGS) -o $@ $^
lab12aasN32511.o: lab12aasN32511.c
	$(CC) $(CFLAGS) -c lab12aasN32511.c
LAB12DEBUG.O: LAB12DEBUG.c
	$(CC) $(CFLAGS) -c LAB12DEBUG.c

valgrind:
	gcc -g -o lab12aasN32511 lab12aasN32511.c LAB12DEBUG.c
	valgrind --leak-check=full ./lab12aasN32511> valgrind.txt 2>&1