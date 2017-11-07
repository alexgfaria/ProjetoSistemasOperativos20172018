programa: monitor.o simulador.o
	gcc monitor.o simulador.o Header.h -o programa
simulador: simulador.o Header.h
	gcc -g simulador.o -o simulador

simulador.o: simulador.c Header.h
	gcc -g -c simulador.c

monitor: monitor.o Header.h
	gcc -g monitor.o -o monitor

monitor.o: monitor.c Header.h
	gcc -g -c monitor.c

clean:
	rm *.o
