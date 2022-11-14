// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "readCSVFile.h"
#include "NN_Library.h"
#include <math.h>
#include "measure_time.h"

#define PORT 8080

#define TIEMPO_MUESTREO 0.0198
#define LONGITUD_BUFFER 228
#define PESO 53

#define M_EXO 5 // Masa del exoesqueleto
#define GRAVEDAD 9.776
#define PI 3.141592654

#define MAX_DATA_CSV 2088
#define FLAG_LEAPMOTION_CSV 1


//constantes para el calculo de la inercia
#define B_HOMBRE 0.285
#define C_HOMBRE 0.182
#define A_HOMBRE 0.233

//constantes para el calculo de la inercia
#define B_MUJER 0.241
#define C_MUJER 0.152
#define A_MUJER 0.206





/*//////////////////////////////////////////////////////                                   
                DECLARACION   FUNCIONES
*///////////////////////////////////////////////////////
float control_exoesqueleto(float *leapmotion, float Ts, float *data_config,float *in_red_neuronal ,float *out_red_neuronal, float *medida_tiempo);
void mtx_tras(float *matrix, int filas, int columnas, float *mtx_out);
float longitud_mano(float *leapmotion);
float norma_vec(float *vec1, float *vec2);
void calc_param_ini(float *m_2, float *mtx_k, int genero);
void calc_param(float L, float *r_CD, float *r_AC, float *I_2C, float m_2, int genero);
void socket_connection(int *sock,struct sockaddr_in *serv_addr,uint16_t port, int *client_fd);
void read_data_socket(int valread, char *buffer, float *num_in_python);
int vec2str(char *buffer_send, float *vec, int tam);
int cond_time_leap(float tiempo);

int main(int argc, char const* argv[])
{
    FILE* fichero;
    fichero = fopen("datos_control_total_Raspberry.txt", "wt");


    /// Inicializaciones
    char *filename="datos_leapmotion.csv";
    FILE * csv;
    int cont_data_read=0;

    float medida_tiempo=0,medida_tiempo_NN=0;

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
    float vec_out_socket[7];


    float ambiente_rv, disposicion,intensidad_torque,torque_control,  torque_final;

    float datos_entrada[LONGITUD_BUFFER];

    if(FLAG_LEAPMOTION_CSV){
        csv = ini_LeerCsvData(filename);           // Leer datos csv
        if(csv==NULL){
            printf("Error leyendo\n");
            return 1;
        }else{
            printf("CSV leido\n");
        }

    }

    ambiente_rv= 2.5;
    disposicion= 1;
    intensidad_torque= 0.9;

    float medida_red_neuronal[4];
    float entrada_red_neuronal[6];

    // COMIENZA CODIGO
    if(1){

        flag_continue = 1;
        while(flag_continue){
            // Lee leapmotion
            if(FLAG_LEAPMOTION_CSV){

                //Se simula la espera del leapmotion
                //while(cond_time_leap(TIEMPO_MUESTREO)==0);
                leer_valor(csv, datos_entrada,LONGITUD_BUFFER);
                cont_data_read++;
            }

            // Hace  iteracion control

            start();
            torque_control= control_exoesqueleto(datos_entrada, TIEMPO_MUESTREO,  num_in_python,entrada_red_neuronal ,medida_red_neuronal, &medida_tiempo_NN);
            stop();
            
            medida_tiempo= time_diff();

            printf("%f %f\n",medida_tiempo,medida_tiempo_NN);

            fprintf(fichero,"%0.20f %0.20f ",torque_control,medida_tiempo);

            for(int i = 0;i<6;i++){
                fprintf(fichero,"%0.20f ",entrada_red_neuronal[i]);
            }
            for(int i = 0;i<4;i++){
                fprintf(fichero,"%0.20f ",medida_red_neuronal[i]);
            }

            fprintf(fichero,"%0.20f\n", medida_tiempo_NN);


            if(cont_data_read<MAX_DATA_CSV){
                vec_out_socket[6]=9.8;
            }else{
                vec_out_socket[6]=-9.8;
                flag_continue=0;
            }
        }

        if(FLAG_LEAPMOTION_CSV){
            close_CSVData(csv);
        }
    }

    fclose(fichero);

    return 0;
}




