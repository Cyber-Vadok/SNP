all: server client

server: server.c Header.h
	gcc -o server server.c

client: client.c Header.h
	gcc -o client client.c

clean:
	rm -f server client
