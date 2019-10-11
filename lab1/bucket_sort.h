/*****************************************************************************
​ ​* ​ ​ @file​ ​  		bucket_sort.h
​ * ​ ​ @brief​ ​ 		Header file containing global variables/function declarations
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		September 24th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#ifndef BUCKET_H
#define BUCKET_H

#include<stddef.h>

static volatile int incrementor1;
static int incrementor2;

void bucket(size_t tid);

#endif /* BUCKET_H */