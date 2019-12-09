/*
 * pcb.cpp
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */

#include <dos.h>
#include "pcb.h"
#include "kernel.h"

//#include <stdio.h>
//#include <stdlib.h>

ID PCB::lastID = -2;

PCB::PCB(StackSize size, Time time, Thread* thr) {
	if (size < 12) stackSize = defaultStackSize/sizeof(unsigned);
	else if (size > maxStackSize) stackSize = maxStackSize/sizeof(unsigned);
	else stackSize = size/sizeof(unsigned);
	timeSlice = time;
	sleepTime = 0; //koliko spava na semaforu
	waitTime = 0; //da li spava neograniceno
	myThread = thr;
	myID = ++lastID;
	status = NEW;
	stack = 0;
	waitingProcesses = new Queue();
	ss = sp = bp = 0;

	lock();
	stack = new unsigned[stackSize];
	unlock();

	stack[stackSize - 1] = 0x200;

	#ifndef BCC_BLOCK_IGNORE
		stack[stackSize - 2] = FP_SEG(PCB::wrapper);
		stack[stackSize - 3] = FP_OFF(PCB::wrapper);
		sp = FP_OFF(stack + stackSize - 12); //8 praznih mesta za registre
		bp = sp;
		ss = FP_SEG(stack + stackSize - 12);
	#endif

	if (myID > 0) Kernel::allProcesses->insert(this);
}

PCB::~PCB() {
	if (stack) delete stack;
	stack = 0;
}

void PCB::start() {
	lock();
	if (myID > 0 && status == NEW) { //ako se vise puta poziva start, ubacuje je samo kad je NEW
		status = READY;
		Scheduler::put(this);
	}
	unlock();
}

void PCB::wrapper() {
	Kernel::running->myThread->run();
	int j = Kernel::running->waitingProcesses->getSize();
	for(int i = 0; i < j; i++) {
		lock();
		PCB* tmp = Kernel::running->waitingProcesses->deleteFirst();
		tmp->status = READY;
		Scheduler::put(tmp);
		unlock();
	}
	if (Kernel::running->myThread->parent) {
		Kernel::running->myThread->parent->signal1Allowed = 1;
		Kernel::running->myThread->parent->signal(1);
		Kernel::running->myThread->parent->signal1Allowed = 0;
	}
	Kernel::running->myThread->signalHandlerTwo();
	Kernel::running->status = FINISHED;
	Kernel::dispatch();
}
