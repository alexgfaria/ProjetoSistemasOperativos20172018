//
//  Header.h
//  ProjSO
//
//  Created by Alex Faria on 25/10/17.
//  Copyright © 2017 Alex Faria. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#ifndef Header_h
#define Header_h



void writeReport(void);
void readFile(void);
int getTempoGuiche(void);
int getTempoViagem(void);
int getTempoSimulacao(void);
int getCapacidadeFila(void);
int getCapacidadeCarro(void);
int getProbDesisteGuiche(void);
int getProbDesisteFila(void);

extern int tempoGuiche, tempoViagem, tempoSimulacao, capacidadeFila, capacidadeCarro, probDesisteGuiche, probDesisteFila;


#endif /* Header_h */
