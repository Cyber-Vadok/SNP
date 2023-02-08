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

all: MulticastSender MulticastReceiver

MulticastSender: MulticastSender.c
	gcc -ggdb -o MulticastSender MulticastSender.c

MulticastReceiver: MulticastReceiver.c
	gcc -ggdb -o MulticastReceiver MulticastReceiver.c

clean:
	rm MulticastSender MulticastReceiver

##################################################