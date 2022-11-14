#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int vec2str(char *buffer_send, float *vec, int tam);

int main(){

    float a[6]= {19.5455,1,2.45,5.6,12.3,89.9};
    char buff_send[1024];
    int tam_buffer;
    tam_buffer = vec2str(buff_send, a, 6);

    for(int i=0;i<tam_buffer;i++){
        printf("%c",buff_send[i]);
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