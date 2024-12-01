# Makefile to build and clean

CC = gcc
CFLAGS = -lpthread

all:
	@if [ -f pa06 ]; then cp pa06 pa06.back; fi
	$(CC) $(CFLAGS) -o pa06 pa06.c random437.h

clean:
	rm -f pa06
	rm -f pa06.back
