#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>



int * leituraFicheiroConfig(char * file){
	int * anum = (int *)malloc(sizeof(int) * 10);
	FILE * fp = fopen (file, "r");
	if(fp == NULL ){
		printf("ERRO AO ABRIR O FICHEIRO DE CONFIGURAÇÃO!\n");
		abort();
	}
	int num, i=0;
	char name[100], buff[500];
	while (fgets( buff, sizeof buff, fp) != NULL){
		if(sscanf(buff, "%[^=]=%d", name, &num) == 2)
			anum[i++]=num;
	}
	fclose(fp);
	return anum;
}
