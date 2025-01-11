#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXSIZE 300

int splitCharText(char *text, int arr[])
{
    char *token = strtok(text, ","); // Split by comma
    int arrSize = 0;

    // Split the string by commas and convert to integers
    while (token != NULL)
    {
        arr[arrSize++] = atoi(token); // Convert the token to an integer
        token = strtok(NULL, ",");    // Get the next token
    }

    // Return the size of the array
    return arrSize;
}

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int client_data, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;

    char client_message[MAXSIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("\nSocket creation error");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(7878);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // Use htonl for proper network byte order
    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if (retval == -1)
    {
        printf("Binding error");
        close(sockfd);
        return 1;
    }

    retval = listen(sockfd, 1);
    if (retval == -1)
    {
        close(sockfd);
        return 1;
    }
    printf("Server started and listening at port %d\n", ntohs(serveraddr.sin_port));

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

    if (newsockfd == -1)
    {
        close(sockfd);
        return 1;
    }

    int exit = 0; // exit flag
    int intArr[20];

    while (!exit)
    {
        client_data = recv(newsockfd, client_message, sizeof(client_message) - 1, 0);

        if (client_data == -1)
        {
            close(sockfd);
            close(newsockfd);
            return 1;
        }

        client_message[client_data] = '\0'; // Null-terminate the received message

        puts(client_message);
        printf("\n");
        printf("Comparing with exit: %s: %d\n", client_message, strcmp(client_message, "exit"));

        if (strcmp(client_message, "exit") == 0)
        {
            exit = 1;
            printf("\nExit command received... exiting\n");
        }
        else
        {
            printf("\nPrinting split char text\n");
            int size = splitCharText(client_message, intArr); // Get the size of the array

            // Print the parsed integers
            printf("Array size = %d\n", size);
            for (int i = 0; i < size; i++)
            {
                printf("Element %d = %d\n", i, intArr[i]);
            }

            char toSend[MAXSIZE];
            strcpy(toSend, "Enter choice of operation:"); // Properly initialize the string

            sentbytes = send(newsockfd, toSend, strlen(toSend) + 1, 0); // Send null-terminated string

            if (sentbytes == -1)
            {
                printf("\nCould not send bytes\n");
                close(sockfd);
                close(newsockfd);
                return 1;
            }

            client_data = recv(newsockfd, client_message, sizeof(client_message) - 1, 0);

            if (client_data == -1)
            {
                close(sockfd);
                close(newsockfd);
                return 1;
            }

            client_message[client_data] = '\0'; // Null-terminate the received message

            puts(client_message);
            printf("\n");
            printf("Comparing with exit: %s\n", client_message);

            int choice = 0;
            // Ensure valid parsing
            if (sscanf(client_message, "%d", &choice) != 1)
            {
                printf("Invalid input. Please enter a valid number.\n");
                continue;
            }

            printf("Choice: %d\n", choice);

            switch (choice)
            {
            case 0:
                printf("No choice\n");
                break;
            case 1:
                printf("Ascending\n");
                break;
            case 2:
                printf("Descending\n");
                break;
            case 3:
                printf("Odd only\n");
                break;
            case 4:
                printf("Even only\n");
                break;
            default:
                printf("Invalid choice\n");
                break;
            }
        }
    }

    close(sockfd);
    close(newsockfd);

    return 0;
}
