 
 
#ifndef READCSVFILE_H_
#define READCSVFILE_H_

//#pragma once
#include <stdio.h>

FILE *ini_LeerCsvData(char* csvFilePath);
void close_CSVData(FILE *csvFile);
int leer_valor(FILE *csvFile, float *vector,int tam);
float armar_numero(int signo,int pos_vec_int,int *vec_int, int pos_vec_dec,int *vec_dec);

 #endif