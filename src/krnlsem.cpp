/*
 * krnlsem.cpp
 *
 *  Created on: May 13, 2019
 *      Author: OS1
 */

#include "krnlsem.h"
#include "lock.h"
#include "kernel.h"
#include "schedule.h"
#include "queue.h"
#include "pcb.h"

//#include <stdio.h>
//#include <iostream.h>
//#include <stdlib.h>

KSemList KernelSem::allKernelSems;

KernelSem::KernelSem(int init) {
	value = init;
	lock();
	blocked = new Queue();
	unlock();
	allKernelSems.add(this);
}

KernelSem::~KernelSem() {
	allKernelSems.remove(this);
	for (int i=0; i<blocked->getSize(); i++)
		deblock();
	delete blocked;
}

int KernelSem::wait(Time maxTimeToWait) { //0 ako je deblokirana zbog isteka vremena
	int ret = 1;
	if(--value < 0) {
		Kernel::running->sleepTime = maxTimeToWait;
		Kernel::running->waitTime = maxTimeToWait ? 1 : 0;
		block();
		if (Kernel::running->sleepTime == 0 && Kernel::running->waitTime == 1 )
			ret = 0;
	}
	return ret;
}

int KernelSem::signal(int n) {
	if(n == 0) {
		if(value++ < 0) {
			deblock();
			return 0;
		}
	}
	else if(n > 0) {
		lock();
		int i;
		for (i = 0; i < n && blocked->getSize() > 0; i++)
			deblock();
		value += n;
		unlock();
		return i;
	}
	return n;
}

void KernelSem::block() {
	lock();
	Kernel::running->status = BLOCKED;
	blocked->insert((PCB*)Kernel::running);
	unlock();
	Kernel::dispatch();
}

void KernelSem::deblock() {
	lock();
	PCB* pcb = blocked->deleteFirst();
	if (pcb == 0) {
		unlock();
		return;
	}
	pcb->status = READY;
	Scheduler::put(pcb);
	unlock();
}

void KernelSem::semTick() {
	allKernelSems.tick();
}
