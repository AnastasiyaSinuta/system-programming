CFLAGS=-Wall -Wextra -Werror -O2
TARGETS=lab1aasN32511

.PHONY: all clean

all: lab1aasN32511

clean:
	rm -rf *.o $(TARGETS)

lab1aasN32511: lab1aasN32511.o LAB1DEBUG.o walk_dir.o
	gcc $(CFLAGS) -o $@ $^

lab1aasN32511.o: lab1aasN32511.c
	gcc $(CFLAGS) -c lab1aasN32511.c

LAB1DEBUG.o: LAB1DEBUG.c
	gcc $(CFLAGS) -c LAB1DEBUG.c

walk_dir.o: walk_dir.c
	gcc $(CFLAGS) -c walk_dir.c

valgrind:
	gcc -g -o lab1aasN32511 lab1aasN32511.c LAB1DEBUG.c
	valgrind --leak-check=full ./lab1aasN32511> valgrind.txt 2>&1

search:
	./lab1aasN32511 /home/vboxuser/Desktop/tests/test.txt "0b110110100001"

search_debug:
	LAB1DEBUG=1 ./lab1aasN32511 /home/vboxuser/Desktop/tests "0b110110100001"
