#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <jwt.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Error Handling.
#define K_SUCCESS 0
#define K_FAILURE 1

// Hardcoded credentials
#define USERNAME "anurag"
#define PASSWORD "iot789"
#define SECRET_KEY "mysecretkey123"

bool Authenticate_Client(int fd_server, int cl_socket, jwt_t *Jauth);

int main() {
    bool auth_state = 0U;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    jwt_t *jwt = NULL; //jwt object.

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
    auth_state = Authenticate_Client(server_fd, new_socket, jwt);

    // Echo loop
    while (auth_state == K_SUCCESS) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        // Expect "JWT:message"
        char *sep = strchr(buffer, ':');
        if (!sep) continue;
        *sep = '\0';
        char *recv_jwt = buffer;
        char *message = sep + 1;

        if (jwt_decode(&jwt, recv_jwt, (unsigned char*)SECRET_KEY, strlen(SECRET_KEY)) != 0) {
            printf("Invalid JWT. Closing.\n");
            break;
        }

        // Check expiry
        time_t exp = jwt_get_grant_int(jwt, "exp");
        if (time(NULL) > exp) {
            printf("JWT expired. Closing.\n");
            jwt_free(jwt);
            break;
        }

        if (strcmp(message, "exit") == 0) {
            printf("Exit received. Closing.\n");
            jwt_free(jwt);
            break;
        }

        printf("Received: %s\n", message);
        send(new_socket, message, strlen(message), 0);
        jwt_free(jwt);
    }

    close(new_socket);
    close(server_fd);
    return K_SUCCESS;
}

bool Authenticate_Client(int fd_server, int cl_socket, jwt_t *Jauth)
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

    if (strcmp(buffer, USERNAME ":" PASSWORD) == 0) {
        jwt_new(&Jauth);

        // Add claims
        jwt_add_grant(Jauth, "sub", USERNAME);
        jwt_add_grant(Jauth, "role", "user"); //Role set as user.

        // Expiry claim (60 seconds)
        time_t exp = time(NULL) + 60;
        jwt_add_grant_int(Jauth, "exp", exp);

        // Sign JWT
        jwt_set_alg(Jauth, JWT_ALG_HS256, (unsigned char*)SECRET_KEY, strlen(SECRET_KEY));
        char *out = jwt_encode_str(Jauth);

        send(cl_socket, out, strlen(out), 0);
        printf("Issued JWT: %s\n", out);

        jwt_free(Jauth);
        free(out);
    }
    else
    {
        char *fail = "AUTH_FAIL";
        send(cl_socket, fail, strlen(fail), 0);
        close(cl_socket);
        close(fd_server);
        return K_FAILURE;
    }

    return K_SUCCESS;
}