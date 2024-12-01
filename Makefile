# Make file to make things

# Flags 
CC = gcc
CFLAGS = -lpthread

all:
	cp pa06 pa06.back
	$(CC) $(CFLAGS) -o pa06 pa06.c 

clean: 
  rm -f pa06 pa06.c.back