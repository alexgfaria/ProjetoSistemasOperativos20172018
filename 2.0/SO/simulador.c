#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "unix.h"
#include "util.h"


//sem_t guicheVIP;   NÃO UTILIZADO
sem_t guiche;
sem_t embarqueCarro;
sem_t carroControlo;
sem_t desembarqueCarro;
sem_t semClientes;

pthread_mutex_t mutex;

int numCliente, numCarro, numEmbarque, filaVIP, carro, carrosEmUso;

time_t start;

struct sockaddr_un serv_addr;
int sockfd, servlen;

//Variáveis da configuração
int INICIO_FINAL,
		TEMPO_SIMULACAO,
		TEMPO_MEDIO_CHEGADA_CLIENTES,
		PROB_DESISTE_ESPERA,
		PROB_DESISTE_ESPERA_VIP,
		PROB_DESISTE_MEDO,
		PROB_DESISTE_MEDO_VIP,
		TEMPO_VIAGEM,
		CAPACIDADE_CARROS,
		NUMERO_CARROS;

int emExecucao = 0,
		pausa 		 = 0,
		pauseControlo;






// ---------------------------------------- CLIENTE PRIORITÁRIO ---------------------------------------------------
void * clienteVIP(void * ptr){
	pthread_detach(pthread_self()); //cria espaço
	int carro2v, num;
	time_t temp_i;
	int tempoDeEspera = rand()%10; // entre 0 e 10 minutos de espera
	char buffer_c[256];

	//...Secção crítica...
	pthread_mutex_lock(&mutex);
	int numClienteVIP = numCliente++;
	filaVIP++;
	printf("O cliente %d chegou à fila de espera.\n", numClienteVIP);
	//1 indica que é VIP
	sprintf(buffer_c, "%d CHEGADA %d 1\n", (int)time(0), numClienteVIP);
	send(sockfd,buffer_c,sizeof(buffer_c),0);
	temp_i = time(0);
	pthread_mutex_unlock(&mutex);
	//...fim da secção crítica...
	sem_wait(&semClientes);


	//...secção crítica...
	pthread_mutex_lock(&mutex);


		// ---------- Desiste por tempo à espera --------------------
		if((int)(time(0) - temp_i) > tempoDeEspera){
			num = rand()%100;
			if(num <= PROB_DESISTE_ESPERA_VIP){
				if(tempoDeEspera == 1) printf("O cliente %d desistiu ao fim de %d minutos à espera.\n", numClienteVIP, tempoDeEspera);
			else printf("O cliente %d desistiu ao fim de %d minutos à espera.\n", numClienteVIP, tempoDeEspera);
			sprintf(buffer_c, "%d DESISTE_FILA %d 1\n", (int)time(0), numClienteVIP);
			send(sockfd, buffer_c, sizeof(buffer_c), 0);
			filaVIP--;
			sem_post(&semClientes); //desbloqueia o semáforo dos clientes (inicializado no main())
			pthread_mutex_unlock(&mutex);
			//...fim da secção crítica


			return NULL;
		}
	}

	filaVIP--; //retira cliente da fila VIP




	//------ DESISTE MEDO ----------
	num = rand()%100;
	if(num <= PROB_DESISTE_MEDO_VIP){
		printf("O cliente %d entrou na zona de embarque.\n",numClienteVIP);
		printf("O cliente %d desistiu porque ficou com medo.\n", numClienteVIP);
		sprintf(buffer_c, "%d DESISTE_EMBARQUE\n", (int)time(0));
		send(sockfd,buffer_c,sizeof(buffer_c),0);
		sem_post(&semClientes); //desbloqueia semáforo dos clientes
		if (filaVIP == 0) sem_post(&guiche); //se não existir alguém na fila VIP então desbloqueia o semáforo guiche
		pthread_mutex_unlock(&mutex);
		return NULL;
	}

	numEmbarque++; //se não desiste então aumento o número de clientes embarcados

	printf("O cliente %d entrou na zona de embarque.\n", numClienteVIP);
	sprintf(buffer_c, "%d ENTRA_EMBARQUE %d 1\n", (int)(time(0)-temp_i), numClienteVIP);
	send(sockfd, buffer_c, sizeof(buffer_c), 0);

	if (filaVIP == 0 && numEmbarque < CAPACIDADE_CARROS)
		sem_post(&guiche); //desbloqueia o semaforo guiche
    pthread_mutex_unlock(&mutex); //desbloqueia trinco


	sem_wait(&embarqueCarro); //bloqueia semáforo
	pthread_mutex_lock(&mutex); //bloqueia trinco

	carro2v = carro;
	printf("O cliente %d entrou no carro %d.\n", numClienteVIP, carro2v);
	sprintf(buffer_c, "%d EMBARQUE %d %d\n", (int)time(0), numClienteVIP, carro2v);
	send(sockfd, buffer_c, sizeof(buffer_c), 0);
	pthread_mutex_unlock(&mutex);//desbloqueia trinco
	sem_wait(&desembarqueCarro);//bloqueia semáforo
	pthread_mutex_lock(&mutex);//bloqueia trinco
	printf("O cliente %d saiu do carro %d.\n", numClienteVIP, carro2v);
	sprintf(buffer_c, "%d DESEMBARQUE %d %d\n", (int)time(0), numClienteVIP, carro2v);
	send(sockfd, buffer_c, sizeof(buffer_c), 0);
	sem_post(&embarqueCarro);//desbloqueia semáforo embarqueCarro
	pthread_mutex_unlock(&mutex);//desbloqueia trinco

	return NULL;
}










