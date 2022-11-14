// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8000

 
void socket_connectio(int *sock,struct sockaddr_in *serv_addr,uint16_t port, int *client_fd);

int main(int argc, char const* argv[])
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    char *ite_buffer;
    int conteo_data;

    char* hello = "Hello from client";

    socket_connectio( & sock, &serv_addr, PORT , &client_fd);

    send(sock, hello, strlen(hello), 0);

    printf("Hello message sent\n");

    valread= -1;
    while(valread==-1){
        valread = read(sock, buffer, 1024);
    }

    if(valread!=-1){
        ite_buffer= buffer;
        printf("Numero: %f\n",atof(ite_buffer));
        conteo_data=1;

        while(conteo_data<2){
            ite_buffer++;
            if(*ite_buffer=='\n'){
                ite_buffer++;
                printf("Numero: %f\n",atof(ite_buffer));
                conteo_data++;
            }
        }

    }
    

    printf("%d\n",valread);
    printf("%s\n", buffer);
 
    // closing the connected socket
    close(client_fd);
    return 0;
}



void socket_connectio(int *sock,struct sockaddr_in *serv_addr,uint16_t port, int *client_fd){

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(port);

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(-1);
    }
    
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &(serv_addr->sin_addr)) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(-1);
    }

    if ((*client_fd = connect(*sock, (struct sockaddr*) serv_addr, sizeof(*serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        exit(-1);
    }


    int flags = fcntl(*sock, F_GETFL, 0);
    if (flags == -1){
        printf("\nConnection read flags \n");
        exit(-1);
    }
    flags |= O_NONBLOCK;
    fcntl(*sock, F_SETFL, flags);
}

