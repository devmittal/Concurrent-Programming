/*****************************************************************************
​ ​* ​ ​ @file​ ​  		lab3.c
​ * ​ ​ @brief​ ​ 		Parallelize merge sort using openMP
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		October 25th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include<stdio.h>
#include<getopt.h>
#include<omp.h>
#include<stdlib.h>
#include<time.h>

//#define DEBUG

#define MAX_INT_SIZE 10 //max number of digits in integer

/*
 * @func_name - merge
 * @Description - Merge sort array
 * @params - int *elements - pointer to unsorted array
 			 int start - starting index of unsorted array
 			 int mid - middle index of unsorted array
 			 int end - last index of unsorted array
 * @return - void
 */
void merge(int *elements, int start, int mid, int end)
{
	int low_1, low_2;
	int i;
	int temp_arr[end-start+1];
	int k = 0;

	low_1 = start;
	low_2 = mid+1;

	for(i=start; i<=end; i++)
	{
		if(low_1 > mid)
			temp_arr[k++] = elements[low_2++];
		else if(low_2 > end)
			temp_arr[k++] = elements[low_1++];
		else if(elements[low_1] < elements[low_2])
			temp_arr[k++] = elements[low_1++];
		else
			temp_arr[k++] = elements[low_2++];
	}

	for(i=0; i<k; i++)
	{
		elements[start++] = temp_arr[i];
	}
}

/*
 * @func_name - merge_sort
 * @Description - Divide unsorted array. Parallelized using openMP
 * @params - int *elements - pointer to unsorted array
 			 int start - starting index of unsorted array - 0
 			 int end - last index of unsorted array
 * @return - void
 */
void merge_sort(int *elements, int start, int end)
{
	int mid;

	if(start < end)
	{
		mid = (start + end)/2;

		/* Sort the 2 recursive section in parallel using OpenMP */
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				merge_sort(elements, start, mid);
			}
			#pragma omp section
			{
				merge_sort(elements, (mid+1), end);
			}
		}
		merge(elements, start, mid, end);
	}
}

int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	int index;
	char *sorted_filename = NULL;
	char *unsorted_filename;
	int *unsorted = NULL;
	int i = 0;
	int length = 0;
	char int_string[MAX_INT_SIZE];
	struct timespec start, time_end;

	static struct option long_options[] = {
		{"name", no_argument, 0, 'n'},      //Optional arg - print name
		{"o",    required_argument, 0, 'o'},//Optional arg - output sorted list to file
		{0     , 0          , 0,  0 }
	};

	/* Loop to get all otpional arguments */
	while((c = getopt_long(argc, argv, "no:", long_options, &option_index)) != -1)
	{
		/* Handle Optional Arguments */		
		switch(c)
		{
			case 'n': printf("Devansh Mittal\n");
					  return 0; //Return after name is printed
					  break;

			case 'o': sorted_filename = optarg;
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

	/* Allocate heap space for unsorted and sorted arrays */
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
		fgets(int_string, MAX_INT_SIZE, input_fileptr);
		unsorted[i++] = atoi(int_string); //Convert string read to integer
	}

	clock_gettime(CLOCK_MONOTONIC,&start);
	merge_sort(unsorted, 0, length-1);	
	clock_gettime(CLOCK_MONOTONIC,&time_end);

	/* If sorted list needs to be outputted to file */
	if(sorted_filename != NULL)
	{
		FILE* output_fileptr = fopen(sorted_filename,"w");

		if(output_fileptr == NULL)
			perror("File could not be opened for writing\n");

		for(i=0; i<length; i++)
			fprintf(output_fileptr,"%d\n",unsorted[i]);
	}
	/* if sorted list needs to be outputted to stdout */
	else
	{
		for(i=0; i<length; i++)
			printf("%d\n", unsorted[i]);
	}

	/* Free dynamically allocated arrays */
	free(unsorted);
	fclose(input_fileptr);

	#ifdef DEBUG
	unsigned long long elapsed_ns;
	elapsed_ns = (time_end.tv_sec-start.tv_sec)*1000000000 + (time_end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
	#endif
	
	return 0;
}