#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 2
#define MAX_RETRIES 3

int main() {
    int sockfd;
    int seq = 0;
    char buffer[BUFFER_SIZE];
    char packet[BUFFER_SIZE+10];
    int retries = 0;
    int ack_received = 0;
    int n=0;
    int ack_seq=0;
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
        snprintf(packet, sizeof(packet), "%d:%s", seq, buffer);

        retries = 0;
        ack_received = 0;
        
        while (retries < MAX_RETRIES && !ack_received)
        {
            sendto(sockfd, packet, strlen(packet), MSG_CONFIRM,
                   (const struct sockaddr *)&servaddr, len);

            n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL,
                             (struct sockaddr *)&servaddr, &len);

            if (n < 0)
            {
                // Timeout occurred
                retries++;
                printf("Timeout, retrying (%d/%d)...\n", retries, MAX_RETRIES);
            }
            else
            {
                buffer[n] = '\0';
                sscanf(buffer, "ACK:%d", &ack_seq);
                if (ack_seq == seq)
                {
                    printf("Received ACK for seq=%d\n", ack_seq);
                    ack_received = 1;
                    seq = 1 - seq; // toggle sequence number
                }
                else
                {
                    printf("Wrong ACK received, ignoring.\n");
                }
            }
        }

        if (!ack_received) 
        {            
            printf("Failed to receive ACK after %d retries. Giving up.\n", MAX_RETRIES);
        }

        if (strcmp(packet, "0:exit") == 0 || strcmp(packet, "1:exit") == 0) 
        {
            printf("Exit command sent. Closing client.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}