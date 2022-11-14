
#include "NN_Library.h"


void normalizar(float *datos_in, float *datos_norm){
    datos_norm[0] = (datos_in[0] - MIN12) / ( MAX12 - MIN12); 
    datos_norm[1] = (datos_in[1] - MIN12) / ( MAX12 - MIN12); 
    datos_norm[2] = (datos_in[2] - MIN34) / ( MAX34 - MIN34); 
    datos_norm[3] = (datos_in[3] - MIN34) / ( MAX34 - MIN34); 
    datos_norm[4] = (datos_in[4] - MIN56) / ( MAX56 - MIN56); 
    datos_norm[5] = (datos_in[5] - MIN56) / ( MAX56 - MIN56); 
}

void normalizar_inversa(float *datos_out_norm, float *datos_out){
    datos_out[0]=datos_out_norm[0]*(MAX810-MIN810)+MIN810;
    datos_out[1]=datos_out_norm[1]*(MAX810-MIN810)+MIN810;
    datos_out[2]=datos_out_norm[2]*(MAX810-MIN810)+MIN810;
    datos_out[3]=datos_out_norm[3]*(MAX7-MIN7)+MIN7;
}

int inicializacion_parametros_redes(NN_param *NN1, NN_param *NN2,NN_param *NN3,NN_param_m *NN4){
    float vec_offset[6] = {6.7155162862109e-07,0.227765751497821,0,0.260905029807448,0,0.401016135895025};
    float vec_gain[6] = {2.00000134860859,3.78564680696321,2,4.86867054572389,2,7.6089687160829};


    fill_matrix(NN1->x1_step1.xoffset,vec_offset,6);
    fill_matrix(NN1->x1_step1.gain,vec_gain,6);
    NN1->x1_step1.ymin = -1;

    fill_matrix(NN2->x1_step1.xoffset,vec_offset,6);
    fill_matrix(NN2->x1_step1.gain,vec_gain,6);
    NN2->x1_step1.ymin = -1;

    fill_matrix(NN3->x1_step1.xoffset,vec_offset,6);
    fill_matrix(NN3->x1_step1.gain,vec_gain,6);
    NN3->x1_step1.ymin = -1;

    fill_matrix(NN4->x1_step1.xoffset,vec_offset,6);
    fill_matrix(NN4->x1_step1.gain,vec_gain,6);
    NN4->x1_step1.ymin = -1;

    //Lectura b1
    FILE *csv = ini_LeerCsvData("b1.csv");
    if(csv==NULL){
        printf("Error leyendo");
        exit(0);
        return 1;
    }
    leer_valor(csv, NN1->b1,20);
    leer_valor(csv, NN2->b1,20);
    leer_valor(csv, NN3->b1,20);
    close_CSVData(csv);
    csv = ini_LeerCsvData("b1m.csv");
    if(csv==NULL){
        printf("Error leyendo");
        exit(0);
        return 1;
    }
    leer_valor(csv, NN4->b1,25);
    close_CSVData(csv);
    // Lectura IW1_1
    csv = ini_LeerCsvData("IWtotal.csv");
    if(csv==NULL){
        printf("Error leyendo");
        exit(0);
        return 1;
    }
    leer_valor(csv, NN1->IW1_1,120);
    leer_valor(csv, NN2->IW1_1,120);
    leer_valor(csv, NN3->IW1_1,120);
    close_CSVData(csv);
    csv = ini_LeerCsvData("IWm.csv");
    if(csv==NULL){
        printf("Error leyendo");
        exit(0);
        return 1;
    }
    leer_valor(csv, NN4->IW1_1,150);
    close_CSVData(csv);
    // Declarar b2
    NN1->b2 = 1.713663495945270121;
    NN2->b2 = 0.60287361652793691746;
    NN3->b2 = 1.32260691215875692;
    NN4->b2 = -1.33790902519107191;
    //Leer LW2_1
    csv = ini_LeerCsvData("LW_total.csv");
    if(csv==NULL){
        printf("Error leyendo");
        exit(0);
        return 1;
    }
    leer_valor(csv, NN1->LW2_1,20);
    leer_valor(csv, NN2->LW2_1,20);
    leer_valor(csv, NN3->LW2_1,20);
    close_CSVData(csv);
    csv = ini_LeerCsvData("LWm.csv");
    if(csv==NULL){
        printf("Error leyendo");
        exit(0);
        return 1;
    }
    leer_valor(csv, NN4->LW2_1,25);
    close_CSVData(csv);

    NN1->xoffset = 0.534912025083002;
    NN2->xoffset = 0;
    NN3->xoffset = 0.747131082103254;
    NN4->xoffset = 0;

    NN1->gain = 6.12965805719512;
    NN2->gain = 24.5186322287804;
    NN3->gain = 7.9092362028324;
    NN4->gain = 2;

    NN1->ymin = -1;
    NN2->ymin = -1;
    NN3->ymin = -1;
    NN4->ymin = -1;

}