/*
/  CONTROL_EXOQUELETO:
/   
/  -Descripcion: 
/   
/  En esta funcion se implementara el codigo general 
/  del control adaptativo para una lectura del 
/  leap motion.
/  
/  -Entradas:
/        + leapmotion: vector float de 228 espacios.   
/        + genero: hombre=> 0, mujer=> 1.
*/
// id, ambiente_rv, disposicion, intensidad_torque, torque_control, torque_final, parar
float control_exoesqueleto(float *leapmotion, float Ts, float *data_config,float *in_red_neuronal, float *out_red_neuronal, float *medida_tiempo){
    // Inizializacion variables
    static int cont = 0, flag = 1;

    static float R_arm_act[9]= {0,0,0,0,0,0,0,0,0};
    static float R_arm_menos_1[9]= {0,0,0,0,0,0,0,0,0};
    static float R_arm_menos_2[9]= {0,0,0,0,0,0,0,0,0};

    static float R_hand_act[9]= {0,0,0,0,0,0,0,0,0};
    static float R_hand_menos_1[9]= {0,0,0,0,0,0,0,0,0};
    static float R_hand_menos_2[9]= {0,0,0,0,0,0,0,0,0};

    static float R_wrist_act[9]= {0,0,0,0,0,0,0,0,0};
    static float R_wrist_menos_1[9]= {0,0,0,0,0,0,0,0,0};
    static float R_wrist_menos_2[9]= {0,0,0,0,0,0,0,0,0};

    static float Alpha_WL_act=0;
    static float Beta_WL_act=0;
    static float Gamma_WL_act=0;

    static float Alpha_WL_menos_1=0;
    static float Beta_WL_menos_1=0;
    static float Gamma_WL_menos_1=0;

    static float Alpha_WL_menos_2=0;
    static float Beta_WL_menos_2=0;
    static float Gamma_WL_menos_2=0;

    static float AlphaP_WL_menos_1[3]= {0,0,0};
    static float BetaP_WL_menos_1[3]= {0,0,0};
    static float Rx_WL_menos_1[9]={1,0,0,0,0,0,0,0,0};
    static float Ry_WL_menos_1[9]= {0,0,0,0,1,0,0,0,0};

    static float vxyz_W[3]= {0,0,0};
    static float vxyz_W_menos_1[3]= {0,0,0};
    static float vxyz_W_menos_2[3]= {0,0,0};

    static float m_2;
    static float K[4];

    static float Theta_M_act;
    static float Theta_M_menos_1=0;
    static float Theta_M_menos_2=0;

    static float vxyz_We_act[3];
    static float vxyz_We_menos_1[3];
    static float vxyz_We_menos_2[3];

    static NN_param NN1;
    static NN_param NN2;
    static NN_param NN3;
    static NN_param_m NN4;

    static time_struct var_time;


    float L;
    float I_2C[9]= {0,0,0,0,0,0,0,0,0};
    float r_CD;
    float r_AC;

    // Variebles intermedias
    float R_arm_act_t[9];
    float Rx_WL_menos_1_T[9];
    float axyz_W[3];
    float axyz_M[3];
    float m_M[2], M_alpha, M_beta;
    float ThetaP_M[3]= {0,0,0};
    float vxyz_W_aux[3];
    float T_adap_M;
    float datos_in[6];
    float datos_in_norm[6];
    float datos_out_norm[4];
    float datos_out[4];
    // Codigo

    /* Recordar filtro digital*/

    // Determinar longitud mano
    L = longitud_mano(leapmotion);
    if(flag){
        flag = 0;

        // emocion, espasticidad, dolor, realidad_virtual, genero, stop
        calc_param_ini(&m_2, K, data_config[4]);
        inicializacion_parametros_redes(&NN1,&NN2,&NN3,&NN4);
    }
    // Estimacion de r_CD, r_AC, I_2C
    calc_param(L, &r_CD, &r_AC, I_2C, m_2, data_config[4]);


    // Lectura de matriz de rotacion de la mano y el brazo apartir de datos del leapmotion
    fill_matrix(R_arm_act,&(leapmotion[203]),9);
    fill_matrix(R_hand_act,&(leapmotion[1]),9);

    //MATRIZ DE ROTACIÓN DE LA ARTICULACIÓN DE LA MUÑECA A PARTIR DEL LAS
    //MATRICES DE ROTACIÓN DE LAS FALANGES DISTAL Y PROXIMAL
    mul_mtx(R_hand_act,3, 3,R_arm_act, 3, 3, R_wrist_act);

    // Angulos muñeca
    Alpha_WL_act= atan2(-R_wrist_act[7],R_wrist_act[8]);
    Beta_WL_act= atan2(R_wrist_act[6],sqrt(R_wrist_act[0]*R_wrist_act[0]+R_wrist_act[3]*R_wrist_act[3]));
    Gamma_WL_act= atan2(-R_wrist_act[3],R_wrist_act[0]);

    // Velocidad angular biomecánica
    AlphaP_WL_menos_1[0]= (Alpha_WL_act-Alpha_WL_menos_2)/(Ts*2);
    BetaP_WL_menos_1[1]= (Beta_WL_act-Beta_WL_menos_2)/(Ts*2);

    //
    Rx_WL_menos_1[4] = cos(Alpha_WL_menos_1);
    Rx_WL_menos_1[5] = sin(Alpha_WL_menos_1); 
    Rx_WL_menos_1[8] = cos(Alpha_WL_menos_1);
    Rx_WL_menos_1[7] = -sin(Alpha_WL_menos_1);
    //Ry_WL_menos_1[0] = cos(Beta_WL_menos_1);
    //Ry_WL_menos_1[6] = sin(Beta_WL_menos_1); 
    //Ry_WL_menos_1[8] = cos(Beta_WL_menos_1);
    //Ry_WL_menos_1[2] = -sin(Beta_WL_menos_1); 

    //Calculo de la velocidad angular articular de la muñeca
    mtx_tras(Rx_WL_menos_1, 3,3, Rx_WL_menos_1_T);
    mul_mtx(Rx_WL_menos_1_T,3, 3,BetaP_WL_menos_1, 3, 1,vxyz_W_aux);
    vec_sum(vxyz_W_aux,AlphaP_WL_menos_1, 3,vxyz_W);

    // Aceleracion biomecanica
    axyz_W[0] = (vxyz_W[0]-vxyz_W_menos_2[0])/(2*Ts);
    axyz_W[1] = (vxyz_W[1]-vxyz_W_menos_2[1])/(2*Ts);
    axyz_W[2] = (vxyz_W[2]-vxyz_W_menos_2[2])/(2*Ts);

    //Cálculo de la dinámica inversa de la muñeca
    m_M[0]= I_2C[8]*axyz_W[0]-K[0]*Alpha_WL_act+K[1]*Beta_WL_act;
    m_M[1]= I_2C[0]*axyz_W[1]-K[2]*Alpha_WL_act+K[3]*Beta_WL_act+ m_2*GRAVEDAD*r_CD/100;

    M_alpha = axyz_W[0]*(m_2*r_AC*(r_AC+2*(r_CD/100)*cos(Beta_WL_act))+I_2C[8]*sin(Beta_WL_act)*sin(Beta_WL_act)+I_2C[0]*cos(Beta_WL_act)*cos(Beta_WL_act))+vxyz_W[0]*vxyz_W[1]*(2*(I_2C[8]-I_2C[0])*cos(Beta_WL_act)*sin(Beta_WL_act)-2*m_2*r_AC*(r_CD/100)*sin(Beta_WL_act))-Alpha_WL_act*K[0]+Beta_WL_act*K[1];
    M_beta = I_2C[4]*axyz_W[1]-vxyz_W[0]*vxyz_W[0]*((I_2C[8]-I_2C[0])*cos(Beta_WL_act)*sin(Beta_WL_act)-m_2*r_AC*(r_CD/100)*sin(Beta_WL_act))-Alpha_WL_act*K[1]+Beta_WL_act*K[3]+m_2*GRAVEDAD*(r_CD/100)*cos(Beta_WL_act);


    // Cinematica inversa robot
    //Theta_M_act = 180*atan(-R_wrist_act[5]/R_wrist_act[2])/PI;
    Theta_M_act = atan2(-R_wrist_act[5],R_wrist_act[2]);
    // Velocidad angular del exo
    ThetaP_M[0]= (Theta_M_act-Theta_M_menos_2)/(2*Ts);
    vec_sum(ThetaP_M,vxyz_W_aux, 3,vxyz_We_act);

    // Aceleracion angular del exo
    axyz_M[0] = (vxyz_We_act[0]-vxyz_We_menos_2[0])/(2*Ts);
    axyz_M[1] = (vxyz_We_act[1]-vxyz_We_menos_2[1])/(2*Ts);
    axyz_M[2] = (vxyz_We_act[2]-vxyz_We_menos_2[2])/(2*Ts);

    T_adap_M = (M_EXO-m_M[0])*((L*L)*axyz_M[0]) + (M_EXO*GRAVEDAD)*L*cos(Theta_M_act);
    if(cont>3){
        
        start_v2(&var_time);

        datos_in[0]= M_alpha;
        datos_in[1]= M_beta;
        datos_in[2]= Alpha_WL_act;
        datos_in[3]= Beta_WL_act;
        datos_in[4]= axyz_W[0];
        datos_in[5]= axyz_W[1];

        normalizar( datos_in, datos_in_norm);
        datos_out_norm[0] = NN_function(&NN1,datos_in_norm);
        datos_out_norm[1] = NN_function(&NN2,datos_in_norm);
        datos_out_norm[2] = NN_function(&NN3,datos_in_norm);
        datos_out_norm[3] = NN_function_m(&NN4,datos_in_norm);


        normalizar_inversa(datos_out_norm, datos_out);

        stop_v2(&var_time);

        *medida_tiempo= time_diff_v2(&var_time);

        K[0] = datos_out[0];
        K[1] = datos_out[1];
        K[2] = -0.16;
        K[3] = datos_out[2];
        m_2 = datos_out[3];

    }


    // Actualizacion variables
    cont++;
        //fill_matrix(dst, scr, 9);
    fill_matrix(R_arm_menos_2, R_arm_menos_1, 9);
    fill_matrix(R_arm_menos_1, R_arm_act, 9);

    fill_matrix(R_hand_menos_2, R_hand_menos_1, 9);
    fill_matrix(R_hand_menos_1, R_hand_act, 9);

    fill_matrix(R_wrist_menos_2, R_wrist_menos_1, 9);
    fill_matrix(R_wrist_menos_1, R_wrist_act, 9);

    fill_matrix(vxyz_W_menos_2, vxyz_W_menos_1, 3);
    fill_matrix(vxyz_W_menos_1, vxyz_W, 3);

    Alpha_WL_menos_2 = Alpha_WL_menos_1;
    Alpha_WL_menos_1 = Alpha_WL_act;
    Beta_WL_menos_2 = Beta_WL_menos_1;
    Beta_WL_menos_1 = Beta_WL_act;
    Gamma_WL_menos_2 = Gamma_WL_menos_1;
    Gamma_WL_menos_1 = Gamma_WL_act;

    Theta_M_menos_2 = Theta_M_menos_1;
    Theta_M_menos_1 = Theta_M_act;

    fill_matrix(vxyz_We_menos_2, vxyz_We_menos_1, 3);
    fill_matrix(vxyz_We_menos_1, vxyz_We_act, 3);


    fill_matrix(out_red_neuronal, datos_out , 4);
    fill_matrix(in_red_neuronal, datos_in , 6);

    return T_adap_M;


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

/// FUNCIONES CONTROL

float longitud_mano(float *leapmotion){

    float S_meta[3]= {leapmotion[224],leapmotion[225],leapmotion[226]};
    float End_M[3]= {leapmotion[67],leapmotion[68],leapmotion[69]};
    float End_P[3]= {leapmotion[76],leapmotion[77],leapmotion[78]};
    float End_Mi[3]= {leapmotion[58],leapmotion[59],leapmotion[60]};
    float End_D[3]= {leapmotion[49],leapmotion[50],leapmotion[51]};

    float V_hand= norma_vec(S_meta,End_M);
    float V_prox= norma_vec(End_M,End_P);
    float V_med= norma_vec(End_P,End_Mi);
    float V_dist = norma_vec(End_Mi,End_D);

    return (V_hand+V_prox+V_med+V_dist)/10;
}

float norma_vec(float *vec1, float *vec2){
    int i;
    float norma = 0;
    for(i=0;i<3;i++){
        norma+= (vec1[i]-vec2[i])*(vec1[i]-vec2[i]);
    }
    return sqrt(norma);
}

void calc_param_ini(float *m_2, float *mtx_k, int genero){
    if(genero==0){
        //masa de la mano en kg
        *m_2=0.0061*PESO; 
        //matriz de rigidez pasiva de la muñeca en Nm/kg
        mtx_k[0]= 1.42;
        mtx_k[1]= -0.19;
        mtx_k[2]= -0.19;
        mtx_k[3]= 1.85;
    }else{
        //masa de la mano en kg
        *m_2=0.0056*PESO; 
        //matriz de rigidez pasiva de la muñeca en Nm/kg
        mtx_k[0]= 0.96;
        mtx_k[1]= -0.16;
        mtx_k[2]= -0.16;
        mtx_k[3]= 1.49;
    }
}

void calc_param(float L, float *r_CD, float *r_AC, float *I_2C, float m_2, int genero){
    if(genero==0){
        //distancia de la muñeca al centro de masa de la mano en cm 
        *r_CD=0.3691*L-0.2; 
        I_2C[0]= m_2*((A_HOMBRE*L/100)*(A_HOMBRE*L/100)+((*r_CD)/100)*((*r_CD)/100));
        I_2C[4]= m_2*((B_HOMBRE*L/100)*(B_HOMBRE*L/100)+((*r_CD)/100)*((*r_CD)/100));
        I_2C[8]= m_2*((C_HOMBRE*L/100)*(C_HOMBRE*L/100));

    }else{
        I_2C[0]= m_2*((A_MUJER*L/100)*(A_MUJER*L/100)+((*r_CD)/100)*((*r_CD)/100));
        I_2C[4]= m_2*((B_MUJER*L/100)*(B_MUJER*L/100)+((*r_CD)/100)*((*r_CD)/100));
        I_2C[8]= m_2*((C_MUJER*L/100)*(C_MUJER*L/100));
        *r_CD=0.3502*L-0.2; 
    }
    *r_AC=0.004; 

}






void mtx_tras(float *matrix, int filas, int columnas, float *mtx_out){
    //i,j => j,i
    int i = 0,j = 0;
    float aux;
    for(i=0;i<filas;i++){
        for(j=0;j<columnas;j++){
            mtx_out[j*filas+i]= matrix[i*columnas+j];
        }
    }
}
