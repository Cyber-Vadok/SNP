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

all: C S

C: C.c
	gcc -ggdb -o C C.c
	
S: S.c
	gcc -ggdb -o S S.c

clean:
	rm C S

##################################################