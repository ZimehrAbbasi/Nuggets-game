#
# Makefile for server and it's dependencies
# CS50 project Spring 2021
#
# TEAM PINE, May 2021
#

########### support library ###########
L = support

########### compiler flags ############
CFLAGS= -Wall -pedantic -std=c11 -ggdb -I$(L)
CC=gcc
VALGRIND= valgrind --leak-check=full --show-leak-kinds=all

######### local dependencies ##########
LIB = locallib.a


PROG = server
OBJS = server.o
LIBS =  $(L)/support.a -lm

######### default rule #######
all: $(LIBS) $(PROG)

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIB) $(LIBS) -o $@
	rm -rf $(OBJS) $(LIB)

###### dependency library #####
$(LIB): gamestate.o player.o grid.o gold.o spectator.o
	ar cr $(LIB) $^
	rm -rf *.o


###### dependency objects #######
server.o: server.c $(LIB)

gamestate.o:  gamestate.h player.h grid.h gold.h spectator.h

player.o: player.h grid.h $(L)/message.h

spectator.o: spectator.h grid.h $(L)/message.h

grid.o: grid.h $(L)/file.h player.h gamestate.h $(L)/message.h

gold.o: gold.h grid.h -lm player.h

$(L)/support.a:
	make -C $(L)



########### tests #############
quicktest: $(PROG)
	$(VALGRIND)	./server ./maps/main.txt 257573

test: 
	make -C tests test

memcheck:
	make -C tests memcheck

######## phony target ########
.PHONY: all test clean


########### clean ############
clean:
	rm -rf *.dYSM
	rm -rf *~ *.o
	rm -rf $(PROG)
	rm -rf $(LIB)
	make -C $(L) clean
