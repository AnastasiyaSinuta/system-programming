CFLAGS=-O3 -Wall -Wextra -Werror
TARGET=lab2aasN32511_server lab2aasN32511_client

./PHONY: all

all: $(TARGET)

clean: 
	@rm -rf *.o $(TARGET)

lab2aasN32511_server: lab2aasN32511_server.c
	gcc $(CFLAGS) -o lab2aasN32511_server lab2aasN32511_server.c
lab2aasN32511_client: lab2aasN32511_client.c
	gcc $(CFLAGS) -o lab2aasN32511_client lab2aasN32511_client.c