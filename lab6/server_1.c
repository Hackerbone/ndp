#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to calculate parity bit (0 for even parity, 1 for odd parity)
int calculate_parity(const char *data, int length)
{
    int parity = 0;
    for (int i = 0; i < length; i++)
    {
        parity ^= data[i];
    }
    return parity & 1;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char data[] = "1010101"; // Example binary data

    // Calculate parity bit
    int parity = calculate_parity(data, strlen(data));
    printf("Calculated parity: %d\n", parity);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Send data and parity bit to receiver
    snprintf(buffer, BUFFER_SIZE, "%s%d", data, parity);
    send(new_socket, buffer, strlen(buffer), 0);
    printf("Data sent: %s\n", buffer);

    close(new_socket);
    close(server_fd);
    return 0;
}