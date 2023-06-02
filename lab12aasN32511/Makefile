CFLAGS=-Wall -Wextra -Werror -O2
TARGETS=lab1aasN32511 libaasN32511

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1aasN32511: lab1aasN32511.o LAB1DEBUG.o change_target.o plugin_api.h
	gcc $(CFLAGS) -g -o lab1aasN32511 lab1aasN32511.c LAB1DEBUG.c change_target.c -ldl -lm

libaasN32511.so: libaasN32511.c plugin_api.h
	gcc $(CFLAGS) -g -shared -fPIC -o libaasN32511.so libaasN32511.c -ldl -lm
