CC = gcc
CFLAGS = -Wall -g

all: myshell looper mypipe

myshell: myshell.c LineParser.o
	$(CC) $(CFLAGS) -o myshell myshell.c LineParser.o

LineParser.o: LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c LineParser.c

looper: Looper.c
	$(CC) $(CFLAGS) -o looper Looper.c

mypipe: mypipe.c
	$(CC) $(CFLAGS) -o mypipe mypipe.c

clean:
	rm -f *.o myshell looper mypipe
