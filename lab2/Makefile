vpath %.c bucket_sort/ counter_program/
vpath %.h bucket_sort/ counter_program/

CC = g++

exec = counter mysort
all: $(exec)

counter: lab2.o locks_barriers.o
		$(CC) -o counter -g -pthread -Ofast lab2.o locks_barriers.o

mysort: lab1.o merge.o bucket_sort.o
		$(CC) -o mysort -pthread -Ofast -g lab1.o merge.o bucket_sort.o

lab2.o: lab2.c locks_barriers.h
		$(CC) -c -pthread -Ofast -g counter_program/lab2.c 

locks_barriers.o: locks_barriers.c locks_barriers.h
		$(CC) -c -pthread -g -Ofast counter_program/locks_barriers.c

lab1.o: lab1.c config.h merge.h bucket_sort.h
		$(CC) -c -pthread -Ofast -g bucket_sort/lab1.c

merge.o: merge.c merge.h config.h
		$(CC) -c -pthread -Ofast -g bucket_sort/merge.c

bucket_sort.o: bucket_sort.c bucket_sort.h config.h
		$(CC) -c -pthread -Ofast -g bucket_sort/bucket_sort.c 

clean: 
		rm -f *.o
		rm -f counter mysort
		rm -f out.txt case3.txt linux_test_bucket.txt test_bucket.txt