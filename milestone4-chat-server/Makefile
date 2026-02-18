# Makefile for Echo Server and Client

CC = gcc
CFLAGS = -Wall -Wextra -O2

SERVER = Chat_Server_Tcp
CLIENT = Chat_Client_Tcp

all: $(SERVER) $(CLIENT)

$(SERVER): Chat_Server_Tcp.c
	$(CC) $(CFLAGS) Chat_Server_Tcp.c -o $(SERVER)

$(CLIENT): Chat_Client_Tcp.c
	$(CC) $(CFLAGS) Chat_Client_Tcp.c -o $(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)