#include <pthread.h>
#include <string.h>
#include "unix.h"
#include "util.h"

//apontador para o ficheiro onde será escrito o relatório da simulação
FILE * relatorio;

//variáveis globais para a estatística
int num_fila_normal = 0,
		num_carros = 2,
		num_cli_normal = 0,
		desiste_espera = 0,
		desiste_medo = 0,
		num_viagens = 0,
		inicio_simulacao = 0,
		fim_simulacao = 0,
		soma_espera_normal = 0,
		esperou_normal = 0;

//VAR DE CONTROLO
int emExecucao = 0, bloqueado = 0;


void estatistica(){
	if(emExecucao && !bloqueado)
		printf(" 1. Estado atual: Simulacao a decorrer.\n");
	else
	{
		if(bloqueado)
			printf(" 1. Estado atual: Simulação em pausa.\n");
		else
			printf(" 1. Estado atual: Simulação terminada.\n");
	}
	printf(" 2. Tamanho atual da fila de espera para Clientes Normais: %d\n", num_fila_normal);
	printf(" 4. Número atual de Carros: %d\n", num_carros);
	printf(" 5. Clientes Normais: %d\n", num_fila_normal);
	printf(" 8. Desistências na Fila de espera: %d\n", desiste_espera);
	printf(" 9. Desistências Por medo: %d\n", desiste_medo);
	printf("10. Total de desistências: %d\n", desiste_espera + desiste_medo);
	printf("11. Número de viagens: %d\n", num_viagens);
}


//função que fica à escuta das mensagens do simulador

void *escuta_comunicacao(void *arg)
{
	int sockfd=*((int *) arg);
	int msg, tempo=0, num_comandos, id, com;
	char buffer[256],comando[20];

	//ciclo que fica à espera de mensagens do simulador

	while(1)
	{
		if((msg=recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
		{
			if(msg < 0)
				perror("recv");
		}
		else
		{
           	buffer[msg]='\0';
			fprintf(relatorio,"%s", buffer);

			num_comandos = sscanf(buffer,"%d %s %d %d", &tempo, comando, &id, &com);

			if(num_comandos>0)
			{
				if(!strcmp(comando,"INICIO"))
				{
					inicio_simulacao = tempo;
				}
				else if(!strcmp(comando,"CHEGADA"))
				{
						num_fila_normal++;
				}
				else if(!strcmp(comando,"DESISTE_FILA"))
				{
					desiste_espera++;
				}
				else if(!strcmp(comando,"ENTRA_EMBARQUE"))
				{
						soma_espera_normal += tempo;
						esperou_normal++;

				}
				else if(!strcmp(comando,"DESISTE_EMBARQUE"))
				{
					desiste_medo++;
				}
			}
		}
	}
	return NULL;
}


//função principal do monitor

int main(int argc, char *argv[])
{
	struct sockaddr_un serv_addr, cli_addr;
	int sockfd, servlen, newsockfd;

	int clilen=sizeof(cli_addr);
	char buffer[256];
	char menu[512] = "----------------------------------\n| comandos disponiveis no monitor|\n----------------------------------\n| inicio - Comecar simulacao     |\n| pausa  - Pausar simulacao      |\n| retomar - Retomar simulacao    |\n| estado - Estado da simulacao   |\n| sair   - Sair do monitor       |\n----------------------------------\n";

	//Criacao do socket UNIX
	if((sockfd=socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		perror("cant open socket stream");
	serv_addr.sun_family=AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	servlen=strlen(serv_addr.sun_path)+sizeof(serv_addr.sun_family);
	unlink(UNIXSTR_PATH);
	if(bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
		perror("cant bind local address");
	listen(sockfd, 1);

	if((newsockfd=accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		perror("accept error");

	//criacao da tarefa que ira tratar da comunicação com o simulador

	pthread_t thread;
	pthread_create(&thread, NULL, &escuta_comunicacao, &newsockfd);

	//Ciclo que espera pela informacao vinda da consola

	printf("%s",menu);

	unlink("relatorio.log");
	relatorio=fopen("relatorio.log", "a");

	do
	{
		fgets(buffer, sizeof(buffer), stdin);



		if(send(newsockfd, buffer, sizeof(buffer), 0) == -1)
		{
			perror("send");
			exit(1);
		}

		if(!strcmp (buffer, "inicio\n"))
		{
			emExecucao=1;
			bloqueado=0;
		}
		if(!strcmp (buffer, "pausa\n"))
		{
			bloqueado=1;
			printf("Simulacao em pausa - \"retomar\" para continuar\n");
		}
		if(!strcmp (buffer, "retomar\n"))
		{
			bloqueado=0;
		}
		if(!strcmp (buffer, "estado\n"))
		{
			estatistica();
		}
	}
	while(strcmp (buffer, "sair\n"));

	emExecucao = 0;
	bloqueado = 0;

	printf("\nExecução terminou.\n\n");
	estatistica();
	fclose(relatorio);
	close(sockfd);

	return 0;
}
