all: multicastreceiver multicastsender

multicastreceiver: MulticastReceiver.c
	gcc -o multicastreceiver MulticastReceiver.c

multicastsender: MulticastSender.c
	gcc -o multicastsender MulticastSender.c

clean:
	rm -f multicastreceiver multicastsender
