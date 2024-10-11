#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_socket = -1;
    struct addrinfo hints, *res = NULL;
    char *address = NULL;
    char *port = NULL;
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(1);
    }
    address = argv[1];
    port = argv[2];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      
    hints.ai_socktype = SOCK_STREAM;  
    int status = getaddrinfo(address, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next) {
        client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (client_socket < 0) {
            perror("Socket Error");
            continue;
        }
        if (connect(client_socket, p->ai_addr, p->ai_addrlen) < 0) {
            perror("Connect Error");
            close(client_socket);
            continue;
        }
        break; 
    }
    if (p == NULL) {
        fprintf(stderr, "Failed to connect to %s:%s\n", address, port);
        freeaddrinfo(res);
        exit(1);
    }
    freeaddrinfo(res);
    printf("Connected to %s:%s\n", address, port);
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    ssize_t bytes_sent, bytes_received;
    while (fgets(send_buffer, sizeof(send_buffer), stdin) != NULL) {
        size_t len = strlen(send_buffer);
        bytes_sent = send(client_socket, send_buffer, len, 0);
        if (bytes_sent < 0) {
            perror("Send Error");
            break;
        }
    }
    printf("Closing connection.\n");
    close(client_socket);
}
