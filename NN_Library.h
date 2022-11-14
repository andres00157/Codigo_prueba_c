#ifndef NN_LIBRARY_H_
#define NN_LIBRARY_H_

#include <stdio.h>
#include <math.h>
#include "readCSVFile.h"
#include <stdlib.h>

#define MAX12  1.9251
#define MIN12  -2.3421
#define MAX34 1.1023
#define MIN34 -1.3443
#define MAX56 55.5066
#define MIN56 -64.0526
#define MAX7 0.4269
#define MIN7 0.2441
#define MAX810 2.1599
#define MIN810 -0.2899





struct transform_data{
    float xoffset[6];
    float gain[6];
    float ymin;
};
typedef struct transform_data transform_data;

struct NN_param{
    transform_data x1_step1;
    float b1[20];
    float IW1_1[120];
    float b2;
    float LW2_1[20];
    float xoffset;
    float gain;
    float ymin;
};
typedef struct NN_param NN_param;

struct NN_param_m{
    transform_data x1_step1;
    float b1[25];
    float IW1_1[150];
    float b2;
    float LW2_1[25];
    float xoffset;
    float gain;
    float ymin;
};
typedef struct NN_param_m NN_param_m;


float NN_function(NN_param *NN,float *datos);
float NN_function_m(NN_param_m *NN,float *datos);
float vec_dot(float *vec1,float *vec2, int len);
int inicializacion_parametros_redes(NN_param *NN1, NN_param *NN2,NN_param *NN3,NN_param_m *NN4);
void fill_matrix(float *matrix, float *vec,int len);
void vec_sum(float *vec1,float *vec2, int len,float *vec_out);
void mapminmax_apply(float *x,transform_data *x1_step1, float *out, float len);
void print_matrix(float *matrix, int filas, int columnas);
void mul_mtx(float *matrix1,int filas1, int columnas1,float *matrix2_T, int filas2, int columnas2, float *mtx_out);
void tansig(float *vec,int len);
void normalizar(float *datos_in, float *datos_norm);
void normalizar_inversa(float *datos_out_norm, float *datos_out);

 #endif
