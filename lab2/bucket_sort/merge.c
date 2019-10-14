/*****************************************************************************
​ ​* ​ ​ @file​ ​  		merge.c
​ * ​ ​ @brief​ ​ 		Implements merge sort using multithreading
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 24th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include "merge.h"
#include "config.h"
#include<stdio.h>

/*
 * @func_name - myMerge
 * @Description - Perform actual merge of sub-arrays 
 * @params - low - lower indice of sub-array
 *			 mid - mid indice of sub-array
 *			 high - higher indice of sub-array
 * @return - void
 */
void myMerge(int low, int mid, int high)
{
	int low_1, low_2;
	int i;
	int temp_arr[high-low+1];
	int k = 0;

	low_1 = low;
	low_2 = mid+1;

	for(i=low; i<=high; i++)
	{
		if(low_1 > mid)
			temp_arr[k++] = unsorted[low_2++];
		else if(low_2 > high)
			temp_arr[k++] = unsorted[low_1++];
		else if (unsorted[low_1] < unsorted[low_2])
			temp_arr[k++] = unsorted[low_1++];
		else
			temp_arr[k++] = unsorted[low_2++];
	}

	for(i=0; i<k; i++)
	{
		unsorted[low++] = temp_arr[i];
	}
}

/*
 * @func_name - mergeSort
 * @Description - Divide array recursively there is only one element 
 * @params - low - lower indice of sub-array
 *			 high - higher indice of sub-array
 * @return - void
 */
void mergeSort(int low, int high)
{
	int mid;

	if(low < high)
	{
		mid = low + ((high - low)/2);
		mergeSort(low, mid);
		mergeSort(mid+1, high);

		myMerge(low, mid, high);
	}
}


/*
 * @func_name - fj_merge
 * @Description - Handles dividing of array among threads
 * @params - tid - thread id
 * @return - void
 */
void fj_merge(size_t tid)
{
	int low, high, mid, i;

	/* Assign first and last indice to each thread based on tid */
	low = (tid - 1) * (length/NUM_THREADS);
	if(tid == NUM_THREADS) 
	{
		/*Handles case when no. of elements not perfectly divisable by no. of threads
		  Last thread get a high value equal to last element */
		high = length - 1;
	}
	else
		high = ((tid) * (length/NUM_THREADS)) - 1;

	mid = low + ((high - low)/2);

#ifdef DEBUG
	printf("Thread: %lu - low: %d - high: %d - mid: %d\n", tid, low, high, mid);
#endif
	if(low < high)
	{
		mergeSort(low, mid);
		mergeSort(mid+1, high);
		myMerge(low, mid, high);
	}

#ifdef DEBUG
	for(i=low; i<=high; i++)
	{
		printf("T: %ld; No.: %d - %d\n", tid, i, unsorted[i]);
	}
#endif

	/* Wait for all the threads to merge their components */
	pthread_barrier_wait(&bar);

	/* Master thread manages final merge of individual thread contents */
	if(tid==1)
	{
		high = (length-1);
		mid = low + ((high - low)/2);

		mergeSort(low, mid);
		mergeSort(mid+1, high);
		myMerge(low, mid, high);
	
#ifdef DEBUG	
		printf("Sorted List:\n");
		for(i=0; i<length; i++)
		{
			printf("%d\n", unsorted[i]);
		}
#endif
	}
}