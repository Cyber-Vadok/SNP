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

all: pipe1 pipe2

pipe1: pipe1.c
	gcc -ggdb -o pipe1 pipe1.c
	
pipe2: pipe2.c
	gcc -ggdb -o pipe2 pipe2.c

clean:
	rm pipe1 pipe2

##################################################