#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_USERNAME_LEN 32

typedef struct {
    int sockfd;
    char username[50];
} Client;

int main() {
    int server_fd, new_socket, max_sd, sd, activity;
    struct sockaddr_in address;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    Client clients[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].sockfd = 0;
        strcpy(clients[i].username, "");
    }

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow reuse of address
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Chat server listening on port %d...\n", PORT);

    socklen_t addrlen = sizeof(address);

    while (1) {
        // Clear and set file descriptors
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = clients[i].sockfd;
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Wait for activity
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select error");
        }

        // Incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection: socket fd %d, ip %s, port %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add to client list
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].sockfd == 0) {
                    clients[i].sockfd = new_socket;
                    break;
                }
            }
        }

        // IO on existing sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].sockfd;
            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&address, &addrlen);
                    printf("Client disconnected: ip %s, port %d\n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(sd);
                    clients[i].sockfd = 0;
                    strcpy(clients[i].username, "");
                }
                else 
                {
                    buffer[valread] = '\0';

                    if (strlen(clients[i].username) == 0)
                    {
                        // First message is the username
                        size_t len = strcspn(buffer, "\n"); // strip newline
                        buffer[len] = '\0';
                        //Validate Username length in server side
                        if (strlen(buffer) == 0) {
                            send(sd, "Error: Username cannot be empty.\n", 33, 0);
                            close(sd);
                            clients[i].sockfd = 0;
                            continue;
                        }
                    
                        if (strlen(buffer) > MAX_USERNAME_LEN) {
                            send(sd, "Error: Username too long (max 32 chars).\n", 40, 0);
                            close(sd);
                            clients[i].sockfd = 0;
                            continue;
                        }
                    
                        snprintf(clients[i].username, sizeof(clients[i].username), "%s", buffer);
                        printf("Client registered as: %s\n", clients[i].username);
                    }
                    else
                    {
                        printf("[%s]: %s\n", clients[i].username, buffer);

                        char msg[BUFFER_SIZE + 50];
                        snprintf(msg, sizeof(msg), "[%s]: %s", clients[i].username, buffer);

                        for (int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if (clients[j].sockfd != 0 && clients[j].sockfd != sd)
                            {
                                send(clients[j].sockfd, msg, strlen(msg), 0);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}