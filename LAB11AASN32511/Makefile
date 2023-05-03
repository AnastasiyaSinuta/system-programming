# gcc -g -o lab11aasN32511 lab11aasN32511.c walk_dir.c options.c
# valgrind --leak-check=full ./lab11aasN32511 -v > valgrind.txt 2>&1

CC=gcc
CFLAGS=-O3 -Wall -Wextra -Werror

./PHONY: all

all: lab11aasN32511

clean: 
	@rm -rf *.o lab11aasN32511

lab11aasN32511: lab11aasN32511.o LAB11DEBUG.o options.o walk_dir.o error_message.o
	$(CC) $(CFLAGS) -o $@ $^
lab11aasN32511.o: lab11aasN32511.c
	$(CC) $(CFLAGS) -c lab11aasN32511.c
LAB11DEBUG.O: LAB11DEBUG.c
	$(CC) $(CFLAGS) -c LAB11DEBUG.c
options.o: options.c
	$(CC) $(CFLAGS) -c options.c
walk_dir.o: walk_dir.c
	$(CC) $(CFLAGS) -c walk_dir.c
error_message.o: error_message.c
	$(CC) $(CFLAGS) -c error_message.c

valgrind:
	gcc -g -o lab11aasN32511 lab11aasN32511.c LAB11DEBUG.c options.c walk_dir.c error_message.c
	valgrind --leak-check=full ./lab11aasN32511 -v > valgrind.txt 2>&1