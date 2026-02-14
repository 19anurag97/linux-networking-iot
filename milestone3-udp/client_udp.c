#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 2
#define MAX_RETRIES 3

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server info
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof(servaddr);

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (1)
    {
        printf("> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            perror("fgets failed");
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0';

       int retries = 0;
        int ack_received = 0;

        while (retries < MAX_RETRIES && !ack_received)
        {
            sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM,
                   (const struct sockaddr *)&servaddr, len);

            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL,
                             (struct sockaddr *)&servaddr, &len);

            if (n < 0) {
                // Timeout occurred
                retries++;
                printf("Timeout, retrying (%d/%d)...\n", retries, MAX_RETRIES);
            } else {
                buffer[n] = '\0';
                printf("Server replied: %s\n", buffer);
                ack_received = 1;
            }
        }

        if (!ack_received) {
            printf("Failed to receive ACK after %d retries. Giving up.\n", MAX_RETRIES);
        }

        if (strcmp(buffer, "exit") == 0) {
            printf("Exit command sent. Closing client.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}