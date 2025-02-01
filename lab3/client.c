#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server...\n");
    printf("Client PID: %d, PPID: %d\n", getpid(), getppid());

    // Fork to handle communication with server
    pid = fork();
    if (pid == 0) {  // Child process
        // Communication loop with server (server -> client)
        while (1) {
            printf("Client: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer), 0);

            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                break;
            }
            printf("Server: %s\n", buffer);
        }
        close(sockfd);
        exit(0);
    } else if (pid > 0) {  // Parent process
        wait(NULL);  // Wait for child to finish
    } else {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
    return 0;
}
