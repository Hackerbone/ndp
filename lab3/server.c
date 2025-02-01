#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Server waiting for connections...\n");
    printf("Server PID: %d, PPID: %d\n", getpid(), getppid());

    // Accept client connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Fork to handle communication with client
    pid = fork();
    if (pid == 0) {  // Child process
        // Communication loop with client (client -> server)
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                break;
            }
            printf("Client: %s\n", buffer);

            // Server -> Client
            printf("Server: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(client_fd, buffer, strlen(buffer), 0);
        }
        close(client_fd);  // Close socket in the child
        exit(0);
    } else if (pid > 0) {  // Parent process
        wait(NULL);  // Wait for child to finish
    } else {
        perror("Fork failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    close(server_fd);  // Close server socket in parent
    return 0;
}
