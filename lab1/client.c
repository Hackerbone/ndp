#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define MAXSIZE 50

int main()
{
    int sockfd, retval;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket Creation Error\n");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(7878);                   // Use port 7878 to match server code
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost IP address

    // Connect to the server
    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("Connection error\n");
        close(sockfd);
        return 1;
    }

    int exit = 0;
    while (!exit)
    {
        // Prompt user for input
        printf("Enter integers comma separated:\n");
        fgets(buff, sizeof(buff), stdin); // Use fgets to avoid buffer overflow

        // Remove newline character at the end of input
        buff[strcspn(buff, "\n")] = '\0';

        // Send the input to the server
        sentbytes = send(sockfd, buff, strlen(buff) + 1, 0); // Include the null terminator
        if (sentbytes == -1)
        {
            printf("Send error!\n");
            close(sockfd);
            return 1;
        }

        // Receive the server response
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1)
        {
            printf("Receive error!\n");
            close(sockfd);
            return 1;
        }
        buff[recedbytes] = '\0'; // Null terminate the received string
        puts(buff);              // Print server's message

        // Prompt user for choice of operation
        printf("Enter choice of operation:\n");
        printf("1 - Ascending\n2 - Descending\n3 - Odd only\n4 - Even only\n");
        printf("0 - Exit\n");
        fgets(buff, sizeof(buff), stdin); // Use fgets for choice input

        // Remove newline character at the end of choice
        buff[strcspn(buff, "\n")] = '\0';

        // Send the choice to the server
        sentbytes = send(sockfd, buff, strlen(buff) + 1, 0); // Send null-terminated string
        if (sentbytes == -1)
        {
            printf("Send error!\n");
            close(sockfd);
            return 1;
        }

        // Receive the server's response to the choice
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1)
        {
            printf("Receive error!\n");
            close(sockfd);
            return 1;
        }
        buff[recedbytes] = '\0'; // Null terminate the received string
        puts(buff);              // Print the result from the server

        // If the user chooses to exit, break out of the loop
        if (strcmp(buff, "Exit") == 0)
        {
            exit = 1;
        }
    }

    close(sockfd); // Close the socket before exiting
    return 0;
}
