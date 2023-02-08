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

all: TCPUDPServer TCPUDPClient 

TCPUDPServer: TCPUDPServer.c
	gcc -ggdb -o TCPUDPServer TCPUDPServer.c

TCPClient: TCPUDPClient.c
	gcc -ggdb -o TCPUDPClient TCPUDPClient.c

clean:
	rm TCPUDPServer TCPUDPClient 

##################################################