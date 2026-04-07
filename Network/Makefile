CC = gcc
CFLAGS = -Wall -Wextra -g

all: server client

server: example_server.c network.c network.h
	$(CC) $(CFLAGS) -o server example_server.c network.c

client: example_client.c network.c network.h
	$(CC) $(CFLAGS) -o client example_client.c network.c

clean:
	rm -f server client