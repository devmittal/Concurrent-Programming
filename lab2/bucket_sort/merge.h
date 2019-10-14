/*****************************************************************************
​ ​* ​ ​ @file​ ​  		merge.h
​ * ​ ​ @brief​ ​ 		Header file containing function declarations
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 24th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#ifndef MERGE_H
#define MERGE_H

#include<stddef.h>

void myMerge(int low, int mid, int high);
void mergeSort(int low, int high);
void fj_merge(size_t tid);

#endif /* MERGE_H */