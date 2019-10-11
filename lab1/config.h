/*****************************************************************************
​ ​* ​ ​ @file​ ​  		config.c
​ * ​ ​ @brief​ ​ 		Contains variables needed by all 3 source files
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 24th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include<pthread.h>

/* uncomment to print debug statements */
//#define DEBUG

extern int *unsorted;
extern int length; //Number of integers in file
extern size_t NUM_THREADS;
extern pthread_barrier_t bar;
extern int max_unsorted;

#endif /* CONFIG_H */