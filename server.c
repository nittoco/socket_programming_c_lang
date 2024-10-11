#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void *handle_client(void *arg);

int main(int argc, char *argv[]) {
    int server_socket = -1;
    struct addrinfo hints, *res = NULL;
    int socket_option = 1;
    char *address = NULL;
    char *port = NULL;
    if (argc >= 2) {
        address = argv[1];
    } else {
        address = "0.0.0.0";
    }
    if (argc >= 3) {
        port = argv[2];
    } else {
        port = "8080";
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;     
    int status = getaddrinfo(address, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        goto cleanup;
    }
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next) {
        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket < 0) {
            perror("Socket Error");
            continue;
        }
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option, sizeof(socket_option)) < 0) {
            perror("Can't set socket option");
            close(server_socket);
            continue;
        }
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) < 0) {
            perror("Can't bind");
            close(server_socket);
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "Failed to bind to any address\n");
        goto cleanup;
    }
    freeaddrinfo(res);
    if (listen(server_socket, SOMAXCONN) < 0) {
        perror("Can't listen");
        goto cleanup;
    }
    printf("Server is listening on %s:%s\n", address, port);
    while (1) {
        struct sockaddr_storage client_address;
        socklen_t client_address_len = sizeof(client_address);
        int new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket < 0) {
            perror("Accept Error");
            continue;
        }
        pthread_t thread_id;
        int *pclient = malloc(sizeof(int));
        if (pclient == NULL) {
            perror("Malloc Error");
            close(new_socket);
            continue;
        }
        *pclient = new_socket;
        if (pthread_create(&thread_id, NULL, handle_client, pclient) != 0) {
            perror("Thread Creation Error");
            close(new_socket);
            free(pclient);
            continue;
        } else {
            pthread_detach(thread_id);
        }
    }

cleanup:
    if (res != NULL) {
        freeaddrinfo(res);
    }
    if (server_socket != -1) {
        close(server_socket);
    }
    exit(1);
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytes_read) < 0) {
            perror("Write Error");
            break;
        }
    }
    if (bytes_read < 0) {
        perror("Read Error");
    }
    close(client_socket);
    return NULL;
}
