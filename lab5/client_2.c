#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX 1024

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int areAnagrams(char *str1, char *str2)
{
    int count[256] = {0};
    int i;

    for (i = 0; str1[i] && str2[i]; i++)
    {
        count[str1[i]]++;
        count[str2[i]]--;
    }

    if (str1[i] || str2[i])
        return 0;

    for (i = 0; i < 256; i++)
        if (count[i])
            return 0;

    return 1;
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX];
    char str1[MAX], str2[MAX];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("Socket creation failed");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        error("Connection to the server failed");
    }

    printf("Enter first string: ");
    fgets(str1, MAX, stdin);
    str1[strcspn(str1, "\n")] = 0; // Remove newline character

    printf("Enter second string: ");
    fgets(str2, MAX, stdin);
    str2[strcspn(str2, "\n")] = 0; // Remove newline character

    // combined string with space in between
    strcat(str1, " ");
    strcat(str1, str2);

    // Send strings to server
    write(sockfd, str1, sizeof(str1));

    // Receive result from server
    read(sockfd, buffer, sizeof(buffer));
    printf("Server: %s\n", buffer);

    close(sockfd);
    return 0;
}