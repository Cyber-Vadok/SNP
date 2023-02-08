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

all: Smulticast Cmulticast

Smulticast: Smulticast.c
	gcc -ggdb -o Smulticast Smulticast.c

Cmulticast: Cmulticast.c
	gcc -ggdb -o Cmulticast Cmulticast.c

clean:
	rm Smulticast Cmulticast

##################################################