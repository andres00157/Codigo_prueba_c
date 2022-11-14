
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "measure_time.h"

#define MPI 3.14159265358979323846
#define MAX_PLOT 10000

#define MAX_ITE 15

struct MF{ 
    char *nombre;
    int len_nombre;
    float media;
    float varianza;
    float plot[MAX_PLOT+1];
};
typedef struct MF MF;


struct data_fuzzy{ 
    float rango_min;
    float rango_max;
    int num_MF;
    MF *data_MF;
};
typedef struct data_fuzzy data_fuzzy;


struct fis{
    int num_input;
    int num_output;
    data_fuzzy * Input;
    data_fuzzy * Output;
    int *rules;
    int num_rules;
};
typedef struct fis fis;

void leer_fis(fis *fuzzy_logic);
void leer_fis_2(fis *fuzzy_logic);
void print_data_fuzzy(data_fuzzy *data);
void print_fis(fis *data);
void eval_fis(fis *data, float *val, float *out);
float eval_UA(MF *funcion, float valor);
void fill_plot(MF *funcion, float rango_min, float rango_max);
int str_comp(char *string1, char *string2, int max);
int str_num(char *string, int max);

int main(){

    fis fuzzy_logic,fuzzy_logic_2;
    leer_fis(&fuzzy_logic);
    leer_fis_2(&fuzzy_logic_2);
    float out_1[2],out_2[1];
    float input_1[3]= {100,3.6,0.77};
    float input_2[2]= {0,1.3};
    FILE* fichero;
    fichero = fopen("data_Fuzzy_Error_C_10000.txt", "wt");

    for(int i1 = 0; i1<MAX_ITE;i1++){
        printf("%d\n",i1);
        for(int i2 = 0; i2<MAX_ITE;i2++){
            printf("%d\n",i2);
            for(int i3 = 0; i3<MAX_ITE;i3++){
                for(int i4 = 0; i4<MAX_ITE;i4++){

                    start();
                    input_1[0]= i1*360/MAX_ITE;
                    input_1[1]= i2*6/MAX_ITE;
                    input_1[2]= i3*1/MAX_ITE;
                    input_2[1]= i4*5/MAX_ITE;

                    eval_fis(&fuzzy_logic , input_1, out_1);
                    input_2[0]= out_1[0];
                    eval_fis(&fuzzy_logic_2 , input_2, out_2);
                    stop();

                    fprintf(fichero, "%0.20f ",out_1[1]);
                    fprintf(fichero, "%0.20f ",out_1[0]);
                    fprintf(fichero, "%0.20f ",out_2[0]);
                    fprintf(fichero,"%0.20f\n",time_diff());
                }
            }
        }
    }

    input_1[0]= 200;
    input_1[1]= 4;
    input_1[2]= 0.77;
    input_2[1]= 1.3;
    eval_fis(&fuzzy_logic , input_1, out_1);
    input_2[0]= out_1[0];
    eval_fis(&fuzzy_logic_2 , input_2, out_2);

    printf("Output 1: %f, Output 2: %f \n",out_1[1],out_1[0]);
    printf("Output 3: %f\n",out_2[0]);



    return 0;
}

