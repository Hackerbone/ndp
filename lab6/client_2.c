#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

unsigned short crc16(const unsigned char *data, size_t length)
{
    unsigned short crc = 0xFFFF;
    for (size_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
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

    // Receive data and CRC checksum from server
    read(sock, buffer, BUFFER_SIZE);
    printf("Data received: %s\n", buffer);

    // Extract data and CRC checksum
    int length = strlen(buffer) - 4;
    char data[length + 1];
    strncpy(data, buffer, length);
    data[length] = '\0';
    unsigned short received_crc;
    sscanf(buffer + length, "%4hx", &received_crc);

    // Calculate CRC checksum
    unsigned short calculated_crc = crc16((unsigned char *)data, length);
    printf("Calculated CRC: %04X\n", calculated_crc);

    // Check if data is corrupted
    if (calculated_crc == received_crc)
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