/*****************************************************************************
​ ​* ​ ​ @file​ ​  		bucket_sort.c
​ * ​ ​ @brief​ ​ 		Implements bucket sort using multithreading
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 24th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include "bucket_sort.h"
#include "config.h"
#include<set>
#include<stdio.h>
#include<iterator>

using namespace std;

/*
 * @func_name - bucket
 * @Description - Bucket sort using multisets
 * @params - tid - thread id
 * @return - void
 */
void bucket(size_t tid)
{
	int bucketNum;
	int numBuckets = NUM_THREADS;
	multiset<int> bucket;
	int hashval, i;

	incrementor1 = 1;
	incrementor2 = 0;

	/* Calculate which bucket(thread) element gets assigned to. 
	   If TID matches the calculated value, assign to that bucket(thread) */
	for(i=0; i<length; i++)
	{
		hashval = (numBuckets * unsorted[i])/(max_unsorted + 1) + 1;

		if(tid == hashval)
		{
			bucket.insert(unsorted[i]);
		}
	}

	multiset<int>::iterator itr;	
#ifdef DEBUG
	for(itr = bucket.begin(); itr != bucket.end(); itr++)
	{
		printf("thread: %lu; element: %d\n", tid, *itr);
	}
#endif

	/* Wait for all threads to insert their elements into buckets */
	pthread_barrier_wait(&bar);

	/* Following while loop ensures all threads are concatenated in order of their TID 
	   Note: incrementor1 has to be volatile to ensure that compiler optimatizations (Ofast) 
	   allows it to run */
	while(incrementor1<=numBuckets)
	{
		if((tid) == incrementor1)
		{
			for(itr = bucket.begin(); itr != bucket.end(); itr++)
			{	
				unsorted[incrementor2++] = *itr;
			}
			incrementor1++; 
			break;
		}
	}
}