void fill_plot(MF *funcion, float rango_min, float rango_max){
    float delta = (rango_max-rango_min)/MAX_PLOT;
    float val = rango_min;
    int i;
    for(i=0;i<MAX_PLOT+1;i++){
        funcion->plot[i]= eval_UA(funcion, val);
        val+=delta;
    }
}
/*
Funcion encargada de realizar la evaluacion de la logica 
difusa para valor de entrada de las variables.

*/
void eval_fis(fis *data, float *val, float *out){
    // Declaracion variables
    int i,j,k;
    float UA,UA_aux, dato_plot, rango_min,rango_max,delta,x,sum1,sum2;
    float **plot=  malloc(data->num_output*sizeof(float *));
    float *p;
    int n_MF;

    // Creacion vector para cada una de las variables de salidas
    // de acuerdo al MAX_PLOT
    for(j=0;j<data->num_output;j++){
            plot[j] = malloc((MAX_PLOT+1)*sizeof(float));
            for(k=0;k<(MAX_PLOT+1);k++){
                plot[j][k]= 0;
            }
    }

    // Iteracion en en las reglas
    for(i=0;i<data->num_rules;i++){
        UA = 1;
        for(j=0;j<data->num_input;j++){
            UA_aux = eval_UA(&(data->Input[j].data_MF[-1+data->rules[j+i*(data->num_input+data->num_output)]]), val[j]);
            if(UA_aux<UA){
                UA = UA_aux;
            }
        }
        for(j=0;j<data->num_output;j++){
            p = plot[j];
            n_MF = data->rules[(j+data->num_input)+i*(data->num_input+data->num_output)]-1;
            for(k=0;k<(MAX_PLOT+1);k++){
                dato_plot = data->Output[j].data_MF[n_MF].plot[k];
                if(UA < dato_plot){
                    if(p[k]<UA){
                        p[k]= UA;
                    }
                }else{
                    // Saca el maximo entre las graficas de las 
                    // salidas de las diferentes graficas 
                    if(p[k]<dato_plot){
                        p[k]= dato_plot;
                    }
                }
            }
        }
    }

    // Defuzificacion
    for(j=0;j<data->num_output;j++){
        p = plot[j];
        rango_min = data->Output[j].rango_min;
        rango_max = data->Output[j].rango_max;
        delta = (rango_max-rango_min)/MAX_PLOT;
        x = rango_min;
        sum1=0;
        sum2=0;
        for(k=0;k<(MAX_PLOT+1);k++){
            //printf("%f\n",p[k]);
            sum1+=x*p[k];
            x+= delta;
            sum2+=p[k];
        }
        out[j]= sum1/sum2;
    }
    p = plot[0];
    rango_min = data->Output[0].rango_min;
    rango_max = data->Output[0].rango_max;
    delta = (rango_max-rango_min)/MAX_PLOT;
    x = rango_min;

}

float eval_UA(MF *funcion, float valor){
    float media = funcion->media;
    float varianza = (funcion->varianza);
    float out = exp(-((valor-media)/varianza)*((valor-media)/varianza)/2);
    return out;
}


void print_data_fuzzy(data_fuzzy *data){
    printf("Range=[%f %f]\n",data->rango_min,data->rango_max);
    printf("NumMFs=%d\n",data->num_MF);
    int i,j;
    for(i=0;i<data->num_MF;i++){
        printf("MF%d='",i+1);
        for(j = 0;j<data->data_MF[i].len_nombre;j++){
            printf("%c",data->data_MF[i].nombre[j]);
        }
        printf("':'gaussmf',[%f %f]\n",data->data_MF[i].varianza,data->data_MF[i].media);
    }
}

void print_fis(fis *data){
    int i,j;
    printf("\n");
    for(i = 0;i<data->num_input;i++){
        printf("[Input%d]\n",i+1);
        print_data_fuzzy(&(data->Input[i]));
        printf("\n");
    }
    for(i = 0;i<data->num_output;i++){
        printf("[Output%d]\n",i+1);
        print_data_fuzzy(&(data->Output[i]));
        printf("\n");
    }
    printf("[Rules]\n");
    for(i=0;i<data->num_rules;i++){
        for(j=0;j<data->num_input;j++){
            printf("%d ",data->rules[j+i*(data->num_input+data->num_output)]);
        }
        printf(", ");
        for(j=0;j<data->num_output;j++){
            printf("%d ",data->rules[(j+data->num_input)+i*(data->num_input+data->num_output)]);
        }
        printf("(1) : 1\n");
    }
}

