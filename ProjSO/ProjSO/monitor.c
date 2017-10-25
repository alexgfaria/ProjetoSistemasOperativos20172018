//
//  monitor.c
//  ProjSO
//
//  Created by Alex Faria on 25/10/17.
//  Copyright © 2017 Alex Faria. All rights reserved.
//

#include "Header.h"

/*//apontador para FILE
FILE *fp;
*/


void escrita(){
    FILE* logs;
    
    logs = fopen("Logs.txt", "a");
    
    if(logs == NULL){
        printf("ERRO AO CRIAR FICHEIRO");
    }
    else{
        fprintf(logs, "---SIMULAÇÃO INICIADA---\n");
        fprintf(logs, "TEMPO_ESPERA:30\n");
        fprintf(logs, "TEMPO_DISCO:40\n");
        fprintf(logs, "CAPACIDADE_FILA:20\n");
        fprintf(logs, "CAPACIDADE_SALA:10\n");
        fprintf(logs, "PROB_DESISTE_FILA:100\n");
        fprintf(logs, "---SIMULAÇÃO TERMINADA---\n");
    }
    
    fclose(logs);
}


int main()
{
    escrita();
    printf("Ficheiro Logs.txt criado com exito!\n");
    
    return 0;
    
}
