/*****************************************************************************
​ ​* ​ ​ @file​ ​  		lab0.c
​ * ​ ​ @brief​ ​ 		Print name or merge sort to stdout or file based on cmd 
 *					line arguments
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 5th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>

#define MAX_INT_SIZE 10 //max number of digits in integer

/*
 * @func_name - merge_sort
 * @Description - Merge sort unsorted array
 * @params - int *unsorted - pointer to unsorted array
 			 int *sorted - pointer to array where sorted list is to be stored
 			 int length - length of unsorted array
 * @return - void
 */
void merge_sort(int *unsorted, int *sorted, int length)
{
	int i,j,k,l,low1,high1,low2,high2;
	int *temp; 

	temp = (int *)malloc(sizeof(int) * length); //Allocate heap space for temp array for merge sort

	for(i=1;i < length; i=i*2)
	{
		low1 = 0;
		k=0;
		while(low1+i < length)
		{
			high1=low1+i-1;
			low2=high1+1;
			high2=low2+i-1;

			if(high2 >= length)
				high2 = length-1;

			l=low1;
			j=low2;

			while(l<=high1 && j<=high2)
			{
				if(unsorted[l] >= unsorted[j])
					temp[k++] = unsorted[l++];
				else
					temp[k++] = unsorted[j++];
			}

			while(l<=high1)
				temp[k++] = unsorted[l++];

			while(j<=high2)
				temp[k++] = unsorted[j++];

			low1 = high2 + 1;
		}

		for(l=low1; k<length; l++)
			temp[k++] = unsorted[l];

		for(l=0;l<length;l++)
			unsorted[l] = temp[l];
	}

	free(temp);

	j = 0;
	for(i=(length-1); i>=0; i--)
	{
		sorted[j++] = unsorted[i];
	}
}

int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	int index;
	char *sorted_filename = NULL;
	char *unsorted_filename;
	int *sorted = NULL;
	int *unsorted = NULL;
	int i = 0;
	int length = 0;
	char int_string[MAX_INT_SIZE];

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

	sorted = (int *)malloc(sizeof(int) * (length+1));
	if(sorted == NULL)
	{
		perror("Unable to allocate heap for sorted\n");
	}

	/* Go back to beginning of file */
	if(fseek(input_fileptr, 0, SEEK_SET) != 0)
		perror("Unable to go back to beginning of file\n");

	while(!feof(input_fileptr)) //Read integers till EOF
	{
		fgets(int_string, MAX_INT_SIZE, input_fileptr);
		unsorted[i++] = atoi(int_string); //Convert string read to integer
	}

	merge_sort(unsorted, sorted, length);	

	/* If sorted list needs to be outputted to file */
	if(sorted_filename != NULL)
	{
		FILE* output_fileptr = fopen(sorted_filename,"w");

		if(output_fileptr == NULL)
			perror("File could not be opened for writing\n");

		for(i=0; i<length; i++)
			fprintf(output_fileptr,"%d\n",sorted[i]);
	}
	/* if sorted list needs to be outputted to stdout */
	else
	{
		for(i=0; i<length; i++)
			printf("%d\n", sorted[i]);
	}

	/* Free dynamically allocated arrays */
	free(sorted);
	free(unsorted);

	fclose(input_fileptr);

	return 0;
}