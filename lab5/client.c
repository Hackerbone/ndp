#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888

void communicate_with_server(const char *message)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }

    send(sock, message, strlen(message), 0);
    printf("Message sent: %s\n", message);
    read(sock, buffer, 1024);
    printf("Server response: %s\n", buffer);

    close(sock);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <client_number>\n", argv[0]);
        return 1;
    }

    int client_number = atoi(argv[1]);

    if (client_number == 1)
    {
        communicate_with_server("Institute Of");
    }
    else if (client_number == 2)
    {
        communicate_with_server("Technology");
    }
    else
    {
        fprintf(stderr, "Invalid client number. Use 1 or 2.\n");
        return 1;
    }

    return 0;
}