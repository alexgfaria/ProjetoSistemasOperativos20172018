#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>



int * leitura_configuracao(char * file){
	int * a = (int *)malloc(sizeof(int) * 10);
	FILE * fp = fopen (file, "r");
	if(fp == NULL ){
		printf("ERRO: FICHEIRO DE CONFIGURAÇÃO.\n");
		abort();
	}
	int num, i = 0;
	char name[100], buff[500];
	while (fgets( buff, sizeof buff, fp) != NULL) {
		if(sscanf(buff, "%[^=]=%d", name, &num) == 2)
			a[i++]=num;
	}
	fclose(fp);
	return a;
}
