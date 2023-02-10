all: server client

server: server.c Header.h
	gcc -o server server.c
	
client: client.c Header.h
	gcc -o client client.c
	
lettore: lettore.c
	gcc -o lettore lettore.c
	
clean:
	rm -f server client lettore
