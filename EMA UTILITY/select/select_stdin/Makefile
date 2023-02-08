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

all: select_stdin

select_stdin: select_stdin.c
	gcc -ggdb -o select_stdin select_stdin.c

clean:
	rm select_stdin

##################################################