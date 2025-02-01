#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024

// Structure to hold student information
typedef struct
{
    char registration_number[20];
    char name[100];
    char subject_code[10];
} StudentInfo;

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    StudentInfo student_info;

    // Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting to server");
        close(sock);
        exit(1);
    }

    // Ask user for the option
    printf("Enter option (1: Registration Number, 2: Name, 3: Subject Code): ");
    int option;
    scanf("%d", &option);
    getchar(); // to consume the newline character

    // Send the option to the server
    snprintf(buffer, sizeof(buffer), "%d", option);
    write(sock, buffer, strlen(buffer) + 1);

    // Ask user for the corresponding details based on the option
    if (option == 1)
    {
        printf("Enter Registration Number: ");
        fgets(student_info.registration_number, sizeof(student_info.registration_number), stdin);
        student_info.registration_number[strcspn(student_info.registration_number, "\n")] = 0;
    }
    else if (option == 2)
    {
        printf("Enter Name of the Student: ");
        fgets(student_info.name, sizeof(student_info.name), stdin);
        student_info.name[strcspn(student_info.name, "\n")] = 0;
    }
    else if (option == 3)
    {
        printf("Enter Subject Code: ");
        fgets(student_info.subject_code, sizeof(student_info.subject_code), stdin);
        student_info.subject_code[strcspn(student_info.subject_code, "\n")] = 0;
    }

    // Send the student information to the server
    write(sock, &student_info, sizeof(student_info));

    // Receive the response from the server
    read(sock, buffer, sizeof(buffer));
    printf("Server Response: %s\n", buffer);

    close(sock);
    return 0;
}