// ------------------------------------- CLIENTE NORMAL ----------------------------------------------
void * clienteNormal(void *ptr){
	pthread_detach(pthread_self());
	int carro2v, num;
	time_t temp_i;
	char buffer_c[256];
	int tempoDeEspera = rand()%10; // entre 0 e 10 minutos de espera


	//...secção crítica....
	pthread_mutex_lock(&mutex); //lock trinco

	int numCli = numCliente++;
	printf("O cliente %d chegou a fila de espera.\n", numCli);
	sprintf(buffer_c, "%d CHEGADA %d 0\n", (int)time(0), numCli);
	send(sockfd,buffer_c,sizeof(buffer_c),0);

	temp_i = time(0);

	pthread_mutex_unlock(&mutex);//unlock trinco
	//...fim da secção crítica...

	sem_wait(&guiche);//lock semáforo
	pthread_mutex_lock(&mutex);//lock trinco


	//--------------DESISTE POR TEMPO---------------------
	if((int)(time(0) - temp_i) > tempoDeEspera){
		num = rand()%100;
		if(num <= PROB_DESISTE_ESPERA){
			if(tempoDeEspera == 1) printf("O cliente %d desistiu ao fim de %d minuto a espera.\n", numCli, tempoDeEspera);
			else printf("O cliente %d desistiu ao fim de %d minutos a espera.\n", numCli, tempoDeEspera);
			sprintf(buffer_c, "%d DESISTE_FILA %d 0\n", (int)time(0), numCli);
			send(sockfd,buffer_c,sizeof(buffer_c),0);

			sem_post(&guiche);//unlock semáforo
			pthread_mutex_unlock(&mutex);//unlock trinco
			return NULL;
		}
	}
	pthread_mutex_unlock(&mutex); //unlock trinco

	sem_wait(&semClientes);//lock semáforo
	pthread_mutex_lock(&mutex); //lock trinco



	//------------DESISTE POR MEDO-------------
	num = rand()%100;
	if(num <= PROB_DESISTE_MEDO){
		printf("O cliente %d entrou na zona de embarque.\n", numCli);
		printf("O cliente %d desistiu porque ficou com medo.\n", numCli);
		sprintf(buffer_c, "%d DESISTE_EMBARQUE\n", (int)time(0));
		send(sockfd, buffer_c, sizeof(buffer_c), 0);
		sem_post(&semClientes); //lock semáforo
		if (filaVIP == 0) sem_post(&guiche); //se não existe ninguém na fila VIP bloqueia o semáforo Guiche
		pthread_mutex_unlock(&mutex);//unlock trinco
		return NULL;
	}

	numEmbarque++; //incrementa o número de passageiros na zona de embarque


	if (filaVIP == 0 && numEmbarque < CAPACIDADE_CARROS) sem_post(&guiche); //se não existem pessoas na fila VIP e existe espaço no carro unlock semaforo

	printf("O cliente %d entrou na zona de embarque.\n", numCli);
	sprintf(buffer_c, "%d ENTRA_EMBARQUE %d 0\n", (int)(time(0)-temp_i), numCli);
	send(sockfd, buffer_c, sizeof(buffer_c),0);
	pthread_mutex_unlock(&mutex);//unlock trinco

	sem_wait(&embarqueCarro);//lock semáforo
	pthread_mutex_lock(&mutex);//lock trinco
	carro2v = carro;
	printf("O cliente %d entrou no carro %d.\n", numCli, carro2v);
	sprintf(buffer_c, "%d EMBARQUE %d %d\n", (int)time(0), numCli, carro2v);
	send(sockfd,buffer_c,sizeof(buffer_c),0);
	pthread_mutex_unlock(&mutex);//unlock trinco

	sem_wait(&desembarqueCarro);//lock semáforo carro
	pthread_mutex_lock(&mutex);//lock trinco
	printf("O cliente %d saiu do carro %d.\n", numCli, carro2v);
	sprintf(buffer_c, "%d DESEMBARQUE %d %d\n", (int)time(0), numCli, carro2v);
	send(sockfd,buffer_c,sizeof(buffer_c),0);
	sem_post(&embarqueCarro);//unlock semáforo
	pthread_mutex_unlock(&mutex);//unlock trinco

	return NULL;
}






