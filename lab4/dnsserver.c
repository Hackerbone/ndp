#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024
#define DATABASE_FILE "database.txt"

struct dns_entry {
    char domain[50];
    char ip[50];
};

// Load database into an array of dns_entry structures
void load_database(struct dns_entry *database, int *entry_count) {
    FILE *file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        perror("Error opening database file");
        exit(1);
    }

    *entry_count = 0;
    while (fscanf(file, "%s %s", database[*entry_count].domain, database[*entry_count].ip) != EOF) {
        (*entry_count)++;
    }

    fclose(file);
}

// Handle the client's request
void handle_client(int client_socket, struct dns_entry *database, int entry_count) {
    char domain[MAX_BUFFER];
    char response[MAX_BUFFER];

    // Receive the domain name from the client
    int recv_len = recv(client_socket, domain, sizeof(domain), 0);
    if (recv_len < 0) {
        perror("Error receiving data from client");
        return;
    }
    domain[recv_len] = '\0';  // Ensure the domain string is null-terminated

    printf("Received domain: %s\n", domain);

    // Search for the domain in the database
    int found = 0;
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(database[i].domain, domain) == 0) {
            // Found the domain, send the corresponding IP address
            strcpy(response, database[i].ip);
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(response, "Domain not found");
    }

    // Send the response to the client
    send(client_socket, response, strlen(response), 0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    struct dns_entry database[100];
    int entry_count;

    // Load database from file
    load_database(database, &entry_count);
    printf("Loaded database!");
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == 0) {
        printf("Server listening on port %d...\n", PORT);
    } else {
        perror("Error listening on socket");
        exit(1);
    }

    // Accept client connections and handle them
    addr_size = sizeof(client_addr);
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }
        printf("Connection established with client\n");


        handle_client(client_socket, database, entry_count);

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
