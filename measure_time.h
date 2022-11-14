
 #ifndef MEASURE_TIME_H_
 #define MEASURE_TIME_H_


#include <sys/time.h>
#include <time.h>
#include <stdio.h>

typedef struct timeval timeval;

struct time_struct{
    timeval start_time;
    timeval stop_time;
};
typedef struct time_struct time_struct;

void start_v2(time_struct *tiempo);
void stop_v2(time_struct *tiempo);
float time_diff_v2(time_struct *tiempo);

void start();
void stop();
float time_diff();
void printime();
#endif 