#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char domain[MAX_BUFFER];
    char response[MAX_BUFFER];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Assuming server is running locally

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    // Get domain name from the user
    printf("Enter domain name to lookup: ");
    fgets(domain, sizeof(domain), stdin);
    domain[strcspn(domain, "\n")] = 0;  // Remove newline character

    // Send the domain name to the server
    send(sock, domain, strlen(domain), 0);
    printf("Sent domain: %s\n", domain);

    // Receive the response from the server
    int recv_len = recv(sock, response, sizeof(response), 0);
    if (recv_len < 0) {
        perror("Error receiving data from server");
        close(sock);
        exit(1);
    }
    response[recv_len] = '\0';  // Null-terminate the response

    // Print the response (IP address or error message)
    printf("Server Response: %s\n", response);

    close(sock);
    return 0;
}
