#include<stdio.h>
#include "locks_barriers.h"

void ticket_lock(void)
{
	int my_num = ticket_next_num.fetch_add(1);
	while(ticket_now_serving.load() != my_num);
}

void ticket_unlock(void)
{
	ticket_now_serving.fetch_add(1);
}

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

void mcs_lock(struct Node* myNode)
{
	struct Node* oldTail = tail.load();
	
	//myNode->next.store(NULL, memory_order_relaxed);
	
	myNode->next = NULL;

	while(!tail.compare_exchange_strong(oldTail,myNode))
	{
		oldTail = tail.load();
	}

	if(oldTail != NULL)
	{
		myNode->wait.store(true, memory_order_relaxed);
		//oldTail->next.store(myNode);
		oldTail->next = myNode; 

		while(myNode->wait.load());
	}
}

void mcs_unlock(struct Node* myNode)
{
	if(tail.compare_exchange_strong(myNode, NULL))
	{
		//No thread is waiting. Just free the lock
	}
	else
	{
		//while(myNode->next.load() == NULL);
		while(myNode->next == NULL);
		myNode->next->wait.store(false);//Facing issue here. Atomic variable cannot dereference. For now made next non atomic and works fine.
	}
}