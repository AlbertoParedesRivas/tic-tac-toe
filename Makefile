all: modelo.o control.o
	gcc -o juegoGato modelo.o control.o -pthread `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
modelo.o: modelo/modelo.c modelo/modelo.h
	gcc -c -pthread -pipe modelo/modelo.c `pkg-config --cflags --libs gtk+-3.0` -o modelo.o
control.o: control/control.c
	gcc -c -pthread -pipe control/control.c `pkg-config --cflags --libs gtk+-3.0` -o control.o
clean:
	rm -f *.o juegoGato