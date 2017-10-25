all: simulador monitor

monitor: monitor.o
	gcc -Wall -g monitor.o -o monitor -lpthread
monitor.o: monitor.c Header.h
	gcc -c monitor.c
simulador: simulador.o
	gcc -Wall -g simulador.o -o simulador -lpthread
simulador.o: simulador.c util.h
	gcc -c simulador.c
clean:
	rm *.o
