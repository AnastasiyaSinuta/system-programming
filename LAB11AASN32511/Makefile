# gcc -g -o lab11aasN32511 lab11aasN32511.c walk_dir.c options.c
# valgrind --leak-check=full ./lab11aasN32511 -v > valgrind.txt 2>&1

CC=gcc
CFLAGS=-Wall -Wextra -Werror

./PHONY: all

all: lab11aasN32511

clean: 
	@rm -rf *.o lab11aasN32511

lab11aasN32511: lab11aasN32511.o walk_dir.o options.o
	$(CC) $(CFLAGS) -o $@ $^
lab11aasN32511.o: lab11aasN32511.c
	$(CC) $(CFLAGS) -c lab11aasN32511.c
walk_dir.o: walk_dir.c
	$(CC) $(CFLAGS) -c walk_dir.c
options.o: options.c
	$(CC) $(CFLAGS) -c options.c