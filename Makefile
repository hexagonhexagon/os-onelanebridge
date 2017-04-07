CC=gcc
CFLAGS=-lpthread -lrt
DEPS = spin.h queue.h

all: greedy.o fcfs.o
greedy.o: greedy.c spin.h
	$(CC) -c greedy.c
fcfs.o: fcfs.c spin.h queue.h
	$(CC) -c fcfs.c

greedy:
	$(CC) greedy.o -o onelanebridge $(CFLAGS)
fcfs:
	$(CC) fcfs.o -o onelanebridge $(CFLAGS)

clean:
	rm -f onelanebridge *.o
