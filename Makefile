all:
	gcc greedy.c -o greedy -lpthread -lrt
	gcc fcfs.c -o fcfs -lpthread -lrt