// ----------------------------------------------- CARRO --------------------------------------------------
void * funcaoCarro(void *ptr){
	pthread_mutex_lock(&mutex);
	pthread_detach(pthread_self());
	int id = numCarro++;
	char buffer_c[256];
	time_t timestampPartida, hora_i;
	char variavel[20] = "carro";
	pthread_mutex_unlock(&mutex);


	void comecaViagem(){
		int i;
		for(i = 0; i < CAPACIDADE_CARROS; i++)
			sem_post(&embarqueCarro);//unlock semáforo quando inicia a viagem

		printf("O carro %d iniciou a viagem.\n", id);
		sprintf(buffer_c, "%d ARRANQUE %d\n", (int)time(0), id);
		send(sockfd,buffer_c,sizeof(buffer_c),0);

		sem_post(&carroControlo); //abre semaforo dos carros
		timestampPartida = time(0);
		pthread_mutex_unlock(&mutex);
		while((int)(time(0) - timestampPartida) < TEMPO_VIAGEM); // tempo de viagem
		pthread_mutex_lock(&mutex);
		printf("O carro %d terminou a viagem e chegou a zona de desembarque.\n", id);
		sprintf(buffer_c, "%d FINAL_VIAGEM %d\n", (int)time(0), id);
		send(sockfd, buffer_c, sizeof(buffer_c), 0);
		for(i = 0; i < CAPACIDADE_CARROS; i++)
			sem_post(&desembarqueCarro); //desbloquear o semáforo para cada cliente
	}


	//loop infinito
	while(1){
		pthread_mutex_lock(&mutex);
		printf("O carro %d ficou em lista de espera.\n", id);
		sprintf(buffer_c, "%d ESPERA %d\n", (int)time(0), id);
		send(sockfd, buffer_c, sizeof(buffer_c), 0);
		pthread_mutex_unlock(&mutex);
		sem_wait(&carroControlo); // espera pelo semaforo dos carros
		pthread_mutex_lock(&mutex);
		printf("O carro %d chegou à zona de embarque.\n", id);
		sprintf(buffer_c, "%d DISPONÍVEL %d\n", (int)time(0), id);
		send(sockfd, buffer_c, sizeof(buffer_c), 0);
		int i;
		numEmbarque = 0;
		sem_getvalue(&guiche, &i); //se o semáforo está bloqueado o valor i é 0
		if(filaVIP == 0 && i == 0) sem_post(&guiche);  //se não existem pessoas na vila VIP e o semáforo está bloqueado
		for(i = 0; i < CAPACIDADE_CARROS; i++) sem_post(&semClientes); //desbloqueia o semáforo clientes enquanto existem pessoas na fila


		carro = id;
		hora_i = time(0);

		if(time(0) > start + TEMPO_SIMULACAO){
			pthread_mutex_unlock(&mutex);
			usleep(100000);
			pthread_mutex_lock(&mutex);

			if(numEmbarque == 0){
				printf("O carro %d vai ser arrumado.\n", id);
				carrosEmUso--;
				sem_post(&carroControlo);
				pthread_mutex_unlock(&mutex);
				return NULL;
			}

			for(i = 0; i < CAPACIDADE_CARROS; i++)
				sem_post(&embarqueCarro);

			printf("O carro %d iniciou viagem.\n", id);
			sprintf(buffer_c, "%d ARRANQUE %d\n", (int)time(0), id);
			send(sockfd,buffer_c,sizeof(buffer_c),0);

			sem_post(&carroControlo); //abre semaforo dos carros
			timestampPartida = time(0);
			pthread_mutex_unlock(&mutex);
			while((int)(time(0) - timestampPartida) < TEMPO_VIAGEM); // tempo de viagem
			pthread_mutex_lock(&mutex);
			printf("O carro %d completou a viagem e chegou à zona de desembarque.\n", id);
			sprintf(buffer_c, "%d FINAL_VIAGEM %d\n", (int)time(0), id);
			send(sockfd,buffer_c,sizeof(buffer_c),0);
			for(i = 0; i < CAPACIDADE_CARROS; i++)
				sem_post(&desembarqueCarro);
		}
		else{
			pthread_mutex_unlock(&mutex);
			while(numEmbarque < CAPACIDADE_CARROS && time(0) < start + TEMPO_SIMULACAO);
			pthread_mutex_lock(&mutex);
			if(numEmbarque == CAPACIDADE_CARROS){
				pthread_mutex_unlock(&mutex);
					usleep(100000);
				pthread_mutex_lock(&mutex);

					comecaViagem();
			}
			else{
				pthread_mutex_unlock(&mutex);
					usleep(100000);
				pthread_mutex_lock(&mutex);

					if(numEmbarque == 0){
						printf("O carro %d está a ser guardado.\n", id);
						carrosEmUso--;
						sem_post(&carroControlo);
						pthread_mutex_unlock(&mutex);
						return NULL;
					}

					comecaViagem();
			}
		}
	pthread_mutex_unlock(&mutex);
	}
	return NULL;
}




//--------------------------------  tratamento dos pedidos do monitor ---------------------------------
void * menuMonitorCmd(void *arg){
	struct sockaddr_un cli_addr;
	int acabou, n, id;

	int sockfd =*((int *) arg), clilen=sizeof(cli_addr);

	char buffer[256];

	//ciclo do monitor
	while(1){
		acabou=0;
		if((n=recv(sockfd, buffer, sizeof(buffer), 0)) <= 0){
			if(n < 0)
				perror("recv error");
			acabou = 1;
		}
		buffer[n]='\0';

		if(!strcmp(buffer, "termina\n")){
			emExecucao = 0;
			exit(1);
		}
		else{
			if(!strcmp(buffer, "inicio\n"))
				emExecucao = 1;
			if(!strcmp(buffer, "pausa\n"))
				pausa = 1;
			if(!strcmp(buffer, "retomar\n"))
				pausa = 0;
		}
	}
	return NULL;
}





//------------------------------------------ MAIN ------------------------------------------
int main(int argc, char *argv[]){

	srand(time(NULL));

	numCliente=1;
	numCarro=1;
	carro = 1;
	numEmbarque = 0;
	filaVIP = 0;

	if(argc<2){
		printf("ERRO: Não introduziu o ficheiro de configuração da simulação\n");
		return 1;
	}
	else {

		// LEITURA DO FICHEIRO DE CONFIGURAÇÃO
		int * configVal = leituraFicheiroConfig(argv[1]);
		INICIO_FINAL									= configVal[0];
		TEMPO_SIMULACAO								= configVal[1];
		TEMPO_MEDIO_CHEGADA_CLIENTES	= configVal[2];
		PROB_DESISTE_ESPERA						= configVal[3];
		PROB_DESISTE_ESPERA_VIP				= configVal[4];
		PROB_DESISTE_MEDO							= configVal[5];
		PROB_DESISTE_MEDO_VIP					= configVal[6];
		TEMPO_VIAGEM									= configVal[7];
		CAPACIDADE_CARROS							= configVal[8];
		NUMERO_CARROS									= configVal[9];




		// Inicializar semáforos
		sem_init (&guiche, 0, 0);
		sem_init (&semClientes, 0, 0);
		sem_init (&embarqueCarro, 0, CAPACIDADE_CARROS);
		sem_init (&desembarqueCarro, 0, 0);
		sem_init (&carroControlo, 0, 1);


		//Cria socket stream
		if((sockfd=socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
			perror("Can't open socket stream");


		/* Primeiro uma limpeza preventiva!
		   Dados para o socket stream: tipo + nome do ficheiro.
	     O ficheiro identifica o servidor */
		serv_addr.sun_family=AF_UNIX;
		strcpy(serv_addr.sun_path, UNIXSTR_PATH);
		servlen=strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);


		/* Tenta estabelecer uma ligação. Só funciona se o servidor tiver
         sido lançado primeiro (o servidor tem de criar o ficheiro e associar
         o socket ao ficheiro) */
		if(connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
			perror("Connection error");


		//monitor
		pthread_t thread;
		pthread_create(&thread, NULL, &menuMonitorCmd, &sockfd);

		while(!emExecucao);

		int i, i1, i2;
		char buffer[256];
		carrosEmUso = NUMERO_CARROS;


		start = time(0);
		sprintf(buffer, "%d INÍCIO\n", (int)start);
		send(sockfd,buffer,sizeof(buffer),0);

		TEMPO_SIMULACAO *= 60;

		//thread carro
		for(i = 0; i < NUMERO_CARROS; i++){
			pthread_create(&thread, NULL, &funcaoCarro, &sockfd);
		}

		while((int)(time(0) - start) < TEMPO_SIMULACAO){
			i1 = rand()%100;
			if (i1 <= 10){
				//thread cliente prioritário
				pthread_create(&thread, NULL, &clienteVIP, &sockfd);
			}
			else{
				//thread cliente normal
				pthread_create(&thread, NULL, &clienteNormal, &sockfd);
			}
			i1 = rand()%1;
			i2 = rand()%3;
			i2 = i2 * 100000;
			if(i1 == 0)
				i2 = 0 - i2;
			usleep((TEMPO_MEDIO_CHEGADA_CLIENTES*1000000) + i2);

			if(pausa){
				pthread_mutex_lock(&mutex);
				pauseControlo = 1;
			}
			while(pausa);
			if(pauseControlo){
				pthread_mutex_unlock(&mutex);
				pauseControlo = 0;
			}

		}

		while(carrosEmUso>0);
		printf("\nFim de simulação.\n");
		sprintf(buffer,"%d FIM \n",(int) time(0));
		send(sockfd,buffer,sizeof(buffer),0);

		while(emExecucao);

		close(sockfd);
	}
	return 0;
}
