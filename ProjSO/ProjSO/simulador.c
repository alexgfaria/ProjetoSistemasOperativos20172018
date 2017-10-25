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

int INICIO_FINAL, TEMPO_SIMULACAO, TEMPO_MEDIO_CHEGADA_CLIENTES, PROB_DESISTE_GUICHE,  PROB_DESISTE_FILA,  TEMPO_VIAGEM, CAPACIDADE_CARROS, NUMERO_CARROS;

int main(int argc, char *argv[]){
    
    
    
    //interpretacao do ficheiro de configuraçao
    int *conf = lerConfig(argv[1]);
    INICIO_FINAL                     = conf[0];
    TEMPO_SIMULACAO                  = conf[1];
    TEMPO_MEDIO_CHEGADA_CLIENTES     = conf[2];
    PROB_DESISTE_GUICHE              = conf[3];
    TEMPO_VIAGEM                     = conf[7];
    CAPACIDADE_CARROS                = conf[8];
    NUMERO_CARROS                    = conf[9];

    return 0;
}
