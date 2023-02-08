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

all: TCPC 1TCPS 2TCPS

TCPC: TCPC.c
	gcc -ggdb -o TCPC TCPC.c

1TCPS: 1TCPS.c
	gcc -ggdb -o 1TCPS 1TCPS.c
	
2TCPS: 2TCPS.c
	gcc -ggdb -o 2TCPS 2TCPS.c

clean:
	rm TCPC 1TCPS 2TCPS

##################################################