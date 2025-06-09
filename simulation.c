#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "simulation.h"
#include "scheduler.h"
#include "process.h"

void reset_processes(Process* process_list, int num, Queue* jq) {
    for (int i = 0; i < num; i++) {
        process_list[i].state = STATE_NEW;
        process_list[i].remaining = process_list[i].cpu_burst;
        process_list[i].io_remaining = 0;
        process_list[i].start = -1;
        process_list[i].end = -1;
        process_list[i].waiting = 0;
        process_list[i].turnaround = 0;
        process_list[i].entered_ready_at = -1; // RR
        enqueue(jq, &process_list[i]);
    }
}

void simulate(Process* process_list, int num, alg pick, int preemptive, int quantum) {
    
    char gantt[MAX_TICKS][8]; 
    int time_log[MAX_TICKS + 1];
    int block_count = 0;
    char last_label[8] = "";  
    char label[8] = "idle";
    int completed = 0, tick = 0, consumed = 0;
    int running_to_NULL = 0;
    int total_tat = 0, total_wt = 0;
    double avg_tat, avg_wt;

    Queue jq, rq, wq, wait_done;
    init_queue(&jq); // Job Queue
    init_queue(&rq); // Ready Queue
    init_queue(&wq); // Waiting Queue
    init_queue(&wait_done); // Wait done Queue

    reset_processes(process_list, num, &jq);

    Process* running = NULL;

    for (tick = 0; tick < MAX_TICKS && completed < num; tick++) {

        printf("Time %d ~ %d:", tick, tick + 1);

        // 1. 도착한 프로세스는 Job Queue → Ready Queue
        int jq_size = jq.count;
        for (int i = 0; i < jq_size; i++) {
            Process* p = dequeue(&jq);
            if (p->arrival == tick) {
                p->state = STATE_READY;
                p->entered_ready_at = tick; // RR에서 ready 상태로 들어온 시간 (이번 tick에 도착한 경우)
                enqueue(&rq, p); 
            } else if (p->arrival > tick) {
                enqueue(&jq, p); 
            }
        }

        // 2. Waiting Queue → Ready Queue (I/O 완료는 도착한 프로세스보다 늦게 반영)
        int wq_size = wq.count;
        for (int i = 0; i < wq_size; i++) {
            Process* p = dequeue(&wq);

            // I/O 시간 감소
            p->io_remaining--;

            if (p->state == STATE_WAITING && p->io_remaining > 0) {
                printf("(P%d I/O processing)", p->pid);
                enqueue(&wq, p);  // I/O 계속 진행
            } else {
                p->state = STATE_READY;
                printf(" (P%d I/O processing) -> I/O done", p->pid);
                enqueue(&wait_done, p);  // I/O 완료, Wait done으로 이동 (I/O 끝난 process가 동일 tick에 run 되지 않도록)
            }
        }

        // 3. schedular가 next process pick
        if (preemptive || running == NULL) {
            Process* next = pick(running, &rq);  
            if (next != running) {
                if (running != NULL && running->state == STATE_RUNNING) {
                    running->state = STATE_READY;
                    running->entered_ready_at = tick; // RR에서 ready 상태로 들어온 시간 (지난 tick까지 실행되다가 끝난 경우)
                    enqueue(&rq, running);
            }
                running = next;
                consumed = 0;
                if (running != NULL) {
                    if (running->start == -1) {
                        running->start = tick;
                    }
                    running->state = STATE_RUNNING; 
                }
            }
        }

        // 4. 실행
        if (running != NULL) {
            printf(" (P%d running)", running->pid);

            running->remaining--;
            consumed++;

            if (running->remaining == 0) { // 프로세스 종료
                running->end = tick + 1;
                running->state = STATE_TERMINATED;
                running_to_NULL = 1;
                completed++;
                printf(" -> terminated (completed %d/%d)", completed, num);
            } else if (running->cpu_burst - running->remaining == running->io_request) { // I/O 시작
                running->state = STATE_WAITING;
                running->io_remaining = running->io_burst;
                enqueue(&wq, running);
                printf(" -> I/O start");
                running_to_NULL = 1;
            } else if (quantum > 0 && consumed == quantum) { // RR quantum 만료
                running->entered_ready_at = tick + 1; // RR에서 ready 상태로 들어온 시간 (이번 tick에 quantum 만료로 ready 상태로 들어온 경우)
                running->state = STATE_READY;
                enqueue(&rq, running);
                printf(" -> time slice expired");
                running_to_NULL = 1;
            } 
        } else {
                printf(" idle"); 
        }     

        while (!is_empty(&wait_done)) { // I/O 완료된 프로세스 처리
            Process* p = dequeue(&wait_done);
            if (preemptive) p->state = STATE_IO_RETURNED; // preempt 하지 못하도록 ready 상태와 구분하여 저장
            else p->state = STATE_READY;
            p->entered_ready_at = tick + 1;
            enqueue(&rq, p);
        }



        // 5. Gantt 차트 출력
        if (running_to_NULL == 1) {
            sprintf(label, "P%d", running->pid);
            running = NULL;
            running_to_NULL = 0;
        } else if (running == NULL) {
            sprintf(label, "idle");
        } else {
            sprintf(label, "P%d", running->pid);
        }
        if (strcmp(label, last_label) != 0) {
            strcpy(gantt[block_count], label);
            time_log[block_count] = tick;
            block_count++;
            strcpy(last_label, label);
        }

        printf("\n");
    }

    printf("Simulation End at Time %d\n", tick);

    time_log[block_count] = tick;

    printf("\nGantt Chart:\n");

    // |PID| 형태 출력
    for (int i = 0; i < block_count; i++) {
        if (strcmp(gantt[i], "idle") == 0) printf("| %s ", gantt[i]);
        else printf("|  %s  ", gantt[i]);
    }
    printf("|\n");

    // 시간 출력
    for (int i = 0; i < block_count; i++) {
        printf("%-7d", time_log[i]);
    }
    printf("%d\n", time_log[block_count]);


    // 6. 프로세스 평가
    printf("\nProcess Evaluation:\n");

    for (int i = 0; i < num; i++) {
        process_list[i].turnaround = process_list[i].end - process_list[i].arrival;
        process_list[i].waiting = process_list[i].turnaround - process_list[i].cpu_burst - process_list[i].io_burst;
        total_tat += process_list[i].turnaround;
        total_wt += process_list[i].waiting;

        printf("P%d: TAT = %d, WT = %d, Start = %d, End = %d\n", 
               process_list[i].pid, process_list[i].turnaround, process_list[i].waiting, process_list[i].start, process_list[i].end);
    }

    avg_tat = (double)total_tat / num;
    avg_wt = (double)total_wt / num;

    printf("\nAverage Turnaround Time: %.2f\n", avg_tat);
    printf("Average Waiting Time: %.2f\n", avg_wt);                                                                                                                                             
}

//io 시간이 turn around 에는 포함, waiting 에서는 제외