CFLAGS=-O3 -Wall -Wextra -Werror

./PHONY: all

all: lab2aasN32511_server lab2aasN32511_client

clean: 
	@rm -rf *.o lab2aasN32511_server lab2aasN32511_client

lab2aasN32511_server: lab2aasN32511_server.c
	gcc $(CFLAGS) -o lab2aasN32511_server lab2aasN32511_server.c
lab2aasN32511_client: lab2aasN32511_client.c
	gcc $(CFLAGS) -o lab2aasN32511_client lab2aasN32511_client.c
