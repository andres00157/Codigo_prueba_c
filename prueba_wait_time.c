
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

int cond_time_leap(float tiempo);

int main(){

    printf("COmenzo\n");

    while(cond_time_leap(10)==0);

    printf("termino\n");

    return 0;
}


int cond_time_leap(float tiempo){


    static struct timeval actual_time;
    static struct timeval last_time;
    static int state = 0;

    if(state==0){
        gettimeofday(&last_time, NULL);
        state = 1;
        return 0;
    }else{
        gettimeofday(&actual_time, NULL);
        if((actual_time.tv_sec - last_time.tv_sec) + 1e-6*(actual_time.tv_usec - last_time.tv_usec)>tiempo){
            gettimeofday(&last_time, NULL);
            return 1;
        }else{
            return 0;
        }
    }

}


