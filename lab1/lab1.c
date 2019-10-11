/*****************************************************************************
​ ​* ​ ​ @file​ ​  		lab1.c
​ * ​ ​ @brief​ ​ 		Merge sort and bucket sort using multithreading
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 24th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include "bucket_sort.h"
#include "config.h"
#include "merge.h"

using namespace std;

#define MAX_INT_SIZE 10 //max number of digits in integer

pthread_t* threads;
size_t* args;
size_t NUM_THREADS;
pthread_barrier_t bar;
int length; //Number of integers in file
struct timespec start, time_end;
char *sorting_alg;
int max_unsorted;
int *unsorted;

/*
 * @func_name - global_init
 * @Description - Initialize global variables
 * @params - void
 * @return - void
 */
void global_init(void)
{
	threads = (pthread_t*)malloc(NUM_THREADS*sizeof(pthread_t));
	args = (size_t*)malloc(NUM_THREADS*sizeof(size_t));
	pthread_barrier_init(&bar, NULL, NUM_THREADS);
}

/*
 * @func_name - global_cleanup
 * @Description - deinitialize global variables
 * @params - void
 * @return - void
 */
void global_cleanup()
{
	free(threads);
	free(args);
	pthread_barrier_destroy(&bar);
}

/*
 * @func_name - thread_main
 * @Description - Callback function for spawned threads
 * @params - args: thread id
 * @return - void
 */
void* thread_main(void* args)
{
	size_t tid = *((size_t*)args);

	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&start);
	}

	if(!strcmp(sorting_alg, "fj"))
	{
		/* call method to fork-join merge sort */
		fj_merge(tid);
	}
	else
	{
		/* Bucket Sort */
		bucket(tid);
	}

	if(tid == 1)
		clock_gettime(CLOCK_MONOTONIC,&time_end);

	return 0;
}


int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	int index;
	char *sorted_filename = NULL;
	char *unsorted_filename;
	int iterator = 0;
	char int_string[MAX_INT_SIZE];
	length = 0;
	unsorted = NULL;
	char* fget_ret;
	NUM_THREADS = 12;
	sorting_alg = NULL;

	static struct option long_options[] = {
		{"name", no_argument, 0, 'n'},      //Optional arg - print name
		{"o",    required_argument, 0, 'o'},//Optional arg - output sorted list to file
		{"t",    required_argument, 0, 't'},//Optional arg - specify number of threads
		{"alg",    required_argument, 0, 'a'}, //Optional arg - Sorting algorithm
		{0     , 0          , 0,  0 }
	};

	/* Loop to get all otpional arguments */
	while((c = getopt_long(argc, argv, "no:t:a:", long_options, &option_index)) != -1)
	{
		/* Handle Optional Arguments */		
		switch(c)
		{
			case 'n': printf("Devansh Mittal\n");
					  return 0; //Return after name is printed
					  break;

			case 'o': sorted_filename = optarg;
					  break;

			case 't': NUM_THREADS = atoi(optarg);
					  break;

			case 'a': sorting_alg = optarg;
					  break;

			default: printf("No optional arguments\n");
		}
	}

	/* Get mandatory arguments */
	for(index = optind; index < argc; index++)
	{
		unsorted_filename = argv[index];
	}

	FILE* input_fileptr = fopen(unsorted_filename,"r"); //Open file for reading

	if(input_fileptr == NULL)
		perror("File could not be opened for reading\n");

	while(!feof(input_fileptr)) //Count number of integers
	{
		if((getc(input_fileptr) == '\n'))
			length++;
	}

	/* If threads are greater than no. of elements, only use as many threads as there are elements */
	if(length < NUM_THREADS)
		NUM_THREADS = length;

	/* Limit number of threads to 50 */
	if(NUM_THREADS > 50)
	{
		printf("Too many threads: limited to 50\n");
		NUM_THREADS = 50;
	}

	if(sorting_alg == NULL)
	{
		printf("Input sorting alg and try again!\n");
		exit(-1);
	}

#ifdef DEBUG
	printf("Length: %d\n", length);
#endif

	/* Allocate heap space for unsorted array */
	unsorted = (int *)malloc(sizeof(int) * (length+1));
	if(unsorted == NULL)
	{
		perror("Unable to allocate heap for unsorted\n");
	}

	/* Go back to beginning of file */
	if(fseek(input_fileptr, 0, SEEK_SET) != 0)
		perror("Unable to go back to beginning of file\n");

	while(!feof(input_fileptr)) //Read integers till EOF
	{
		fget_ret = fgets(int_string, MAX_INT_SIZE, input_fileptr);
		unsorted[iterator++] = atoi(int_string); //Convert string read to integer
	}

	/* Determine max element in file */
	max_unsorted = unsorted[0];
	for(iterator = 1; iterator<length; iterator++)
	{
		if(unsorted[iterator]>max_unsorted)
			max_unsorted = unsorted[iterator];
	}

#ifdef DEBUG
	printf("MAX: %d\n", max_unsorted);
#endif

	global_init();

	/* Launch threads */
	int ret; size_t i;
  	for(i=1; i<NUM_THREADS; i++)
  	{
		args[i]=i+1;

		#ifdef DEBUG
		printf("creating thread %zu\n",args[i]);
		#endif

		ret = pthread_create(&threads[i], NULL, &thread_main, &args[i]);
		if(ret)
		{
			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);
		}
	}
	i = 1;
	thread_main(&i); // master also calls thread_main	

	// join threads
	for(i=1; i<NUM_THREADS; i++)
	{
		ret = pthread_join(threads[i],NULL);
		if(ret)
		{
			printf("ERROR; pthread_join: %d\n", ret);
			exit(-1);
		}

		#ifdef DEBUG
		printf("joined thread %zu\n",i+1);
		#endif
	}

	/* If sorted list needs to be outputted to file */
	if(sorted_filename != NULL)
	{
		FILE* output_fileptr = fopen(sorted_filename,"w");

		if(output_fileptr == NULL)
			perror("File could not be opened for writing\n");

		for(i=0; i<length; i++)
			fprintf(output_fileptr,"%d\n",unsorted[i]);

		fclose(output_fileptr);
	}
	/* if sorted list needs to be outputted to stdout */
	else
	{
		for(i=0; i<length; i++)
			printf("%d\n", unsorted[i]);
	}

	/* free mallocs and files */
	free(unsorted);
	fclose(input_fileptr);

	global_cleanup();

	unsigned long long elapsed_ns;
	elapsed_ns = (time_end.tv_sec-start.tv_sec)*1000000000 + (time_end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

	return 0;
}