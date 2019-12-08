all: server client

client: DUMBclient.c DUMBlib.c DUMB.h
	gcc -std=c99 -g -c DUMBlib.c -o DUMBlib.o
	gcc -std=c99 -g DUMBclient.c -o DUMBclient.o DUMBlib.o

server: DUMBserver.c DUMBlib.c DUMB.h
	gcc -std=c99 -g -c DUMBlib.c -o DUMBlib.o
	gcc -std=c99 -g -lpthread DUMBserver.c -o DUMBserver.o DUMBlib.o
	
	
clean:
	rm -rf *.o
