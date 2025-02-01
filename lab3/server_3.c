#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to compare numbers (for sorting)
int compare_numbers(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Function to compare characters (for sorting)
int compare_chars_desc(const void *a, const void *b) {
    return (*(char *)b - *(char *)a);
}

// Function to separate numbers and characters from the alphanumeric string
void separate_chars_and_numbers(char *input, char *chars, int *numbers, int *num_count) {
    *num_count = 0;
    int char_index = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i])) {
            numbers[*num_count] = input[i] - '0';
            (*num_count)++;
        } else {
            chars[char_index] = input[i];
            char_index++;
        }
    }
    chars[char_index] = '\0';
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections...\n");

    // Accept client connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Receive string from client
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        perror("Receive failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Received string: %s\n", buffer);

    char chars[BUFFER_SIZE];
    int numbers[BUFFER_SIZE];
    int num_count;

    // Separate characters and numbers from the string
    separate_chars_and_numbers(buffer, chars, numbers, &num_count);

    // Fork to create a child process
    pid = fork();

    if (pid == 0) {  // Child process
        // Sort the numbers in ascending order
        qsort(numbers, num_count, sizeof(int), compare_numbers);

        // Prepare the result string with sorted numbers
        printf("Child PID: %d\n", getpid());
        printf("Sorted numbers (ascending order): ");
        for (int i = 0; i < num_count; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

        // Send sorted numbers to the client
        send(client_fd, "Sorted numbers: ", 16, 0);
        for (int i = 0; i < num_count; i++) {
            char num_str[2];
            sprintf(num_str, "%d", numbers[i]);
            send(client_fd, num_str, strlen(num_str), 0);
        }
        send(client_fd, "\n", 1, 0);

        close(client_fd);
        exit(0);
    } else if (pid > 0) {  // Parent process
        // Sort the characters in descending order
        qsort(chars, strlen(chars), sizeof(char), compare_chars_desc);

        // Prepare the result string with sorted characters
        printf("Parent PID: %d\n", getpid());
        printf("Sorted characters (descending order): %s\n", chars);

        // Send sorted characters to the client
        send(client_fd, "Sorted characters: ", 19, 0);
        send(client_fd, chars, strlen(chars), 0);
        send(client_fd, "\n", 1, 0);

        wait(NULL);  // Wait for the child process to finish
    } else {
        perror("Fork failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Close the server socket
    close(server_fd);
    return 0;
}
