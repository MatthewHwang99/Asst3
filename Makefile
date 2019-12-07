all: server client

client: DUMBclient.c DUMB.h
	gcc -std=c99 -g DUMBclient.c -o DUMBclient.o

server: DUMBserver.c DUMB.h
	gcc -std=c99 -g -lpthread DUMBserver.c -o DUMBserver.o
	
clean:
	rm -rf *.o
