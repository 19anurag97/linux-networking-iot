#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Error Handling.
#define K_SUCCESS 0
#define K_FAILURE 1

// Hardcoded credentials
#define USERNAME "anurag"
#define PASSWORD "iot789"

bool Authenticate_Client(int fd_server, int cl_socket);

int main() {
    bool auth_state = 0U;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Echo Server listening on port %d...\n", PORT);

    // Accept a client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // --- Authentication step ---
    auth_state = Authenticate_Client(server_fd, new_socket);

    // Echo loop
    while (auth_state == K_SUCCESS) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("Received: %s\n", buffer);

        // Echo back to client
        send(new_socket, buffer, bytes_read, 0);
    }

    close(new_socket);
    close(server_fd);
    return K_SUCCESS;
}

bool Authenticate_Client(int fd_server, int cl_socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_read = read(cl_socket, buffer, BUFFER_SIZE);
    if (bytes_read <= 0) {
        printf("Client disconnected before authentication.\n");
        close(cl_socket);
        close(fd_server);
        return K_FAILURE;
    }

    // Expect "username:password"
    buffer[bytes_read] = '\0';
    printf("Auth attempt: %s\n", buffer);

    if (strcmp(buffer, USERNAME ":" PASSWORD) == 0) {
        char *ok = "AUTH_OK";
        send(cl_socket, ok, strlen(ok), 0);
        printf("Client authenticated successfully.\n");
    } else {
        char *fail = "AUTH_FAIL";
        send(cl_socket, fail, strlen(fail), 0);
        printf("Authentication failed. Closing connection.\n");
        close(cl_socket);
        close(fd_server);
        return K_FAILURE;
    }
    return K_SUCCESS;
}