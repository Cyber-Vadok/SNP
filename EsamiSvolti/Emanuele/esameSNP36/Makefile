##################################################
# Makefile 
#
# author Catiuscia Melle
#
# Compilazione degli example codes 
#
# Esempio di TCP Echo client e TCP Echo server 
# IPv4 only - no hostname resolution 
# uso dei wildcard IPv4 addresses
# 
##################################################

all: server client

server: server.c
	gcc -ggdb -o server server.c
	
client: client.c
	gcc -ggdb -o client client.c

clean:
	rm client server

##################################################