void mapminmax_apply(float *x,transform_data *x1_step1, float *out,float len){
    int i;
    for(i = 0;i<len;i++){
        out[i] = ((x[i]- x1_step1->xoffset[i])*x1_step1->gain[i])+x1_step1->ymin;
    }
}


/*
/  fill_matrix:
/   
/  -Descripcion: 
/   
/  En esta funcion la matriz se iguala al vector 
/ de entrada
/  
/  -Entradas:
/        + apuntador matrix: apuntador a matriz a llenar.
         + vec :apuntador a vector con el que se va a llenar
                la matriz.     
*/
void fill_matrix(float *matrix, float *vec, int len){
    int i;
    for(i=0;i<len;i++){
        matrix[i]=vec[i];
    }
}



void print_matrix(float *matrix, int filas, int columnas){
    int i = 0;
    int j = 0;
    printf("Matriz: \n");
    for(i=0;i<filas;i++){
        for(j=0;j<columnas;j++){
            printf("%.20f ",*matrix);
            matrix++;
        }
        printf("\n");
    }
}

float NN_function(NN_param *NN,float *datos){
    float x_p[6];
    float out[20];
    float a2=0;
    mapminmax_apply(datos,&(NN->x1_step1),x_p, 6);
    mul_mtx(NN->IW1_1, 20, 6,x_p, 6, 1, out);
    
    vec_sum(out,NN->b1, 20,out);
    tansig(out,20);

    //printf("P\n");
    //print_matrix(out-1, 8, 3);
    //print_matrix(NN->LW2_1-1, 8, 3);

    mul_mtx(NN->LW2_1, 1, 20,out, 20, 1, &a2);
    
    a2 += NN->b2;

    a2 = (a2-NN->ymin)/NN->gain+NN->xoffset;
    return a2;
}
float NN_function_m(NN_param_m *NN,float *datos){
    float x_p[6];
    float out[25];
    float a2;
    mapminmax_apply(datos,&(NN->x1_step1),x_p, 6);


    mul_mtx(NN->IW1_1, 25, 6,x_p, 6, 1, out);
    
    vec_sum(out,NN->b1, 25,out);
    tansig(out,25);

    //printf("P\n");
    //print_matrix(out, 9, 3);
    //print_matrix(NN->LW2_1, 9, 3);

    mul_mtx(NN->LW2_1, 1, 25,out, 25, 1, &a2);
    
    a2 += NN->b2;

    a2 = (a2-NN->ymin)/NN->gain+NN->xoffset;
    return a2;
}

void tansig(float *vec,int len){
    int i;
    for(i=0;i<len;i++){
        vec[i]= 2/(1+exp(-2*vec[i]))-1;
    }
}




void mul_mtx(float *matrix1,int filas1, int columnas1,float *matrix2_T, int filas2, int columnas2, float *mtx_out){
    int i,j;
    float vec1,vec2;
    if(columnas1==filas2){
        for(i=0;i<filas1;i++){
            for(j=0;j<columnas2;j++){
                mtx_out[j+columnas2*i]= vec_dot(&(matrix1[columnas1*i]),&(matrix2_T[filas2*j]),columnas1);
            }
        }
    }else{
        printf("ERROR: tamanio matrices invalida. \n");
    }
}



void vec_sum(float *vec1,float *vec2, int len,float *vec_out){
    int i;

    for(i=0;i<len;i++){
        vec_out[i]= vec1[i]+vec2[i];
    }
}

float vec_dot(float *vec1,float *vec2, int len){
    int i;
    float dot=0;
    //printf("dot:\n");
    for(i=0;i<len;i++){
        dot+= vec1[i]*vec2[i];
    //    if((i+1)%3==0)printf("%f\n",dot+1.713663495945270121);
        //if(i>17){
        //    printf("%f * %f=>%f\n",vec1[i],vec2[i],dot);
        //}
    }
    return dot;
}


