/*****************************************************************************
​ ​* ​ ​ @file​ ​  		lab1.c
​ * ​ ​ @brief​ ​ 		Increment counter based on locks/barriers
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		October 16th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<pthread.h>
#include<assert.h>
#include "locks_barriers.h"

size_t NUM_ITERATIONS;
char *barr;
char *lock;
pthread_t* threads;
size_t* args;
pthread_barrier_t bar;
pthread_barrier_t timing_bar; //barrier used to sync threads to calc. time
pthread_mutex_t inbuilt_lock;
int cntr;//cntr which will be incremented
struct timespec start, time_end;

/* extern variables */
atomic<bool> tas_lock;
atomic<int> ticket_next_num;
atomic<int> ticket_now_serving;
atomic<int> sense_barr_cnt;
atomic<int> sense_barr_sense;
atomic<struct Node*> tail;
size_t NUM_THREADS;

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
	pthread_barrier_init(&timing_bar, NULL, NUM_THREADS);

	/* Init barrier only if selected */
	if((barr != NULL) && (!strcmp(barr,"pthread")))
		pthread_barrier_init(&bar, NULL, NUM_THREADS);

	/* Init mutex only if selected */
	if((lock != NULL) && (!strcmp(lock,"pthread")))
		pthread_mutex_init(&inbuilt_lock,NULL);
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
	pthread_barrier_destroy(&timing_bar);

	if((barr != NULL) && (!strcmp(barr,"pthread")))
		pthread_barrier_destroy(&bar);
	
	if((lock != NULL) && (!strcmp(lock,"pthread")))
		pthread_mutex_destroy(&inbuilt_lock);
}

/*
 * @func_name - inc_cntr
 * @Description - increment counter using lock/barrier specified
 * @params - tid - thread id
 * @return - void
 */
void inc_cntr(size_t tid)
{
	/* pthread locks */
	if((lock != NULL) && (!strcmp(lock,"pthread")))
	{
		for(int i=0; i<NUM_ITERATIONS; i++)
		{
			pthread_mutex_lock(&inbuilt_lock);
			cntr++;
			pthread_mutex_unlock(&inbuilt_lock);
		}
	}

	/* pthread barriers */
	else if((barr != NULL) && (!strcmp(barr,"pthread")))
	{
		for(int i=0; i<(NUM_ITERATIONS*NUM_THREADS); i++)
		{
			if((i%NUM_THREADS)==(tid-1))
			{
				cntr++;
			}
			pthread_barrier_wait(&bar);
		}
	}

	/* tas */
	else if((lock != NULL) && (!strcmp(lock,"tas")))
	{
		bool expected, changed;
		for(int i = 0; i<NUM_ITERATIONS; i++)
		{
			do
			{
				expected = false;
				changed = true;
			}while(!tas_lock.compare_exchange_strong(expected,changed));
			cntr++;
			tas_lock.store(false);
		}
	}

	/* ttas */
	else if((lock != NULL) && (!strcmp(lock,"ttas")))
	{
		bool expected, changed;
		for(int i = 0; i<NUM_ITERATIONS; i++)
		{
			do
			{
				expected = false;
				changed = true;
			}while(tas_lock.load()==true || !tas_lock.compare_exchange_strong(expected,changed));
			cntr++;
			tas_lock.store(false);
		}
	}

	/* ticket */
	else if((lock != NULL) && (!strcmp(lock,"ticket")))
	{
		for(int i = 0; i<NUM_ITERATIONS; i++)
		{
			ticket_lock();
			cntr++;
			ticket_unlock();
		}
	}

	/* sense reversal barrier */
	else if((barr != NULL) && (!strcmp(barr,"sense")))
	{
		for(int i=0; i<(NUM_ITERATIONS*NUM_THREADS); i++)
		{
			if((i%NUM_THREADS)==(tid-1))
			{
				cntr++;
			}
			barrier_wait();
		}
	}

	/* MCS lock */
	else if((lock != NULL) && (!strcmp(lock,"mcs")))
	{
		struct Node* myNode;

		myNode = (struct Node*)malloc(sizeof(struct Node)); //allocated node for each thread

		for(int i = 0; i<NUM_ITERATIONS; i++)
		{
			mcs_lock(myNode);
			cntr++;
			mcs_unlock(myNode);
		}

		free(myNode); //free allocated node
	}

	else
	{
		printf("Incorrect lock/barrier entered. Try again!\n");
	}
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

	pthread_barrier_wait(&timing_bar);

	inc_cntr(tid);

	pthread_barrier_wait(&timing_bar);

	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&time_end);
	}
}

int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	int index;
	char *output_filename = NULL;
	barr = NULL;
	lock = NULL;
	cntr = 0;
	
	/* Initialize extern variables */
	ticket_next_num = 0;
	ticket_now_serving = 0;
	sense_barr_cnt = 1;
	sense_barr_sense = 0;
	NUM_THREADS = 12;
	tail = NULL;

	static struct option long_options[] = {
		{"name", no_argument, 0, 'n'},      //Optional arg - print name
		{"o",    required_argument, 0, 'o'},//Optional arg - output sorted list to file
		{"t",    required_argument, 0, 't'},//Optional arg - specify number of threads
		{"i",    required_argument, 0, 'i'},//Optional arg - No. of iterations
		{"bar",  required_argument, 0, 'b'},//Optional arg - Barrier to use
		{"lock", required_argument, 0, 'l'},//Optional arg - lock to use
		{0     , 0          , 0,  0 }
	};

	/* Loop to get all otpional arguments */
	while((c = getopt_long(argc, argv, "no:t:i:b:l:", long_options, &option_index)) != -1)
	{
		/* Handle Optional Arguments */		
		switch(c)
		{
			case 'n': printf("Devansh Mittal\n");
					  return 0; //Return after name is printed
					  break;

			case 'o': output_filename = optarg;
					  break;

			case 't': NUM_THREADS = atoi(optarg);
					  break;

			case 'i': NUM_ITERATIONS = atoi(optarg);
					  break;

			case 'b': barr = optarg;
					  break;

			case 'l': lock = optarg;
					  break;

			default: printf("No optional arguments\n");
		}
	}

	/* Limit number of threads to 50 */
	if(NUM_THREADS > 50)
	{
		printf("Too many threads: limited to 50\n");
		NUM_THREADS = 50;
	}

	/* Condition to check if both lock and barrier specified. Give errior */
	if(barr != NULL && lock != NULL)
	{
		printf("Both lock and barrier cannot be specified. Run program again specifying only 1.\n");
		exit(-1);
	}

	/* Check if output filename not specified */
	if(output_filename == NULL)
	{
		printf("Output file not specified. Run program again specifying output filename\n");
		exit(-1);
	}

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

	FILE* output_fileptr = fopen(output_filename,"w");

	if(output_fileptr == NULL)
		perror("File could not be opened for writing\n");

	fprintf(output_fileptr,"%d\n",cntr);

	fclose(output_fileptr);

	/* Check if incremented counter value matches expected value */
	assert(cntr == (NUM_THREADS*NUM_ITERATIONS));

	global_cleanup();

	unsigned long long elapsed_ns;
	elapsed_ns = (time_end.tv_sec-start.tv_sec)*1000000000 + (time_end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

	return 0;
}