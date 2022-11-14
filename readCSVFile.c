 
#include "readCSVFile.h"
 
 
int leer_valor(FILE *csvFile, float *vector,int tam){
    char lectura; 
    int estado = 0;
    int signo = 1;
    int cont = 0;
    int flag = 1;

    int vec_int[11];
    int vec_dec[20];

    int pos_vec_int = 0;
    int pos_vec_dec = 0;
    int *pointer_int= vec_int;
    int *pointer_dec= vec_dec;

    lectura= fgetc(csvFile);

    while(flag){
        switch(estado){
            case 0:
                if(lectura>=48 && lectura <= 57){
                    signo = 1;
                    estado = 1;
                }else{
                    if(lectura == 45){
                        signo = -1;
                        lectura= fgetc(csvFile);
                        estado = 1;
                    }else{
                        lectura= fgetc(csvFile);
                    }
                }
            break;
            case 1:
                if(lectura>=48 && lectura <= 57){
                    if(pos_vec_int<11){
                        *pointer_int= (int)lectura-48;
                        pointer_int++;
                        pos_vec_int++;
                    }
                    lectura= fgetc(csvFile);
                }else{
                    estado = 2;
                }
            break;
            case 2:
                if(lectura != 44 && (lectura>=48 && lectura <= 57)){
                    if(pos_vec_dec<20){
                        *pointer_dec= (int)lectura-48;
                        pointer_dec++;
                        pos_vec_dec++;
                    }
                    lectura= fgetc(csvFile);
                }else{
                    if(lectura == 46){
                        lectura= fgetc(csvFile);
                    }
                    else{
                        *vector= armar_numero(signo,pos_vec_int,vec_int,pos_vec_dec,vec_dec);
                        vector++;
                        cont++;
                        if(cont>=tam){
                            flag = 0;
                        }else{
                            estado = 0;
                            pointer_int= vec_int;
                            pointer_dec= vec_dec;
                            pos_vec_int = 0;
                            pos_vec_dec = 0;
                        }
                    }
                }

            break;
        }
    }
}


float armar_numero(int signo, int pos_vec_int,int *vec_int, int pos_vec_dec,int *vec_dec){
    float numero=0;
    pos_vec_int--;
    pos_vec_dec--;
    int i,j;
    int *pointer = &(vec_int[pos_vec_int]);
    float multiplier = 1;

    for (i=0;i<pos_vec_int+1;i++){
        numero+= (float)(*pointer)*multiplier;
        pointer--;
        multiplier*=10;
    }

    multiplier = 10;
    for (i = 0;i<pos_vec_dec+1;i++){
        numero += (float)(*vec_dec)/multiplier;
        vec_dec++;
        multiplier*=10;
    }
    return numero*(float)signo;
}


FILE *ini_LeerCsvData(char* csvFilePath){
    FILE *fp;
	fp = fopen ( csvFilePath, "r" );        
    return fp;
}

void close_CSVData(FILE *csvFile){
    fclose ( csvFile );
}