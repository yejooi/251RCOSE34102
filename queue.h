#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

#define QUEUE_SIZE MAX_PROCESSES

typedef struct {
    Process* data[QUEUE_SIZE];
    int front;
    int rear;
    int count;
} Queue;

void init_queue(Queue* q);
int is_empty(Queue* q);
int is_full(Queue* q);
void enqueue(Queue* q, Process* p);
Process* dequeue(Queue* q);
Process* dequeue_by_index(Queue* q, int idx);

#endif
