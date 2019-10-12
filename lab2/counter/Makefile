CC = g++

counter: lab2.o locks_barriers.o
		$(CC) -o counter -g -pthread -Ofast lab2.o locks_barriers.o

lab2.o: lab2.c locks_barriers.h
		$(CC) -c -pthread -Ofast -g lab2.c 

locks_barriers.o: locks_barriers.c locks_barriers.h
		$(CC) -c -pthread -g -Ofast locks_barriers.c

clean: 
		rm -f *.o
		rm -f counter
		rm -f out.txt