void leer_fis_2(fis *fuzzy_logic){
    int i, resul,flag = 1, num,j;
    char lectura[60]; 
    FILE *fp;
	fp = fopen ( "ModeloRV.fis", "r" );
    if(fp==NULL){
            printf("Error leyendo\n");
            exit(0);
    }else{
            printf("CSV leido\n");
    }
    fuzzy_logic->num_input = 2;
    fuzzy_logic->num_output = 1;
    fuzzy_logic->Input =  malloc(fuzzy_logic->num_input*sizeof(data_fuzzy));
    fuzzy_logic->Output =  malloc(fuzzy_logic->num_output*sizeof(data_fuzzy));

    while(flag){
        fgets(lectura,60,fp);
        resul = str_comp( "NumRules\n", lectura, 60);
        if(resul==1){
            fuzzy_logic->num_rules = str_num(lectura, 60); 
            flag = 0;
        }
    }
    flag = 1;
    while(flag){
        fgets(lectura,60,fp);
        resul = str_comp( "[Rules]\n", lectura, 60);
        if(resul==1){
            flag = 0;
        }
    }


    data_fuzzy *input1= fuzzy_logic->Input;
    input1->rango_min = 0;
    input1->rango_max = 5;
    input1->num_MF = 5;
    //Creacion memoria
    input1->data_MF = malloc(input1->num_MF*sizeof(MF));
    // Declaracion MF1
    input1->data_MF->nombre = "Incomodo";
    input1->data_MF->len_nombre = 8;
    input1->data_MF->media = 0.166;
    input1->data_MF->varianza = 0.4111;
    // Declaracion MF2
    (input1->data_MF+1)->nombre = "Moderado_comodo";
    (input1->data_MF+1)->len_nombre = 16;
    (input1->data_MF+1)->media = 3.6;
    (input1->data_MF+1)->varianza = 0.5521;
    // Declaracion MF3
    (input1->data_MF+2)->nombre = "Moderado_incomodo";
    (input1->data_MF+2)->len_nombre = 18;
    (input1->data_MF+2)->media = 1.2;
    (input1->data_MF+2)->varianza = 0.5521;
    // Declaracion MF4
    (input1->data_MF+3)->nombre = "Neutro";
    (input1->data_MF+3)->len_nombre = 7;
    (input1->data_MF+3)->media = 2.4;
    (input1->data_MF+3)->varianza = 0.5521;
    // Declaracion MF5
    (input1->data_MF+4)->nombre = "Comodo";
    (input1->data_MF+4)->len_nombre = 7;
    (input1->data_MF+4)->media = 4.755;
    (input1->data_MF+4)->varianza = 0.5904;


    input1= fuzzy_logic->Input+1;
    input1->rango_min = 0;
    input1->rango_max = 5;
    input1->num_MF = 5;
    //Creacion memoria
    input1->data_MF = malloc(input1->num_MF*sizeof(MF));
    // Declaracion MF1
    input1->data_MF->nombre = "Facil";
    input1->data_MF->len_nombre = 6;
    input1->data_MF->media = 0.166;
    input1->data_MF->varianza = 0.4111;
    // Declaracion MF2
    (input1->data_MF+1)->nombre = "Moderado_Facil";
    (input1->data_MF+1)->len_nombre = 15;
    (input1->data_MF+1)->media = 1.2;
    (input1->data_MF+1)->varianza = 0.5521;
    // Declaracion MF3
    (input1->data_MF+2)->nombre = "Medio";
    (input1->data_MF+2)->len_nombre = 6;
    (input1->data_MF+2)->media = 2.4;
    (input1->data_MF+2)->varianza = 0.552;
    // Declaracion MF4
    (input1->data_MF+3)->nombre = "Moderado_Desafiante";
    (input1->data_MF+3)->len_nombre = 20;
    (input1->data_MF+3)->media = 3.6;
    (input1->data_MF+3)->varianza = 0.5521;
    // Declaracion MF5
    (input1->data_MF+4)->nombre = "Desafiante";
    (input1->data_MF+4)->len_nombre = 11;
    (input1->data_MF+4)->media = 4.755;
    (input1->data_MF+4)->varianza = 0.5903;


    data_fuzzy *output1= fuzzy_logic->Output;
    output1->rango_min = 0;
    output1->rango_max = 5;
    output1->num_MF = 5;
    //Creacion memoria
    output1->data_MF = malloc(output1->num_MF*sizeof(MF));
    // Declaracion MF1
    output1->data_MF->nombre = "Facil";
    output1->data_MF->len_nombre = 6;
    output1->data_MF->media = 0.166;
    output1->data_MF->varianza = 0.4111;
    // Declaracion MF2
    (output1->data_MF+1)->nombre = "Moderado_facil";
    (output1->data_MF+1)->len_nombre = 15;
    (output1->data_MF+1)->media = 1.2;
    (output1->data_MF+1)->varianza = 0.5521;
    // Declaracion MF3
    (output1->data_MF+2)->nombre = "Medio";
    (output1->data_MF+2)->len_nombre = 6;
    (output1->data_MF+2)->media = 2.4;
    (output1->data_MF+2)->varianza = 0.552;
    // Declaracion MF4
    (output1->data_MF+3)->nombre = "Modera_Desafiante";
    (output1->data_MF+3)->len_nombre = 18;
    (output1->data_MF+3)->media = 3.6;
    (output1->data_MF+3)->varianza = 0.5521;
    // Declaracion MF5
    (output1->data_MF+4)->nombre = "Entorno_Desafiante";
    (output1->data_MF+4)->len_nombre = 19;
    (output1->data_MF+4)->media = 4.755;
    (output1->data_MF+4)->varianza = 0.5903;


    fill_plot(output1->data_MF,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+1,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+2,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+3,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+4,output1->rango_min,output1->rango_max);

    fuzzy_logic->rules= malloc(fuzzy_logic->num_rules*(fuzzy_logic->num_input+fuzzy_logic->num_output)*sizeof(int));
    for(i = 0;i<fuzzy_logic->num_rules;i++){
        fgets(lectura,60,fp);
        for(j=0;j<fuzzy_logic->num_input;j++){
            fuzzy_logic->rules[j+i*(fuzzy_logic->num_input+fuzzy_logic->num_output)] =lectura[2*j]-48; 
        }
        for(j=0;j<fuzzy_logic->num_output;j++){
            fuzzy_logic->rules[(j+fuzzy_logic->num_input)+i*(fuzzy_logic->num_input+fuzzy_logic->num_output)] = lectura[2*fuzzy_logic->num_input+1+2*j]-48; 
        }
    }
    fclose (fp);      
}


