CFLAGS=-Wall -Wextra -Werror -O2
TARGETS=lab1aasN32511

.PHONY: all clean

all: lab1aasN32511

clean:
	rm -rf *.o $(TARGETS)

lab1aasN32511: lab1aasN32511.o LAB1DEBUG.o walk_dir.o change_target.o options.o
	gcc $(CFLAGS) -o $@ $^ -lm

lab1aasN32511.o: lab1aasN32511.c
	gcc $(CFLAGS) -c lab1aasN32511.c

LAB1DEBUG.o: LAB1DEBUG.c
	gcc $(CFLAGS) -c LAB1DEBUG.c

options.o: options.c
	gcc $(CFLAGS) -c options.c

change_target.o: change_target.c
	gcc $(CFLAGS) -c change_target.c

walk_dir.o: walk_dir.c
	gcc $(CFLAGS) -c walk_dir.c

do:
	./lab1aasN32511 /home/anastasiya/Desktop/tests "0b110110100001"

do_debug:
	LAB1DEBUG=1 ./lab1aasN32511 /home/anastasiya/Desktop/tests "0b110110100001"

v:
	LAB1DEBUG=1 ./lab1aasN32511 -v

h:
	LAB1DEBUG=1 ./lab1aasN32511 -h

P:
	LAB1DEBUG=1 ./lab1aasN32511 -P

A:
	LAB1DEBUG=1 ./lab1aasN32511 -A

O:
	LAB1DEBUG=1 ./lab1aasN32511 -O

valgrind:
	gcc -g -o lab1aasN32511 lab1aasN32511.c LAB1DEBUG.c
	valgrind --leak-check=full ./lab1aasN32511> valgrind.txt 2>&1
