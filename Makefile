L = ./support

CFLAGS= -Wall -pedantic -std=c11 -ggdb -I$L -I$(LC)
CC=gcc

PROG = server
OBJS = server.o 
LLIBS = $L/libcs50.a $(LC)/common.a

.PHONY: all test clean

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) -o $(PROG)

server.o: $L/file.h $L/message.h $L/log.h gamestate.h gold.h grid.h player.h spectator.h

test: 
	echo "testing scripts go here"

clean:
	rm -rf *.dYSM
	rm -rf *~ *.o
	rm -rf $(PROG)