#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define BUFFER_SIZE 1024
int main(int argc, char const* argv[])
{
    int server_socket, new_socket;
    char* message = "I'm server";
    struct sockaddr_in server_address;
    int option = 1;
    char buffer[BUFFER_SIZE] = {0};

    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0){
        perror("Socket Error");
        exit(1);
    }


    if(setsocketopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0){
        perror("Can't set option");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    
    if(bind(server_socket, (struct sockadd*)&server_address, sizeof(server_address)) < 0){
        perror("Can't bind");
        exit(1);
    }
    
    if(listen(server_socket, SOMAXCONN) < 0){
        perror("Can't listen");
        exit(1);
    }
    
   new_socket = accept(server_socket, (struct sockadd*)&server_address, sizeof(server_address));
   if(new_socket < 0){
        perror("Can't bind");
        exit(1);
   }

     
    read(new_socket, buffer, BUFFER_SIZE-1)
    printf("%s\n", buffer, );
    close();
    printf("message sent\n");
    close();

    
    
    return 0;
}
