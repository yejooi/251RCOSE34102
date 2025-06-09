#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "queue.h"

typedef Process* (*alg) (Process* running, Queue* ready_queue);

Process* fcfs(Process* running, Queue* ready_queue);

Process* pick_shortest(Queue* rq, int preempt, Process* running);
Process* npsjf(Process* running, Queue* rq); // 버스트 타임 똑같을 때 pid 고려
Process* psjf(Process* running, Queue* rq); // IO가 끝난 프로세스도 실행 중인 프로세스를 preempt 하는 문제

Process* pick_priority(Queue* rq, int allow_preempt, Process* running);
Process* priority(Process* running, Queue* rq);
Process* ppriority(Process* running, Queue* rq);

Process* round_robin(Process* running, Queue* rq);

#endif
