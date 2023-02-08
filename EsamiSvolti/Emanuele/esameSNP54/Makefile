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

all: sharedMemUDP UDPClient

sharedMemUDP: sharedMemUDP.c
	gcc -ggdb -o sharedMemUDP sharedMemUDP.c
	
UDPClient: UDPClient.c
	gcc -ggdb -o UDPClient UDPClient.c

clean:
	rm sharedMemUDP UDPClient

##################################################