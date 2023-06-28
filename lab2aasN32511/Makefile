CFLAGS=-O3 -Wall -Wextra -Werror
TARGET=server client
VLCHECK=valgrind --leak-check=full --show-leak-kinds=all
OPTS=-a 0.0.0.0 -p 55555

./PHONY: all

all: $(TARGET)

clean: 
	@rm -rf *.o $(TARGET)

server: lab2aasN32511_server.c
	gcc $(CFLAGS) -o server lab2aasN32511_server.c -lm -pthread
client: lab2aasN32511_client.c
	gcc $(CFLAGS) -o client lab2aasN32511_client.c
valgrind_s:
	$(VLCHECK) ./server -l ./tmp/lab2log.log -w 2 $(OPTS) > valgrind_s.txt 2>&1
valgrind_c:
	$(VLCHECK) ./client $(OPTS) > valgrind_c.txt 2>&1