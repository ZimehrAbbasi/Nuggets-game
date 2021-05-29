#
# Makefile for server tests
# CS50 project Spring 2019
#
# TEAM PINE, May 2021
#

LIB = support.a
TESTS = miniclient messagetest
SUPPORT = ../../support

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I $(SUPPORT)
CC = gcc
MAKE = make

.PHONY: all clean

############# default rule ###########
all: $(LIB) $(TESTS) 

$(LIB): 
	make -C $(SUPPORT)
	cp $(SUPPORT)/support.a ./support.a

messagetest: $(SUPPORT)/message.c $(SUPPORT)/message.h $(SUPPORT)/log.h $(SUPPORT)/log.o
	$(CC) $(CFLAGS) -DUNIT_TEST $(SUPPORT)/message.c $(SUPPORT)/log.o -o messagetest

miniclient: miniclient.o $(SUPPORT)/message.o $(SUPPORT)/log.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

miniclient.o: miniclient.c
message.o: $(SUPPORT)/message.h $(SUPPORT)/message.c $(SUPPORT)/log.o
	# $(CC) $(CFLAGS) $(SUPPORT)/message.c $(SUPPORT)/log.o -o $@

############# clean ###########
clean:
	make -C $(SUPPORT) clean 
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f $(LIB)
	rm -f $(TESTS)
