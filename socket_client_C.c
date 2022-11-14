// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8080
 
void socket_connection(int *sock,struct sockaddr_in *serv_addr,uint16_t port, int *client_fd);
void read_data_socket(int valread, char *buffer, float *num_in_python);
int vec2str(char *buffer_send, float *vec, int tam);

int main(int argc, char const* argv[])
{

    /// Inicializaciones
    int sock = 0, valread=-1, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    char buff_send[1024];
    char *ite_buffer;
    int conteo_data;
    float num_in_python[6];
    int flag_continue = 1;
    int tam_buffer;
    float id=0;

    socket_connection( & sock, &serv_addr, PORT , &client_fd);

    printf("1\n");
    // COMIENZA CODIGO
    while(1){
        printf("2\n");
        // Esperar buffer empty
        while(valread<=0){
            valread = read(sock, buffer, 1024);
        }
        printf("3\n");

        read_data_socket(valread,buffer,num_in_python);

        for(int i = 0;i<6;i++){
            printf("%f ",num_in_python[i]);
        }
        printf("\n");
        // Cargar valores de 0:emocion, 1:espasticidad, 2:dolor, 3:realidad_virtual, 4:genero, 5:stop

        flag_continue = 1;
        while(flag_continue){
            // Lee leapmotion
            printf("LEAPMOTION YA LEYO?: ");
            int a = 0;
            scanf("%d",&a); 
            // Hace  iteracion control

            valread = read(sock, buffer, 1024);
            if(valread>0){// el buffer no esta empty
                // Actualizar valores
                read_data_socket(valread,buffer,num_in_python);
            }

            if(num_in_python[5]==1){
                // enviar datos a python
                // id, ambiente_rv, disposicion, intensidad_torque, torque_control, torque_final, parar
                    
                float a[7]= {0,19.5455,1,2.45,5.6,12.3,0};
                tam_buffer = vec2str(buff_send, a, 7);
                send(sock, buff_send, tam_buffer, 0);

                //resetearia
                flag_continue = 0;
            }else{
                printf("Enviando mensaje\n");
                float a[7]= {0,19.5455,1,2.45,5.6,12.3,9.8};
                a[0]= id;
                id+=1;
                tam_buffer = vec2str(buff_send, a, 7);
                send(sock, buff_send, (tam_buffer), 0);
            }
        }

    }



 
    // closing the connected socket
    close(client_fd);
    return 0;
}

void read_data_socket(int valread, char *buffer, float *num_in_python){
    char *ite_buffer = buffer;
    int conteo_data;

    if(valread!=-1){
        ite_buffer= buffer;

        *num_in_python= atof(ite_buffer);
        num_in_python++;
        conteo_data=1;

        while(conteo_data<6){
            ite_buffer++;
            if(*ite_buffer=='\n'){
                ite_buffer++;
                *num_in_python= atof(ite_buffer);
                num_in_python++;
                conteo_data++;
            }
        }

    }
}


int vec2str(char *buffer_send, float *vec, int tam){
    int last_digit=0 ;
    for(int i = 0;i<tam;i++){

        gcvt(*vec, 20, buffer_send);
        last_digit+= strlen(buffer_send);

        buffer_send+= strlen(buffer_send);
        *buffer_send= '\n';
        buffer_send++;
        vec++;

    }
    return last_digit;
}

void socket_connection(int *sock,struct sockaddr_in *serv_addr,uint16_t port, int *client_fd){

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

