//
//  simulador.c
//  ProjSO
//
//  Created by Alex Faria on 25/10/17.
//  Copyright © 2017 Alex Faria. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Header.h"
#include <stdio.h>


int tempoGuiche, tempoViagem, tempoSimulacao, capacidadeFila, capacidadeCarro, probDesisteGuiche, probDesisteFila;




//Leitura do ficheiro configSim
void readFile(){
    FILE* configFile;
    
    configFile = fopen("/Users/alex/Documents/GitHub/ProjetoSistemasOperativos20172018/ProjSO/ProjSO/ConfigSim.txt", "r"); //r = read
    
    if(configFile != NULL) {
        char linha[50];
        char * valor;
        while(fgets(linha, sizeof(linha), configFile) != NULL) {
            valor = strtok(linha, ":");
            
            if(strcmp(valor, "TEMPO_GUICHE") == 0) {
                valor = strtok(NULL, ":");
                tempoGuiche = atoi(valor);
            }
            if(strcmp(valor, "TEMPO_VIAGEM") == 0) {
                valor = strtok(NULL, ":");
                tempoViagem = atoi(valor);
            }
            if(strcmp(valor, "TEMPO_SIMULACAO") == 0) {
                valor = strtok(NULL, ":");
                tempoSimulacao = atoi(valor);
            }
            if(strcmp(valor, "CAPACIDADE_FILA") == 0) {
                valor = strtok(NULL, ":");
                capacidadeFila = atoi(valor);
            }
            if(strcmp(valor, "CAPACIDADE_CARRO") == 0) {
                valor = strtok(NULL, ":");
                capacidadeCarro = atoi(valor);
            }
            if(strcmp(valor, "PROB_DESISTE_GUICHE") == 0) {
                valor = strtok(NULL, ":");
                probDesisteGuiche = atoi(valor);
            }
            if(strcmp(valor, "PROB_DESISTE_FILA") == 0) {
                valor = strtok(NULL, ":");
                probDesisteFila = atoi(valor);
            }
            
        }
    }
    else {
        printf("ERRO AO ABRIR FICHEIRO\n");
    }
    
    fclose(configFile);
}



int main(){
    readFile();
    writeReport();
    
    printf("O tempo de espera no guiche e': %d\n", tempoGuiche);
    printf("A viagem tem a duracao: %d\n", tempoViagem);
    printf("O tempo da simulacao': %d\n", tempoSimulacao);
    printf("A capacidade da fila e' %d\n", capacidadeFila);
    printf("A capacidade de cada carro e': %d\n", capacidadeCarro);
    printf("A probablidade de desistencia da fila do Guiche' e': %d\n", probDesisteGuiche);
    printf("A probablidade de desistencia da fila e': %d\n", probDesisteFila);
    return 0;
    
}

