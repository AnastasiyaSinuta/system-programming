\CFLAGS=-Wall -Wextra -Werror -O3
TARGETS=lab1aasN32511 libaasN32511.so

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1aasN32511: lab1aasN32511.c work_with_plugin.c plugin_api.h
	gcc $(CFLAGS) -g -o lab1aasN32511 lab1aasN32511.c work_with_plugin.c -ldl

libaasN32511.so: libaasN32511.c plugin_api.h
	gcc $(CFLAGS) -g -shared -fPIC -o libaasN32511.so libaasN32511.c -ldl -lm

do:
	LAB1DEBUG ./lab1aasN32511 --bit-seq 0b110110100001 /home/anastasiya/Desktop/tests
