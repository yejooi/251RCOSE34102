#include <stdio.h>
#include "scheduler.h"

Process* fcfs(Process* running, Queue* rq) {
    if (running != NULL) return running;
    if (is_empty(rq)) return NULL;

    int min_idx = -1;
    int start = rq->front;

    for (int i = 0; i < rq->count; i++) { // ready queue에서 가장 먼저 도착한 프로세스 선택, 도착 시간 같을시 pid 작은 순서
        int idx = (start + i) % QUEUE_SIZE;
        Process* p = rq->data[idx];

        if (min_idx == -1 || p->arrival < rq->data[min_idx]->arrival || 
            (p->arrival == rq->data[min_idx]->arrival && p->pid < rq->data[min_idx]->pid)) { // 도착 시간 같으면 pid가 작은 순서
            min_idx = idx;
        }
    }

    return dequeue_by_index(rq, min_idx);
} 


Process* pick_shortest(Queue* rq, int preempt, Process* running) {
    if (is_empty(rq)) return running;

    int min_idx = -1;
    int min_val = MAX_CPU_BURST_TIME;
    int start = rq->front;
    int metric = 0;

    if (!preempt && running != NULL) return running; // Non-preemptive인 경우 running이 있으면 그대로 반환

    for (int i = 0; i < rq->count; i++) {
        int idx = (start + i) % QUEUE_SIZE;
        Process* p = rq->data[idx];

        if (p->state == STATE_IO_RETURNED) {
            if (preempt && running != NULL) continue; // 새로 도착한 게 아닌 I/O가 완료되어 들어온 프로세스는 preempt할 수 없음
            else p->state = STATE_READY;
        }

        metric = preempt ? p->remaining : p->cpu_burst; // preempt가 true면 remaining time, false면 cpu burst time 사용

        if (min_idx == -1 || metric < min_val || // 현재 프로세스의 metric이 최소값보다 작거나 같을 때
            (metric == min_val && p->pid < rq->data[min_idx]->pid)) { // metric이 같을 때 pid가 작은 순서
            min_idx = idx;
            min_val = metric;
        }
    }

    if (min_idx == -1) return running;

    if (preempt && running != NULL && running->remaining <= min_val) return running;
    
    return dequeue_by_index(rq, min_idx);
}

Process* npsjf(Process* running, Queue* rq) { // Non-preemptive
    return pick_shortest(rq, 0, running);
}

Process* psjf(Process* running, Queue* rq) { // Preemptive
    return pick_shortest(rq, 1, running);
}


Process* pick_priority(Queue* rq, int preempt, Process* running) {
    if (is_empty(rq)) return running;

    int min_idx = -1;
    int min_pri = MAX_PRIORITY + 1;
    int start = rq->front;

    if (!preempt && running != NULL) return running;

    for (int i = 0; i < rq->count; i++) {
        int idx = (start + i) % QUEUE_SIZE;
        Process* p = rq->data[idx];

        if (p->state == STATE_IO_RETURNED) {
            if (preempt && running != NULL) continue; 
            else p->state = STATE_READY;
        }

        if (min_idx == -1 || p->priority < rq->data[min_idx]->priority ||
            (p->priority == rq->data[min_idx]->priority && // 우선순위가 같을 때 pid가 작은 순서
            p->pid < rq->data[min_idx]->pid)) {
            min_idx = idx;
            min_pri = p->priority;
        }
    }

    if (min_idx == -1) return running;

    if (preempt && running != NULL && running->priority <= rq->data[min_idx]->priority) return running;
    
    return dequeue_by_index(rq, min_idx);
}

Process* priority(Process* running, Queue* rq) { // Non-preemptive
    return pick_priority(rq, 0, running);
}

Process* ppriority(Process* running, Queue* rq) { // Preemptive
    return pick_priority(rq, 1, running);
}

Process* round_robin(Process* running, Queue* rq) {
    if (running != NULL) return running;
    if (is_empty(rq)) return NULL;

    int min_idx = -1;
    int start = rq->front;

    for (int i = 0; i < rq->count; i++) { // ready queue에서 가장 먼저 들어온 프로세스 선택, 도착 시간 같을시 pid 작은 순서
        int idx = (start + i) % QUEUE_SIZE;
        Process* p = rq->data[idx];

        if (min_idx == -1 ||
            (p->entered_ready_at < rq->data[min_idx]->entered_ready_at) || 
            // entered_ready_at이 더 빠른 프로세스 선택 (새로 도착한 프로세스 뿐 아니라 I/O 처리 후 또는 quantum 만료 후 돌아온 프로세스도 고려하여 동일한 시간에 ready queue에 들어온 프로세스 중에서 선택)
            (p->entered_ready_at == rq->data[min_idx]->entered_ready_at && p->pid < rq->data[min_idx]->pid)) {
            min_idx = idx;
        }
    }

    return dequeue_by_index(rq, min_idx);
}