#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "process.h"
#include "queue.h"
#include "scheduler.h"
#include "simulation.h"

int main() {
    Process processes[MAX_PROCESSES];
    srand(time(NULL));
    create_process(processes, MAX_PROCESSES);
    print_process(processes, MAX_PROCESSES);

    printf("\n----- FCFS -----\n");
    simulate(processes, MAX_PROCESSES, fcfs, 0, 0);
    

    printf("\n----- SJF -----\n");
    simulate(processes, MAX_PROCESSES, npsjf, 0, 0);   

    printf("\n----- PSJF -----\n");
    simulate(processes, MAX_PROCESSES, psjf, 1, 0); 


    printf("\n----- Priority -----\n");
    simulate(processes, MAX_PROCESSES, priority, 0, 0);

    printf("\n----- PPriority -----\n");
    simulate(processes, MAX_PROCESSES, ppriority, 1, 0);


    printf("\n----- RR (Q=2) -----\n");
    simulate(processes, MAX_PROCESSES, round_robin, 1, 2);

    return 0;
}
