CFLAGS=-Wall -Wextra -Werror

all: lab11aasN32511

lab11aasN32511: lab11aasN32511.o  
	gcc $(CFLAGS) -o lab11aasN32511 lab11aasN32511.o

lab11aasN32511.o: lab11aasN32511.c
	gcc $(CFLAGS) -c lab11aasN32511.c

clean: rm -rf *.o lab11aasN32511