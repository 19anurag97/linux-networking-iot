#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Server info
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d...\n", PORT);

    // Echo loop
    while (1) {
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL,
                         (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Client: %s\n", buffer);

        // Exit condition
        if (strcmp(buffer, "exit") == 0) {
            printf("Exit command received. Closing server.\n");
            break;
        }

        // Send ACK back
        char ack[BUFFER_SIZE+10];
        snprintf(ack, sizeof(ack), "ACK:%s", buffer);
        sendto(sockfd, ack, strlen(ack), MSG_CONFIRM,
               (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}