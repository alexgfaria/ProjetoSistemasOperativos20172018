//
//  simulador.c
//  ProjSO
//
//  Created by Alex Faria on 25/10/17.
//  Copyright © 2017 Alex Faria. All rights reserved.
//

#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "Header.h"
#include <stdio.h>

int INICIO_FINAL,
TEMPO_SIMULACAO,
TEMPO_MEDIO_CHEGADA_CLIENTES,
PROB_DESISTE_GUICHE,
PROB_DESISTE_FILA,
TEMPO_VIAGEM,
CAPACIDADE_CARROS,
NUMERO_CARROS;


int * lerConfig(char * ficheiro){
    
    int * a = (int *) malloc (sizeof (int) * 10);
    FILE * fp = fopen (ficheiro, "r"); //r - read
    if(fp == NULL){
        printf("ERRO - Impossivel abrir ficheiro\n");
        abort();
    }
    int num, i = 0;
    char name[100], buff[500];
    while (fgets(buff, sizeof buff, fp) != NULL){
        if(sscanf(buff, "%[^=]=%d", name, &num) == 2)
            a[i++]=num;
    }
    fclose(fp);
    return a;
}

int main(int argc, char *argv[]){
    
    
    
    //Upload do ficheiro de configuração
    int * configuracao = lerConfig("ConfigSim.txt");
    INICIO_FINAL                     = configuracao[0];
    TEMPO_SIMULACAO                  = configuracao[1];
    TEMPO_MEDIO_CHEGADA_CLIENTES     = configuracao[2];
    PROB_DESISTE_GUICHE              = configuracao[3];
    TEMPO_VIAGEM                     = configuracao[7];
    CAPACIDADE_CARROS                = configuracao[8];
    NUMERO_CARROS                    = configuracao[9];
    
    printf("INICIO_FINAL= %d\nTEMPO_SIMULACAO= %d\n", INICIO_FINAL,TEMPO_SIMULACAO);

    return 0;
}
