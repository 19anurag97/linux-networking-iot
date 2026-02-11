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

bool Authenticate_Server(int t_sock);

int main() {
    bool auth_state = 0;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return K_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        return K_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return K_FAILURE;
    }

    // --- Authentication step ---
    auth_state = Authenticate_Server(sock);

    while (auth_state == K_SUCCESS) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("> ");

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            // fgets failed or EOF reached
            perror("fgets failed");
            break;
        }

        // Send message
        send(sock, buffer, strlen(buffer), 0);

        // Receive echo
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("Echoed: %s\n", buffer);
    }

    close(sock);
    return K_SUCCESS;
}

bool Authenticate_Server(int t_sock)
{
    char buffer[BUFFER_SIZE];
    char credentials[BUFFER_SIZE];
    printf("Enter username: ");
    if (fgets(credentials, BUFFER_SIZE, stdin) == NULL)
    {
        //error;
        return K_FAILURE;
    }
    credentials[strcspn(credentials, "\n")] = '\0'; // remove newline
    strcat(credentials, ":");
    char password[BUFFER_SIZE];
    printf("Enter password: ");
    if (fgets(password, BUFFER_SIZE, stdin) == NULL)
    {
        //error;
        return K_FAILURE;
    }
    password[strcspn(password, "\n")] = '\0';
    strcat(credentials, password);

    send(t_sock, credentials, strlen(credentials), 0);

    memset(buffer, 0, BUFFER_SIZE);
    int bytes_read = read(t_sock, buffer, BUFFER_SIZE);
    if (bytes_read <= 0 || strcmp(buffer, "AUTH_OK") != 0) {
        printf("Authentication failed. Exiting.\n");
        close(t_sock);
        return K_FAILURE;
    }
    printf("Authenticated successfully!\n");
    return K_SUCCESS;
}