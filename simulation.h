#ifndef SIMULATION_H
#define SIMULATION_H

#include "process.h"
#include "scheduler.h"

#define MAX_TICKS 200

void reset_processes(Process* process_list, int num, Queue* jq);
void simulate(Process* processes, int num, alg pick, int preemptive, int quantum);

#endif