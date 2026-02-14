#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    int seq=0;
    int expected_seq = 0;
    int n=0;
    char buffer[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
    char ack[BUFFER_SIZE+10];
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
        n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL,
                         (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        // Exit condition
        if (strcmp(buffer, "exit") == 0)
        {
            printf("Exit command received. Closing server.\n");
            break;
        }

        //Sequencing
        sscanf(buffer, "%d:%s", &seq, msg);
        printf("Received [seq=%d]: %s\n", seq, msg);

        if (seq == expected_seq)
        {
            // Process message
            if (strcmp(msg, "exit") == 0)
            {
                printf("Exit command received. Closing server.\n");
                break;
            }
            expected_seq = 1 - expected_seq; // toggle between 0 and 1
        }
        else 
        {
            printf("Duplicate packet ignored.\n");
        }

        // Send ACK back
        snprintf(ack, sizeof(ack), "ACK:%d", seq);
        sendto(sockfd, ack, strlen(ack), MSG_CONFIRM,
               (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}