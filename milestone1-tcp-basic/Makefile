# Makefile for Echo Server and Client

CC = gcc
CFLAGS = -Wall -Wextra -O2

SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c -o $(SERVER)

$(CLIENT): tcp_client.c
	$(CC) $(CFLAGS) tcp_client.c -o $(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)