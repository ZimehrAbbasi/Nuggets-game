L = support

CFLAGS= -Wall -pedantic -std=c11 -ggdb -lm -I$L -I$(L)
CC=gcc

LIB = stuff.a

# PROG = server
OBJS = server.o
LIBS = $L/support.a

.PHONY: all test clean

$(PROG): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

server.o: server.c gamestate.h gold.h grid.h player.h spectator.h

$(LIB): gamestate.o player.o grid.o gold.o spectator.o
	ar cr $(LIB) $^


gamestate.o: player.h grid.h gold.h spectator.h gamestate.h

player.o: player.h grid.h $(L)/message.h

spectator.o: spectator.h grid.h $(L)/message.h

grid.o: grid.h $(L)/file.h player.h gamestate.h $(L)/message.h

gold.o: gold.h grid.h player.h



# test: 
# 	echo "testing scripts go here"

clean:
	rm -rf *.dYSM
	rm -rf *~ *.o
	rm -rf $(PROG)

all: $(PROG)
