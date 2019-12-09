/*
 * pcb.h
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "queue.h"
#include "lock.h"
#include "schedule.h"
#include "thread.h"


enum Status{ READY, BLOCKED, FINISHED, NEW };
const StackSize maxStackSize = 65535;

class PCB {
public:
	PCB(StackSize stackSize, Time timeSlice, Thread* thr);
	~PCB();
	static void wrapper();
	void start();
	ID getId() { return myID; }
private:
	friend class Thread;
	friend class Kernel;
	friend class Queue;
	friend class KernelSem;
	friend class KernelEv;

	Status status;
	unsigned *stack;
	unsigned ss;
	unsigned sp;
	unsigned bp;
	Time timeSlice;
	Time sleepTime;
	int waitTime;
	StackSize stackSize;
	static ID lastID;
	ID myID;
	Thread* myThread;
	Queue* waitingProcesses;
};

#endif /* PCB_H_ */
