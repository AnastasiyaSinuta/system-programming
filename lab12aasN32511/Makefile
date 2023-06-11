\CFLAGS=-Wall -Wextra -Werror -O3
TARGETS=lab1aasN32511 libaasN32511.so

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1aasN32511: lab1aasN32511.c plugin_api.h
	gcc $(CFLAGS) -g -o lab1aasN32511 lab1aasN32511.c -ldl

libaasN32511.so: libaasN32511.c plugin_api.h
	gcc $(CFLAGS) -g -shared -fPIC -o libaasN32511.so libaasN32511.c -ldl -lm

vers:
	./lab1aasN32511 -v

help:
	./lab1aasN32511 -h

do:
	./lab1aasN32511 --bit-seq 0b110110100001 /home/vboxuser/Desktop/system-programming/lab12aasN32511/tests

do_debug:
	LAB1DEBUG=1 ./lab1aasN32511 --bit-seq 0b110110100001 /home/vboxuser/Desktop/system-programming/lab12aasN32511/tests

plug:
	LAB1DEBUG=1 ./lab1aasN32511 -P /home/vboxuser/Desktop/system-programming/plugs -A --bit-seq 0b110110100001 --entropy 0.570665 /home/vboxuser/Desktop/system-programming/lab12aasN32511/tests
