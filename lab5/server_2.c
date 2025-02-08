// server_2.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void *handle_client(void *arg);
int are_anagrams(const char *str1, const char *str2);

int main(int argc, char *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        printf("Connected to client: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        time_t now = time(NULL);
        printf("Connection time: %s", ctime(&now));

        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) < 0)
        {
            perror("Could not create thread");
            free(new_sock);
            close(new_socket);
        }

        pthread_detach(thread_id);
    }

    if (new_socket < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    close(server_fd);
    return 0;
}

void *handle_client(void *arg)
{
    int sock = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE] = {0};
    char str1[BUFFER_SIZE] = {0}, str2[BUFFER_SIZE] = {0};
    int valread;

    valread = read(sock, buffer, BUFFER_SIZE);
    sscanf(buffer, "%s %s", str1, str2);

    printf("Received strings: '%s' '%s'\n", str1, str2);

    if (are_anagrams(str1, str2))
    {
        send(sock, "The strings are anagrams.\n", strlen("The strings are anagrams.\n"), 0);
    }
    else
    {
        send(sock, "The strings are not anagrams.\n", strlen("The strings are not anagrams.\n"), 0);
    }

    close(sock);
    return NULL;
}

int are_anagrams(const char *str1, const char *str2)
{
    int count[256] = {0};

    if (strlen(str1) != strlen(str2))
    {
        printf("Lengths of the strings are different.\n %lu %lu\n", strlen(str1), strlen(str2));
        return 0;
    }

    for (int i = 0; str1[i] && str2[i]; i++)
    {
        count[(unsigned char)str1[i]]++;
        count[(unsigned char)str2[i]]--;
    }

    for (int i = 0; i < 256; i++)
    {
        if (count[i] != 0)
        {
            return 0;
        }
    }

    return 1;
}