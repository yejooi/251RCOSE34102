#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 4
#define MAX_PRIORITY 5
#define MAX_ARRIVAL_TIME 10
#define MAX_CPU_BURST_TIME 10
#define MAX_IO_BURST_TIME 3

#define STATE_NEW 0
#define STATE_READY 1
#define STATE_RUNNING 2
#define STATE_WAITING 3
#define STATE_TERMINATED 4
#define STATE_IO_RETURNED 5

typedef struct {
    int pid;
    int priority;
    int state; // 0: new, 1: ready, 2: running, 3: waiting, 4: terminated
    int arrival; // 변수 이름에 time 생략
    int cpu_burst;
    int io_burst;
    int io_request;
    int remaining;
    int io_remaining;
    int start;
    int end;
    int waiting;
    int turnaround;
    int entered_ready_at; // ready queue로 들어간 시간 (RR)
} Process;

void create_process(Process* process_list, int num);
void print_process(Process* process_list, int num);
#endif
