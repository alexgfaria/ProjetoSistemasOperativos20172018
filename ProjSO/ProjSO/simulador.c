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


/*int * lerConfig(char * ficheiro){
    
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
    
    //testa se o ficheiro foi fechado sem erro
    if(fclose(fp) == EOF) {
        printf("Error closing file\n");
    }
    
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

*/





int numCliente;
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
    printf("O tempo de espera no guiche e': %d\n",tempoGuiche);
    printf("A viagem tem a duracao: %d\n",tempoViagem);
    printf("O tempo da simulacao': %d\n",tempoSimulacao);
    printf("A capacidade da fila e' %d\n",capacidadeFila);
    printf("A capacidade de cada carro e': %d\n",capacidadeCarro);
    printf("A probablidade de desistencia da fila do Guiche' e': %d\n",probDesisteGuiche);
    printf("A probablidade de desistencia da fila e': %d\n",probDesisteFila);
    return 0;
    
}

