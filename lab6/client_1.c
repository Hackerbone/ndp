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
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Receive data and parity bit from server
    read(sock, buffer, BUFFER_SIZE);
    printf("Data received: %s\n", buffer);

    // Extract data and parity bit
    int length = strlen(buffer) - 1;
    char data[length + 1];
    strncpy(data, buffer, length);
    data[length] = '\0';
    int received_parity = buffer[length] - '0';

    // Calculate parity bit
    int calculated_parity = calculate_parity(data, length);
    printf("Calculated parity: %d\n", calculated_parity);

    // Check if data is corrupted
    if (calculated_parity == received_parity)
    {
        printf("Data is not corrupted.\n");
    }
    else
    {
        printf("Data is corrupted.\n");
    }

    close(sock);
    return 0;
}