#include <stdio.h>
#include "queue.h"

void init_queue(Queue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int is_empty(Queue* q) {
    return q->count == 0;
}

int is_full(Queue* q) {
    return q->count == QUEUE_SIZE;
}

void enqueue(Queue* q, Process* p) {
    if(is_full(q)) {
        printf("Queue overflow\n");
        return;
    }
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->data[q->rear] = p;
    q->count++;
}

Process* dequeue(Queue* q) {
    if(is_empty(q)) {
        printf("Queue underflow\n");
        return NULL;
    }
    Process* p = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    return p;
}

Process* dequeue_by_index(Queue* q, int idx) {
    if (idx == q->front) return dequeue(q);

    Process* selected = q->data[idx];
    for (int i = idx; i != q->front; i = (i - 1 + QUEUE_SIZE) % QUEUE_SIZE) {
        int prev = (i - 1 + QUEUE_SIZE) % QUEUE_SIZE;
        q->data[i] = q->data[prev];
    }
    q->data[q->front] = selected;
    return dequeue(q);
}