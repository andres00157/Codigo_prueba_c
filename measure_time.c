
#include "measure_time.h"


timeval start_time;
timeval stop_time;


void start_v2(time_struct *tiempo){
    gettimeofday(&(tiempo->start_time), NULL);
}

void stop_v2(time_struct *tiempo){
    gettimeofday(&(tiempo->stop_time), NULL);
}

float time_diff_v2(time_struct *tiempo){
    return (tiempo->stop_time.tv_sec - tiempo->start_time.tv_sec) + 1e-6*(tiempo->stop_time.tv_usec - tiempo->start_time.tv_usec);
}


void start(){
    gettimeofday(&start_time, NULL);
}

void stop(){
    gettimeofday(&stop_time, NULL);
}

float time_diff(){
    return (stop_time.tv_sec - start_time.tv_sec) + 1e-6*(stop_time.tv_usec - start_time.tv_usec);
}

void printime(){
    printf("The time was: %f\n",time_diff());
}


