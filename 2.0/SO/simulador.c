#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "unix.h"
#include "util.h"

//variáveis
int num_cliente, num_carro, num_emb, carro, carros_em_servico;

time_t start;

struct sockaddr_un serv_addr;
int sockfd, servlen;

//var de configuração
int INICIO_FINAL, TEMPO_SIMULACAO, TEMPO_MEDIO_CHEGADA_CLIENTES, PROB_DESISTE_ESPERA,  PROB_DESISTE_MEDO,  TEMPO_VIAGEM, CAPACIDADE_CARROS, NUMERO_CARROS;

int corre = 0, pausa = 0, controlo_pausa;




void * tarefa_cliente(void * ptr){
	pthread_detach(pthread_self());
	int car, g;
	time_t temp_i;
	char buffer_c[256];
	int tem_esp = rand()%10; // entre 0 e 10 minutos de espera



	int num_cli = num_cliente++;
	printf("O cliente %d chegou a fila de espera.\n", num_cli);
	sprintf(buffer_c, "%d CHEGADA %d 0\n", (int)time(0), num_cli);
	send(sockfd,buffer_c,sizeof(buffer_c),0);

	temp_i = time(0);

	// ---------- DESISTÊNCIAS ----------------------
	//TEMPO
	if((int)(time(0) - temp_i) > tem_esp)
	{
		g = rand()%100;
		if(g <= PROB_DESISTE_ESPERA)
		{
			if(tem_esp == 1)
				printf("O cliente %d desistiu ao fim de %d minuto a espera.\n", num_cli, tem_esp);
			else
				printf("O cliente %d desistiu ao fim de %d minutos a espera.\n", num_cli, tem_esp);
			sprintf(buffer_c, "%d DESISTE_FILA %d 0\n", (int)time(0), num_cli);
			send(sockfd,buffer_c,sizeof(buffer_c),0);
			return NULL;
		}
	}



	//MEDO
	g = rand()%100;
	if(g <= PROB_DESISTE_MEDO){
		printf("O cliente %d entrou na zona de embarque.\n",num_cli);
		printf("O cliente %d desistiu porque ficou com medo.\n", num_cli);
		sprintf(buffer_c, "%d DESISTE_EMBARQUE\n", (int)time(0));
		send(sockfd,buffer_c,sizeof(buffer_c),0);
		return NULL;
	}

	num_emb++;

	if ( num_emb < CAPACIDADE_CARROS)


	printf("O cliente %d entrou na zona de embarque.\n",num_cli);
	sprintf(buffer_c, "%d ENTRA_EMBARQUE %d 0\n", (int)(time(0)-temp_i), num_cli);
	send(sockfd,buffer_c,sizeof(buffer_c),0);

	car = carro;
	printf("O cliente %d entrou no carro %d.\n", num_cli, car);
	sprintf(buffer_c, "%d EMBARQUE %d %d\n", (int)time(0), num_cli, car);
	send(sockfd,buffer_c,sizeof(buffer_c),0);


	printf("O cliente %d saiu do carro %d.\n", num_cli, car);
	sprintf(buffer_c, "%d DESEMBARQUE %d %d\n", (int)time(0), num_cli, car);
	send(sockfd,buffer_c,sizeof(buffer_c),0);


	return NULL;
}




void * tarefa_carro(void * ptr){

	pthread_detach(pthread_self());
	int id = num_carro++;
	char buffer_c[256];
	time_t hora_partida, hora_i;
	char variavel[20] = "carro";



	void inicia_viagem(){
		int i;
		for(i = 0; i < CAPACIDADE_CARROS; i++)
		printf("O carro %d iniciou viagem.\n", id);
		sprintf(buffer_c, "%d ARRANQUE %d\n", (int)time(0), id);
		send(sockfd,buffer_c,sizeof(buffer_c),0);
		hora_partida = time(0);
		while((int)(time(0) - hora_partida) < TEMPO_VIAGEM);
		printf("O carro %d terminou a viagem e chegou a zona de desembarque.\n", id);
		sprintf(buffer_c, "%d FINAL_VIAGEM %d\n", (int)time(0), id);
		send(sockfd,buffer_c,sizeof(buffer_c),0);
	}


	while(1){
		printf("O carro %d ficou em lista de espera.\n", id);
		sprintf(buffer_c, "%d ESPERA %d\n", (int)time(0), id);
		send(sockfd,buffer_c,sizeof(buffer_c),0);
		printf("O carro %d chegou a zona de embarque.\n", id);
		sprintf(buffer_c, "%d DISPONIVEL %d\n", (int)time(0), id);
		send(sockfd,buffer_c,sizeof(buffer_c),0);
		int i;
		num_emb = 0;

		if(i == 0)
		for(i = 0; i < CAPACIDADE_CARROS; i++)
		carro = id;
		hora_i = time(0);

		if(time(0) > start + TEMPO_SIMULACAO){
			usleep(100000);
			if(num_emb == 0){
				printf("O parque ja fechou e nao tem clientes em espera.\n");
				printf("O carro %d vai ser arrumado.\n", id);
				carros_em_servico--;
				return NULL;
			}

			for(i = 0; i < CAPACIDADE_CARROS; i++)
			printf("O carro %d iniciou viagem.\n", id);
			sprintf(buffer_c, "%d ARRANQUE %d\n", (int)time(0), id);
			send(sockfd,buffer_c,sizeof(buffer_c),0);


			hora_partida = time(0);
			while((int)(time(0) - hora_partida) < TEMPO_VIAGEM);
			printf("O carro %d terminou a viagem e chegou a zona de desembarque.\n", id);
			sprintf(buffer_c, "%d FINAL_VIAGEM %d\n", (int)time(0), id);
			send(sockfd,buffer_c,sizeof(buffer_c),0);
		}
		else{
			while(num_emb < CAPACIDADE_CARROS && time(0) < start + TEMPO_SIMULACAO);
			if(num_emb == CAPACIDADE_CARROS){
					usleep(100000);
					inicia_viagem();
			}
			else{
					usleep(100000);

					if(num_emb == 0){
						printf("O parque ja fechou e nao tem clientes em espera.\n");
						printf("O carro %d vai ser arrumado.\n", id);
						carros_em_servico--;
						return NULL;
					}
					inicia_viagem();
			}
		}
	}
	return NULL;
}

