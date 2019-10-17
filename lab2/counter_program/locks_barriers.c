/*****************************************************************************
​ ​* ​ ​ @file​ ​  		locks_barriers.c
​ * ​ ​ @brief​ ​ 		Contains lock/barrier implementation
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		October 16th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include<stdio.h>
#include "locks_barriers.h"

/*
 * @func_name - ticket_lock
 * @Description - Ticket lock implemetation
 * @params - void
 * @return - void
 */
void ticket_lock(void)
{
	int my_num = ticket_next_num.fetch_add(1);
	while(ticket_now_serving.load() != my_num);
}

/*
 * @func_name - ticket_unlock
 * @Description - Ticket unlock implementation
 * @params - void
 * @return - void
 */
void ticket_unlock(void)
{
	ticket_now_serving.fetch_add(1);
}

/*
 * @func_name - barrier_wait
 * @Description - Barrier wait implementation
 * @params - void
 * @return - void
 */
void barrier_wait(void)
{
	thread_local bool my_sense = 0;

	my_sense ? my_sense = 0 : my_sense = 1;

	int cnt_cpy = sense_barr_cnt.fetch_add(1);

	if(cnt_cpy == NUM_THREADS)
	{
		sense_barr_cnt.store(1, memory_order_relaxed);
		sense_barr_sense.store(my_sense);
	}
	else
	{
		while(sense_barr_sense.load()!=my_sense);
	}
}

/*
 * @func_name - mcs_lock
 * @Description - mcs lock implementation
 * @params - struct Node* myNode - Structure node to be locked
 * @return - void
 */
void mcs_lock(struct Node* myNode)
{
	struct Node* oldTail = tail.load();
	
	myNode->next.store(NULL, memory_order_relaxed);

	while(!tail.compare_exchange_strong(oldTail,myNode))
	{
		oldTail = tail.load();
	}

	if(oldTail != NULL)
	{
		myNode->wait.store(true, memory_order_relaxed);
		oldTail->next.store(myNode);

		while(myNode->wait.load());
	}
}

/*
 * @func_name - mcs_unlock
 * @Description - mcs unlock implementation
 * @params - struct Node* myNode - Structure node to be unlocked
 * @return - void
 */
void mcs_unlock(struct Node* myNode)
{
	struct Node* tempNode = myNode;

	if(tail.compare_exchange_strong(tempNode, NULL))
	{
		//No thread is waiting. Just free the lock
	}
	else
	{
		while(myNode->next.load() == NULL);
		myNode->next.load()->wait.store(false);
	}
}