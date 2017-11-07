//
//  monitor.c
//  ProjSO
//
//  Created by Alex Faria on 25/10/17.
//  Copyright © 2017 Alex Faria. All rights reserved.
//

#include "Header.h"

void writeReport(){
    
    FILE* log;
    log = fopen("Resultado.txt", "w");
    
    if(log == NULL){
        printf("ERRO AO CRIAR FICHEIRO");
    }
    else {
        
        fprintf(log, "------ SIM BEGIN ------\n");
        fprintf(log, "TEMPO_GUICHE:%d\n", tempoGuiche);
        fprintf(log, "TEMPO_VIAGEM:%d\n", tempoViagem);
        fprintf(log, "TEMPO_SIMULACAO:%d\n", tempoSimulacao);
        fprintf(log, "CAPACIDADE_FILA:%d\n", capacidadeFila);
        fprintf(log, "CAPACIDADE_CARRO:%d\n", capacidadeCarro);
        fprintf(log, "PROB_DESISTE_GUICHE:%d\n", probDesisteGuiche);
        fprintf(log, "PROB_DESISTE_FILA:%d\n", probDesisteFila);
        fprintf(log, "----- SIM FINISHED -----\n");
    }
    printf("Relatorio criado com sucesso\n");
    fprintf(log, "---SIMULAÇÃO TERMINADA---\n\n");
    fclose(log);
}