// COMUNICAÇÃO COM MONITOR
void * monitor_link(void * arg){
	struct sockaddr_un cli_addr;
	int done, n, id;

	int sockfd = * ((int *) arg), clilen = sizeof(cli_addr);

	char buffer[256];

	//Ciclo que fica a espera dos pedidos dos Monitor, para lhe dar resposta adequada
	while(1){
		done=0;
		if((n=recv(sockfd, buffer, sizeof(buffer), 0)) <= 0){
			if(n < 0)
				perror("recv error");
			done=1;
		}
		buffer[n]='\0';

		if(!strcmp(buffer, "termina\n")){
			corre=0;
			exit(1);
		}
		else{
			if(!strcmp(buffer, "inicio\n"))
				corre = 1;
			if(!strcmp(buffer, "pausa\n"))
				pausa = 1;
			if(!strcmp(buffer, "retomar\n"))
				pausa = 0;
		}
	}
	return NULL;
}

int main(int argc, char *argv[]){

	srand(time(NULL));

	num_cliente = 1;
	num_carro = 1;
	carro = NUMERO_CARROS;
	num_emb = 0;

	if(argc<2){
		printf("ERRO: FICHEIRO DE CONFIGURAÇÃO EM FALTA");
		return 1;
	}
	else {

		//interpretacao do ficheiro de configuraçao
		int * conf = leitura_configuracao(argv[1]);
		INICIO_FINAL = conf[0];
		TEMPO_SIMULACAO = conf[1];
		TEMPO_MEDIO_CHEGADA_CLIENTES = conf[2];
		PROB_DESISTE_ESPERA	= conf[3];
		TEMPO_VIAGEM = conf[7];
		CAPACIDADE_CARROS = conf[8];
		NUMERO_CARROS	= conf[9];




		//criacao do socket e ligação
		if((sockfd=socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
			perror("Simulador: cant open socket stream");
		serv_addr.sun_family=AF_UNIX;
		strcpy(serv_addr.sun_path, UNIXSTR_PATH);
		servlen=strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);
		if(connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
			perror("connect error");

		//Criacao da tarefa que ira tratar dos pedidos enviados pelo Monitor
		pthread_t thread;
		pthread_create(&thread, NULL, &monitor_link, &sockfd);

		while(!corre);

		int i, r, h;
		char buffer[256];
		carros_em_servico = NUMERO_CARROS;


		start = time(0);
		sprintf(buffer, "%d INICIO\n", (int)start);
		send(sockfd,buffer,sizeof(buffer),0);

		TEMPO_SIMULACAO *= 60;

		for(i = 0; i < NUMERO_CARROS; i++){
			pthread_create(&thread, NULL, &tarefa_carro, &sockfd);
		}

		while((int)(time(0) - start) < TEMPO_SIMULACAO){
				pthread_create(&thread, NULL, &tarefa_cliente, &sockfd);

			h = rand()%3;
			h = h * 100000;
				h = 0 - h;
			usleep((TEMPO_MEDIO_CHEGADA_CLIENTES*1000000) + h);

			if(pausa){
				controlo_pausa = 1;
			}
			while(pausa);
			if(controlo_pausa){
				controlo_pausa = 0;
			}

		}

		while(carros_em_servico>0);
		printf("\nO parque já fechou e não existem mais clientes em espera.\n");
		printf("\nSimulação terminada.\n");
		sprintf(buffer,"%d FIM \n",(int) time(0));
		send(sockfd,buffer,sizeof(buffer),0);

		while(corre);
		close(sockfd);

	}
	return 0;
}