void leer_fis(fis *fuzzy_logic){
    int i, resul,flag = 1, num,j;
    char lectura[60]; 
    FILE *fp;
	fp = fopen ( "ModeloEspasticidad.fis", "r" );
    if(fp==NULL){
            printf("Error leyendo\n");
            exit(0);
    }else{
            printf("CSV leido\n");
    }
    fuzzy_logic->num_input = 3;
    fuzzy_logic->num_output = 2;
    fuzzy_logic->Input =  malloc(fuzzy_logic->num_input*sizeof(data_fuzzy));
    fuzzy_logic->Output =  malloc(fuzzy_logic->num_output*sizeof(data_fuzzy));

    while(flag){
        fgets(lectura,60,fp);
        resul = str_comp( "NumRules\n", lectura, 60);
        if(resul==1){
            fuzzy_logic->num_rules = str_num(lectura, 60); 
            flag = 0;
        }
    }
    flag = 1;
    while(flag){
        fgets(lectura,60,fp);
        resul = str_comp( "[Rules]\n", lectura, 60);
        if(resul==1){
            flag = 0;
        }
    }


    data_fuzzy *input1= fuzzy_logic->Input;
    input1->rango_min = 0;
    input1->rango_max = 360;
    input1->num_MF = 4;
    //Creacion memoria
    input1->data_MF = malloc(input1->num_MF*sizeof(MF));
    // Declaracion MF1
    input1->data_MF->nombre = "Enojado";
    input1->data_MF->len_nombre = 7;
    input1->data_MF->media = 133.8;
    input1->data_MF->varianza = 39.92;
    // Declaracion MF2
    (input1->data_MF+1)->nombre = "Sereno";
    (input1->data_MF+1)->len_nombre = 6;
    (input1->data_MF+1)->media = 331.6;
    (input1->data_MF+1)->varianza =24.11;
    // Declaracion MF3
    (input1->data_MF+2)->nombre = "Feliz";
    (input1->data_MF+2)->len_nombre = 5;
    (input1->data_MF+2)->media = 24.61;
    (input1->data_MF+2)->varianza = 47.99;
    // Declaracion MF4
    (input1->data_MF+3)->nombre = "Deprimido";
    (input1->data_MF+3)->len_nombre = 9;
    (input1->data_MF+3)->media = 223.6;
    (input1->data_MF+3)->varianza = 35.12;

    input1= (fuzzy_logic->Input)+1;
    input1->rango_min = 0;
    input1->rango_max = 6;
    input1->num_MF = 6;
    //Creacion memoria
    input1->data_MF = malloc(input1->num_MF*sizeof(MF));
    // Declaracion MF1
    input1->data_MF->nombre = "0";
    input1->data_MF->len_nombre = 1;
    input1->data_MF->media = 0.166;
    input1->data_MF->varianza = 0.4111;
    // Declaracion MF2
    (input1->data_MF+1)->nombre = "1";
    (input1->data_MF+1)->len_nombre = 1;
    (input1->data_MF+1)->media = 1.2;
    (input1->data_MF+1)->varianza =0.5521;
    // Declaracion MF3
    (input1->data_MF+2)->nombre = "2";
    (input1->data_MF+2)->len_nombre = 1;
    (input1->data_MF+2)->media = 3.6;
    (input1->data_MF+2)->varianza = 0.5521;
    // Declaracion MF4
    (input1->data_MF+3)->nombre = "3";
    (input1->data_MF+3)->len_nombre = 1;
    (input1->data_MF+3)->media = 4.8;
    (input1->data_MF+3)->varianza = 0.5521;
    // Declaracion MF4
    (input1->data_MF+4)->nombre = "4";
    (input1->data_MF+4)->len_nombre = 1;
    (input1->data_MF+4)->media = 6;
    (input1->data_MF+4)->varianza = 0.5521;
    // Declaracion MF4
    (input1->data_MF+5)->nombre = "1+";
    (input1->data_MF+5)->len_nombre = 2;
    (input1->data_MF+5)->media = 2.4;
    (input1->data_MF+5)->varianza = 0.5521;

    input1= fuzzy_logic->Input+2;
    input1->rango_min = 0;
    input1->rango_max = 1;
    input1->num_MF = 4;
    //Creacion memoria
    input1->data_MF = malloc(input1->num_MF*sizeof(MF));
    // Declaracion MF1
    input1->data_MF->nombre = "Nada";
    input1->data_MF->len_nombre = 4;
    input1->data_MF->media = 0;
    input1->data_MF->varianza = 0.1416;
    // Declaracion MF2
    (input1->data_MF+1)->nombre = "Bajo";
    (input1->data_MF+1)->len_nombre = 4;
    (input1->data_MF+1)->media = 0.3333;
    (input1->data_MF+1)->varianza =0.1416;
    // Declaracion MF3
    (input1->data_MF+2)->nombre = "Moderado";
    (input1->data_MF+2)->len_nombre = 8;
    (input1->data_MF+2)->media = 0.6667;
    (input1->data_MF+2)->varianza = 0.1416;
    // Declaracion MF4
    (input1->data_MF+3)->nombre = "Alto";
    (input1->data_MF+3)->len_nombre = 4;
    (input1->data_MF+3)->media = 1;
    (input1->data_MF+3)->varianza = 0.1416;

    data_fuzzy *output1= fuzzy_logic->Output;
    output1->rango_min = 0;
    output1->rango_max = 5;
    output1->num_MF = 5;
    //Creacion memoria
    output1->data_MF = malloc(output1->num_MF*sizeof(MF));
    // Declaracion MF1
    output1->data_MF->nombre = "Incomodo";
    output1->data_MF->len_nombre = 8;
    output1->data_MF->media = 0.166;
    output1->data_MF->varianza = 0.4111;
    // Declaracion MF2
    (output1->data_MF+1)->nombre = "Moderado_incomodo";
    (output1->data_MF+1)->len_nombre = 17;
    (output1->data_MF+1)->media = 1.2;
    (output1->data_MF+1)->varianza = 0.5521;
    // Declaracion MF3
    (output1->data_MF+2)->nombre = "Neutro";
    (output1->data_MF+2)->len_nombre = 6;
    (output1->data_MF+2)->media = 2.4;
    (output1->data_MF+2)->varianza = 0.5521;
    // Declaracion MF4
    (output1->data_MF+3)->nombre = "Moderado_comodo";
    (output1->data_MF+3)->len_nombre = 15;
    (output1->data_MF+3)->media = 3.6;
    (output1->data_MF+3)->varianza = 0.5521;
    // Declaracion MF5
    (output1->data_MF+4)->nombre = "Comodo";
    (output1->data_MF+4)->len_nombre = 6;
    (output1->data_MF+4)->media = 4.755;
    (output1->data_MF+4)->varianza = 0.5903;

    fill_plot(output1->data_MF,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+1,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+2,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+3,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+4,output1->rango_min,output1->rango_max);

    output1= fuzzy_logic->Output+1;
    output1->rango_min = 0;
    output1->rango_max = 1;
    output1->num_MF = 5;
    //Creacion memoria
    output1->data_MF = malloc(output1->num_MF*sizeof(MF));
    // Declaracion MF1
    output1->data_MF->nombre = "Baja";
    output1->data_MF->len_nombre = 4;
    output1->data_MF->media = 0;
    output1->data_MF->varianza = 0.1062;
    // Declaracion MF2
    (output1->data_MF+1)->nombre = "Mod_Baja";
    (output1->data_MF+1)->len_nombre = 8;
    (output1->data_MF+1)->media = 0.25;
    (output1->data_MF+1)->varianza = 0.1062;
    // Declaracion MF3
    (output1->data_MF+2)->nombre = "Media";
    (output1->data_MF+2)->len_nombre = 5;
    (output1->data_MF+2)->media = 0.5;
    (output1->data_MF+2)->varianza = 0.1062;
    // Declaracion MF4
    (output1->data_MF+3)->nombre = "Mod_Alta";
    (output1->data_MF+3)->len_nombre = 8;
    (output1->data_MF+3)->media =  0.75;
    (output1->data_MF+3)->varianza = 0.1062;
    // Declaracion MF5
    (output1->data_MF+4)->nombre = "Alta";
    (output1->data_MF+4)->len_nombre = 4;
    (output1->data_MF+4)->media = 1;
    (output1->data_MF+4)->varianza = 0.1062;

    fill_plot(output1->data_MF,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+1,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+2,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+3,output1->rango_min,output1->rango_max);
    fill_plot(output1->data_MF+4,output1->rango_min,output1->rango_max);

    fuzzy_logic->rules= malloc(fuzzy_logic->num_rules*(fuzzy_logic->num_input+fuzzy_logic->num_output)*sizeof(int));
    for(i = 0;i<fuzzy_logic->num_rules;i++){
        fgets(lectura,60,fp);
        for(j=0;j<fuzzy_logic->num_input;j++){
            fuzzy_logic->rules[j+i*(fuzzy_logic->num_input+fuzzy_logic->num_output)] =lectura[2*j]-48; 
        }
        for(j=0;j<fuzzy_logic->num_output;j++){
            fuzzy_logic->rules[(j+fuzzy_logic->num_input)+i*(fuzzy_logic->num_input+fuzzy_logic->num_output)] = lectura[2*fuzzy_logic->num_input+1+2*j]-48; 
        }
    }
    fclose (fp);      
}

int str_comp(char *string1, char *string2, int max){
    int i;
    if(*string1 == '\n' || *string2 == '\n'){
        return 0;
    }
    for(i=0;i<max;i++){
        if(*string1 == '\n' || *string2 == '\n'){
            return 1;
        }
        if(*string1!=*string2){
            return 0;
        }
        string1++;
        string2++;
    }
    return 1;
}

int str_num(char *string, int max){
    int i,j,save_i,save_j;
    int num=0, var=1;
    char *p= string;

    for(i=0;i<max;i++){
        if((*string)=='='){
            save_i = i;
            for(j=0;j<4;j++){
                string++;
                if((*string)=='\n'){
                    save_j = j;
                    j = 5;
                    i = max+1;
                }
            }
        }
        string++;
    }
    for(i=save_j+save_i-1;i>save_i;i--){
        num+= (p[i]-48)*var;
        var*=10;
    }
    return num;
}

