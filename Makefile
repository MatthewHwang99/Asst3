all: server

client: DUMBclient.c DUMB.h
	gcc -std=c99 -g DUMBclient.c -o DUMBclient

server: DUMBserver.c DUMB.h
	gcc -std=c99 -g -lpthread DUMBserver.c -o DUMBserver
	

	
clean:
	rm -rf DUMBclient
	rm -rf DUMBserver
