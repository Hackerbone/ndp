#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_BUFFER 1024

// Structure to hold student information
typedef struct
{
    char registration_number[20];
    char name[100];
    char subject_code[10];
} StudentInfo;

// Function to handle the client request based on the option
void handle_client(int client_sock)
{
    char buffer[MAX_BUFFER];
    ssize_t bytes_read;

    // Receive the client option
    bytes_read = read(client_sock, buffer, MAX_BUFFER);
    if (bytes_read <= 0)
    {
        perror("Error reading from client");
        close(client_sock);
        return;
    }

    // Parse the received data
    int option = atoi(buffer);
    StudentInfo student_info;

    // Receive the student information based on the selected option
    bytes_read = read(client_sock, &student_info, sizeof(StudentInfo));
    if (bytes_read <= 0)
    {
        perror("Error reading student info from client");
        close(client_sock);
        return;
    }

    pid_t pid;
    switch (option)
    {
    case 1: // Registration number
        pid = fork();
        if (pid == 0)
        { // Child process
            // Send the name and residential address
            snprintf(buffer, MAX_BUFFER, "Name: %s, Residential Address: XYZ, PID: %d",
                     student_info.name, getpid());
            write(client_sock, buffer, strlen(buffer) + 1);
            exit(0);
        }
        break;

    case 2: // Name of the student
        pid = fork();
        if (pid == 0)
        { // Child process
            // Send the student enrollment details
            snprintf(buffer, MAX_BUFFER, "Dept: CS, Semester: 5, Section: A, Courses: Math, Physics, PID: %d",
                     getpid());
            write(client_sock, buffer, strlen(buffer) + 1);
            exit(0);
        }
        break;

    case 3: // Subject Code
        pid = fork();
        if (pid == 0)
        { // Child process
            // Send the corresponding marks for the subject
            snprintf(buffer, MAX_BUFFER, "Subject Code: %s, Marks: 85, PID: %d",
                     student_info.subject_code, getpid());
            write(client_sock, buffer, strlen(buffer) + 1);
            exit(0);
        }
        break;

    default:
        snprintf(buffer, MAX_BUFFER, "Invalid Option");
        write(client_sock, buffer, strlen(buffer) + 1);
    }

    // Wait for all child processes to finish
    wait(NULL);
    close(client_sock);
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create a TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket");
        close(server_sock);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) < 0)
    {
        perror("Error listening on socket");
        close(server_sock);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept a client connection
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0)
        {
            perror("Error accepting client connection");
            continue;
        }

        // Handle the client request
        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}
