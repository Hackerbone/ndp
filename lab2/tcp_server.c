// tcp_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8081
#define MAX_BUFFER 1024

// Function to search a string in the file
int search_string_in_file(char *filename, char *search_str)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        return -1; // File not found
    }

    char line[MAX_BUFFER];
    int count = 0;
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, search_str))
        {
            count++;
        }
    }

    fclose(file);
    return count;
}

// Function to replace string in the file
int replace_string_in_file(char *filename, char *old_str, char *new_str)
{
    FILE *file = fopen(filename, "r+");
    if (!file)
    {
        return -1; // File not found
    }

    char temp[MAX_BUFFER];
    long pos;
    int found = 0;
    while (fgets(temp, sizeof(temp), file))
    {
        pos = ftell(file);
        if (strstr(temp, old_str))
        {
            found = 1;
            // Replace occurrence
            fseek(file, pos - strlen(temp), SEEK_SET);
            fprintf(file, "%s", new_str);
        }
    }

    fclose(file);
    return found;
}

// Function to reorder lines in file based on ASCII value
int reorder_file_content(char *filename)
{
    FILE *file = fopen(filename, "r+");
    if (!file)
    {
        return -1; // File not found
    }

    char temp[MAX_BUFFER];
    fseek(file, 0, SEEK_SET);
    char *lines[MAX_BUFFER];
    int line_count = 0;

    while (fgets(temp, sizeof(temp), file))
    {
        lines[line_count] = strdup(temp);
        line_count++;
    }

    // Sort lines based on ASCII value
    for (int i = 0; i < line_count - 1; i++)
    {
        for (int j = i + 1; j < line_count; j++)
        {
            if (strcmp(lines[i], lines[j]) > 0)
            {
                char *temp_line = lines[i];
                lines[i] = lines[j];
                lines[j] = temp_line;
            }
        }
    }

    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < line_count; i++)
    {
        fputs(lines[i], file);
        free(lines[i]);
    }

    fclose(file);
    return 0;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER];
    char filename[MAX_BUFFER];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        int bytes_read = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read < 0)
        {
            perror("Read failed");
            close(new_socket);
            continue;
        }
        buffer[bytes_read] = '\0'; // Ensure null-termination

        // Trim any trailing newline or carriage return characters
        buffer[strcspn(buffer, "\r\n")] = '\0';

        strncpy(filename, buffer, sizeof(filename));
        filename[sizeof(filename) - 1] = '\0'; // Ensure null-termination

        printf("Received filename: %s\n", filename); // Debugging statement

        FILE *file = fopen(filename, "r");
        if (!file)
        {
            perror("File open failed"); // Debugging statement
            send(new_socket, "File not present", 17, 0);
            close(new_socket);
            continue;
        }

        send(new_socket, "File found. Select an option:\n1. Search\n2. Replace\n3. Reorder\n4. Exit", 75, 0);

        int choice;
        while (1)
        {
            recv(new_socket, &choice, sizeof(choice), 0);
            if (choice == 4)
            {
                close(new_socket);
                break;
            }

            if (choice == 1)
            { // Search
                char search_str[MAX_BUFFER];
                recv(new_socket, search_str, sizeof(search_str), 0);
                int count = search_string_in_file(filename, search_str);
                if (count >= 0)
                {
                    char result[MAX_BUFFER];
                    snprintf(result, sizeof(result), "Found %d occurrences", count);
                    send(new_socket, result, strlen(result), 0);
                }
                else
                {
                    send(new_socket, "String not found", 17, 0);
                }
            }
            else if (choice == 2)
            { // Replace
                char old_str[MAX_BUFFER], new_str[MAX_BUFFER];
                recv(new_socket, old_str, sizeof(old_str), 0);
                recv(new_socket, new_str, sizeof(new_str), 0);
                int result = replace_string_in_file(filename, old_str, new_str);
                if (result == 1)
                {
                    send(new_socket, "String replaced", 15, 0);
                }
                else
                {
                    send(new_socket, "String not found", 17, 0);
                }
            }
            else if (choice == 3)
            { // Reorder
                int result = reorder_file_content(filename);
                if (result == 0)
                {
                    send(new_socket, "File reordered", 14, 0);
                }
                else
                {
                    send(new_socket, "Error reordering file", 21, 0);
                }
            }
        }
    }

    return 0;
}
