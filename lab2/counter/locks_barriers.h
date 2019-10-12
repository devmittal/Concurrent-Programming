#ifndef LOCKS_BARRIERS_H
#define LOCKS_BARRIERS_H

#include<atomic>

using namespace std;

struct Node{
	atomic<struct Node*> next;
	atomic<bool> wait;
};

extern atomic<struct Node*> tail;  
extern atomic<int> ticket_next_num;
extern atomic<int> ticket_now_serving;
extern atomic<int> sense_barr_cnt;
extern atomic<int> sense_barr_sense;
extern size_t NUM_THREADS;

void ticket_lock(void);
void ticket_unlock(void);
void barrier_wait(void);
void mcs_lock(struct Node* myNode);
void mcs_unlock(struct Node* myNode);

#endif /* LOCKS_BARRIERS_H */