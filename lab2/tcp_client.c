// tcp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    char filename[MAX_BUFFER];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Get filename from user
    printf("Enter the filename: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0; // Remove newline character

    send(sockfd, filename, strlen(filename), 0);

    // Receive response from server
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    if (strncmp(buffer, "File not present", 17) == 0) {
        close(sockfd);
        return 0;
    }

    while (1) {
        printf("Enter your choice (1- Search, 2- Replace, 3- Reorder, 4- Exit): ");
        int choice;
        scanf("%d", &choice);
        getchar(); // Consume newline

        send(sockfd, &choice, sizeof(choice), 0);

        if (choice == 1) { // Search
            char search_str[MAX_BUFFER];
            printf("Enter string to search: ");
            fgets(search_str, sizeof(search_str), stdin);
            search_str[strcspn(search_str, "\n")] = 0; // Remove newline
            send(sockfd, search_str, strlen(search_str), 0);
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        } else if (choice == 2) { // Replace
            char old_str[MAX_BUFFER], new_str[MAX_BUFFER];
            printf("Enter string to replace: ");
            fgets(old_str, sizeof(old_str), stdin);
            old_str[strcspn(old_str, "\n")] = 0; // Remove newline
            printf("Enter replacement string: ");
            fgets(new_str, sizeof(new_str), stdin);
            new_str[strcspn(new_str, "\n")] = 0; // Remove newline
            send(sockfd, old_str, strlen(old_str), 0);
            send(sockfd, new_str, strlen(new_str), 0);
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        } else if (choice == 3) { // Reorder
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        } else if (choice == 4) { // Exit
            break;
        }
    }

    close(sockfd);
    return 0;
}
