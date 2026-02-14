# Makefile for Echo Server and Client

CC = gcc
CFLAGS = -Wall -Wextra -O2

SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): server_udp.c
	$(CC) $(CFLAGS) server_udp.c -o $(SERVER)

$(CLIENT): client_udp.c
	$(CC) $(CFLAGS) client_udp.c -o $(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)