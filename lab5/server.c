#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 2

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t lock;

void *handle_client(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    free(socket_desc);

    char client_message[2000];
    int read_size;

    while ((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        client_message[read_size] = '\0';
        printf("Received message: %s\n", client_message);
    }

    if (read_size == 0)
    {
        printf("Client disconnected\n");
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }

    close(sock);
    pthread_mutex_lock(&lock);
    client_count--;
    pthread_mutex_unlock(&lock);

    return 0;
}

int main(int argc, char *argv[])
{
    int server_fd, new_socket, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        printf("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 3);

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        printf("Connection accepted\n");

        pthread_mutex_lock(&lock);
        if (client_count >= MAX_CLIENTS)
        {
            pthread_mutex_unlock(&lock);
            printf("Maximum clients connected. Terminating session.\n");
            close(new_socket);
            break;
        }
        client_sockets[client_count++] = new_socket;
        pthread_mutex_unlock(&lock);

        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        printf("Handler assigned\n");
    }

    if (new_socket < 0)
    {
        perror("Accept failed");
        return 1;
    }

    return 0;
}