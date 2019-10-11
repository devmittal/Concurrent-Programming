CC = g++

mysort: lab1.o merge.o bucket_sort.o
		$(CC) -o mysort -pthread -Ofast -g lab1.o merge.o bucket_sort.o

lab1.o: lab1.c config.h merge.h bucket_sort.h
		$(CC) -c -pthread -Ofast -g lab1.c

merge.o: merge.c merge.h config.h
		$(CC) -c -pthread -Ofast -g merge.c

bucket_sort.o: bucket_sort.c bucket_sort.h config.h
		$(CC) -c -pthread -Ofast -g bucket_sort.c 

clean: 
		rm -f *.o
		rm -f mysort