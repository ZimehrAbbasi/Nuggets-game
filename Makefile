L = support

CFLAGS= -Wall -pedantic -std=c11 -ggdb -I$L -I$(LC)
CC=gcc

PROG = server
OBJS = server.o
LIBS = $L/support.a

.PHONY: all test clean

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

server.o: server.c gamestate.h gold.h grid.h player.h spectator.h

test: 
	echo "testing scripts go here"

clean:
	rm -rf *.dYSM
	rm -rf *~ *.o
	rm -rf $(PROG)