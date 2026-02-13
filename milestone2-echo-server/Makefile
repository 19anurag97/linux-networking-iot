# Makefile for Echo Server and Client

CC = gcc
CFLAGS = -Wall -Wextra -O2 
LDFLAGS = -ljwt 	   # link against libjwt and libcrypto

SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): server.c
	$(CC) $(CFLAGS) server.c -o $(SERVER) $(LDFLAGS)

$(CLIENT): client.c
	$(CC) $(CFLAGS) client.c -o $(CLIENT) $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT)