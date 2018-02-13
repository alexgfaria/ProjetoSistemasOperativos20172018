#include <pthread.h>
#include <string.h>
#include "unix.h"
#include "util.h"


FILE * relatorio;




int numFilaNormal = 0, numFilaVIP	= 0, numClientesNormal = 0, numClientesVIP = 0, desisteEspera = 0, desisteMedo = 0, numViagens = 0, beginSim = 0, endSim = 0, esperaNormalResultado = 0, esperaVIPResultado = 0, esperouVIP = 0, esperouNormal = 0 /*, numCarros = 0*/;


int emExecucao = 0, pausa = 0;







void stats(){
	if(emExecucao && !pausa)
		printf("- Estado atual=> Simulação a decorrer!\n");
	else {
		if(pausa)
			printf("- Estado atual=> Simulação pausada!\n");
		else
			printf("- Estado atual=> Simulação terminada!\n");
	}
	printf("- Tamanho da fila de espera para clientes normais: %d\n", numFilaNormal);
	printf("- Tamanho da fila de espera para clientes prioritários: %d\n", numFilaVIP);
	//printf("- Número de carros: %d\n", numCarros);
	printf("- Clientes normais: %d\n", numFilaNormal);
	printf("- Clientes prioritários: %d\n", numFilaVIP);
	printf("- Total de clientes: %d\n", numFilaNormal + numFilaVIP);
	printf("- Desistências na fila de espera: %d\n", desisteEspera);
	printf("- Desistências por medo: %d\n", desisteMedo);
	printf("- Total de desistências: %d\n", desisteEspera + desisteMedo);
	printf("- Número de viagens: %d\n", numViagens);
	if(esperouNormal){
		printf("- Tempo médio de espera de clientes normais: %.1f minutos\n", (float)esperaNormalResultado/(float)esperouNormal);
	}
	else {
		printf("- Tempo médio de espera de clientes normais: 0 minutos\n");
	}
	if(esperouVIP){
		printf("- Tempo médio de espera de clientes prioritários: %.1f minutos\n", (float)(esperaVIPResultado/esperouVIP));
	}
	else {
		printf("- Tempo médio de espera de clientes prioritários: 0 minutos\n");
	}
}

void escreveStats(){
	FILE * logResultado = fopen("log.txt", "w");
	fprintf(logResultado, "- Estado atual=> Simulação terminada!\n");
	fprintf(logResultado, "- Tamanho da fila de espera para clientes normais: %d\n", numFilaNormal);
	fprintf(logResultado, "- Tamanho da fila de espera para clientes prioritários: %d\n", numFilaVIP);
	//printf("- Número de carros: %d\n", numCarros);
	fprintf(logResultado, "- Clientes normais: %d\n", numFilaNormal);
	fprintf(logResultado, "- Clientes prioritários: %d\n", numFilaVIP);
	fprintf(logResultado, "- Total de clientes: %d\n", numFilaNormal + numFilaVIP);
	fprintf(logResultado, "- Desistências na fila de espera: %d\n", desisteEspera);
	fprintf(logResultado, "- Desistências por medo: %d\n", desisteMedo);
	fprintf(logResultado, "- Total de desistências: %d\n", desisteEspera + desisteMedo);
	fprintf(logResultado, "- Número de viagens: %d\n", numViagens);
	if(esperouNormal){
		fprintf(logResultado, "- Tempo médio de espera de clientes normais: %.1f minutos\n", (float)esperaNormalResultado/(float)esperouNormal);
	}
	else {
		fprintf(logResultado, "- Tempo médio de espera de clientes normais: 0 minutos\n");
	}
	if(esperouVIP){
		fprintf(logResultado, "- Tempo médio de espera de clientes prioritários: %.1f minutos\n", (float)(esperaVIPResultado/esperouVIP));
	}
	else {
		fprintf(logResultado, "- Tempo médio de espera de clientes prioritários: 0 minutos\n");
	}
	fclose(logResultado);
}








//função que fica à escuta das mensagens do simulador
void * comunicacaoComSimulador(void * arg){
	int sockfd = * ((int *) arg);
	int msg, tempo = 0, num_comandos, id, com;
	char buffer[256], comando[20];

	//ciclo que fica à espera de mensagens do simulador
	while(1){
								// Socket, buffer,  length,       flags
								//receive a message from a connected socket
		if((msg = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0){
			if(msg < 0)
				perror("recv");
		}
		else {

			buffer[msg]='\0';
			fprintf(relatorio,"%s", buffer);
			num_comandos = sscanf(buffer,"%d %s %d %d", &tempo, comando, &id, &com);

			//se foi inserido um comando
			if(num_comandos > 0){
				if(!strcmp(comando,"INICIO")){
					beginSim = tempo;
				}
				else if(!strcmp(comando,"CHEGADA")){
					if(com)
						numFilaVIP++;
					else
						numFilaNormal++;
				}
				else if(!strcmp(comando,"DESISTE_FILA")){
					desisteEspera++;
				}
				else if(!strcmp(comando,"ENTRA_EMBARQUE")){
					if(com){
						esperaVIPResultado += tempo;
						esperouVIP++;
						numViagens++;
					}
					else{
						esperaNormalResultado += tempo;
						esperouNormal++;
						numViagens++;
					}
				}
				else if(!strcmp(comando,"DESISTE_EMBARQUE")){
					desisteMedo++;
					numViagens--;
				}
			}
		}
	}
	return NULL;
}







// ----------------------- MAIN --------------------------------

int main(int argc, char * argv[]){
	struct sockaddr_un serv_addr, cli_addr;
	int sockfd, servlen, newsockfd;
	int clilen = sizeof(cli_addr);
	char buffer[256];
	char menu[512] = "----------------------------------\n|            MONITOR             |\n----------------------------------\n| inicio  - Iniciar simulação    |\n| pausa   - Pausar simulação     |\n| retomar - Retomar simulação    |\n| estado  - Estado da simulação  |\n| sair    - Sair do monitor      |\n----------------------------------\n";

/*
									"----------------------------------\n
									|            MONITOR             |\n
									----------------------------------\n
									| inicio  - Iniciar simulação    |\n
									| pausa   - Pausar simulação     |\n
									| retomar - Retomar simulação    |\n
									| estado  - Estado da simulação  |\n
									| sair    - Sair do monitor      |\n
									----------------------------------\n";
*/

	//cria socket stream
	if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		perror("Failure opening socket stream");


	//limpeza preventiva
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);

	/* O servidor é quem cria o ficheiro que identifica o socket.
         Elimina o ficheiro, para o caso de algo ter ficado pendurado.
         Em seguida associa o socket ao ficheiro.
         A dimensão a indicar ao bind não â a da estrutura, pois depende
         do nome do ficheiro */
	servlen = strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);
	unlink(UNIXSTR_PATH);
	if(bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
		perror("can't bind local address");


	//servidor pronto a aceitar 1 cliente para o socket stream
	listen(sockfd, 1);


	/* Não esquecer que quando o servidor aceita um cliente cria um
		   socket para comunicar com ele. O primeiro socket (sockfd) fica
		   Ä espera de mais clientes */
	if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		perror("accept error");

	//Tarefa da comunic com o sim
	pthread_t thread;
	pthread_create(&thread, NULL, &comunicacaoComSimulador, &newsockfd);

	//Ciclo que espera pela informacao vinda da consola
	printf("%s", menu);
	unlink("relatorio.log");
	relatorio = fopen("relatorio.log", "a");

	do{
		fgets(buffer, sizeof(buffer), stdin);

		if(send(newsockfd, buffer, sizeof(buffer), 0) == -1){
			perror("send");
			exit(1);
		}


		//strcmp devolve 0 se as strings forem iguais
		if(!strcmp (buffer, "inicio\n")){
			emExecucao = 1;
			pausa = 0;
		}
		if(!strcmp (buffer, "pausa\n")){
			pausa = 1;
			printf("Simulação em pausa - \"retomar\" para continuar\n");
		}
		if(!strcmp (buffer, "retomar\n")){
			pausa = 0;
		}
		if(!strcmp (buffer, "estado\n")){
			stats();
		}
	}
	while(strcmp (buffer, "sair\n"));

	emExecucao = 0;
	pausa = 0;

	printf("\nExecução terminou.\n\n");
	stats();
	fclose(relatorio);
	//fprintf(logResultado, %s, stats());
	escreveStats();
	close(sockfd);

	return 0;
}
