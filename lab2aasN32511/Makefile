CFLAGS=-O3 -Wall -Wextra -Werror
TARGET=server client

./PHONY: all

all: $(TARGET)

clean: 
	@rm -rf *.o $(TARGET)

server: lab2aasN32511_server.c
	gcc $(CFLAGS) -o server lab2aasN32511_server.c -lm -pthread
client: lab2aasN32511_client.c
	gcc $(CFLAGS) -o client lab2aasN32511_client.c
