#include <stdio.h>
#include <stdlib.h>
#include "process.h"

void create_process(Process *process_list, int num)
{
    for (int i = 0; i < num; i++)
    {
        process_list[i].pid = i + 1;
        process_list[i].priority = (rand() % MAX_PRIORITY) + 1;
        process_list[i].state = STATE_NEW;
        process_list[i].arrival = rand() % MAX_ARRIVAL_TIME;
        process_list[i].cpu_burst = (rand() % MAX_CPU_BURST_TIME) + 1;
        if (process_list[i].cpu_burst == 1) {
            process_list[i].io_burst = 0;
        } else {
            process_list[i].io_burst = rand() % MAX_IO_BURST_TIME;
        }
        if (process_list[i].io_burst == 0) {
            process_list[i].io_request = 0;
        } else {
            process_list[i].io_request = rand() % (process_list[i].cpu_burst - 1) + 1;
        }
        process_list[i].remaining = process_list[i].cpu_burst;
        process_list[i].io_remaining = process_list[i].io_burst;
    }
}

void print_process(Process *process_list, int num)
{
    printf("PID\tPRI\tAT\tCBT\tIOBT\tIORT\n");
    for (int i = 0; i < num; i++)
    {
        printf("%d\t%d\t%d\t%d\t%d\t%d\n",
               process_list[i].pid,
               process_list[i].priority,
               process_list[i].arrival,
               process_list[i].cpu_burst,
               process_list[i].io_burst,
               process_list[i].io_request);
    }
}
