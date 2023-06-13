\CFLAGS=-Wall -Wextra -Werror -O3
TARGETS=lab1aasN32511 libaasN32511.so
FOLDERTEST=./tests
FOLDERPLUGIN=/home/vboxuser/Desktop/system-programming

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1aasN32511: lab1aasN32511.c plugin_api.h
	gcc $(CFLAGS) -g -o lab1aasN32511 lab1aasN32511.c -ldl

libaasN32511.so: libaasN32511.c plugin_api.h
	gcc $(CFLAGS) -g -shared -fPIC -o libaasN32511.so libaasN32511.c -ldl -lm

valgrind:
	valgrind --leak-check=full ./lab1aasN32511 --bit-seq 0b110110100001 $(FOLDERTEST) > valgrind.txt 2>&1

vers:
	./lab1aasN32511 -v

help:
	./lab1aasN32511 -h

do:
	./lab1aasN32511 --bit-seq 0b110110100001 $(FOLDERTEST)

do_debug_bin:
	LAB1DEBUG=1 ./lab1aasN32511 --bit-seq 0b110110100001 $(FOLDERTEST)

do_debug_dec:
	LAB1DEBUG=1 ./lab1aasN32511 --bit-seq 3489 $(FOLDERTEST)

do_debug_hex:
	LAB1DEBUG=1 ./lab1aasN32511 --bit-seq 0xDA1 $(FOLDERTEST)

plug:
	LAB1DEBUG=1 ./lab1aasN32511 -P $(FOLDERPLUGIN) --bit-seq 0b110110100001 --entropy 0.5 $(FOLDERTEST)

A:
	LAB1DEBUG=1 ./lab1aasN32511 -P $(FOLDERPLUGIN) -A --bit-seq 0b110110100001 --entropy 0.5 $(FOLDERTEST)

NA:
	LAB1DEBUG=1 ./lab1aasN32511 -P $(FOLDERPLUGIN) -N -A --bit-seq 0b110110100001 --entropy 0.5 $(FOLDERTEST)

O:
	LAB1DEBUG=1 ./lab1aasN32511 -P $(FOLDERPLUGIN) -O --bit-seq 0b110110100001 --entropy 0.5 $(FOLDERTEST)

NO:
	LAB1DEBUG=1 ./lab1aasN32511 -P $(FOLDERPLUGIN) -N -O --bit-seq 0b110110100001 --entropy 0.5 $(FOLDERTEST)
	