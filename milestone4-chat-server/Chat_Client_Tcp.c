#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_USERNAME_LEN 32

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to chat server.\n");
    printf("Enter your username (max 32 chars): ");
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
    {
        perror("fgets failed");
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    //Validate Username length in client side.
    if (strlen(buffer) == 0)
    {
        printf("Error: Username cannot be empty.\n");
        return 1;
    }
    
    if (strlen(buffer) > MAX_USERNAME_LEN)
    {
        buffer[MAX_USERNAME_LEN] = '\0'; // truncate safely
        printf("Username too long, truncated to: %s\n", buffer);
    }
    
    send(sockfd, buffer, strlen(buffer), 0);

    fd_set readfds;
    int maxfd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;

    while (1) 
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);       // watch server socket
        FD_SET(STDIN_FILENO, &readfds); // watch keyboard input

        // Wait for activity on either stdin or socket
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0)
        {
            perror("select error");
            break;
        }

        // Check if user typed something
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                perror("fgets failed");
                break;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            send(sockfd, buffer, strlen(buffer), 0);

            if (strcmp(buffer, "exit") == 0) {
                printf("Closing client.\n");
                break;
            }
        }

        // Check if server sent something
        if (FD_ISSET(sockfd, &readfds)) {
            int n = read(sockfd, buffer, BUFFER_SIZE);
            if (n <= 0) {
                printf("Server disconnected.\n");
                break;
            }
            buffer[n] = '\0';
            printf("\nBroadcast: %s\n> ", buffer);
            fflush(stdout);
        }
    }

    close(sockfd);
    return 0;
}