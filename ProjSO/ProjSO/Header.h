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




#endif /* Header_h */
