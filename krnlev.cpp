/*
 * krnlev.cpp
 *
 *  Created on: May 18, 2019
 *      Author: OS1
 */

#include "krnlev.h"
#include "kernel.h"

KernelEv* KernelEv::KernelEvents[256] = { 0 };

KernelEv::KernelEv(IVTNo ivt) {
	holder = (PCB*)Kernel::running;
	blocked = 0;
	value = 0;
	ivtNo = ivt;
	if (ivtNo >= 0 && ivtNo < 256) KernelEvents[ivtNo] = this;
}

KernelEv::~KernelEv() {
	if (blocked != 0) {
		deblock();
		holder = blocked = 0;
	}
	if (ivtNo >= 0 && ivtNo < 256) KernelEvents[ivtNo] = 0;
}

void KernelEv::wait() {
	if (Kernel::running == holder) {
		if (value == 0) block();
		else value = 0;
	}
}

void KernelEv::signal() {
	if (blocked == 0) value = 1;
	else deblock();
}

void KernelEv::block() {
	lock();
	Kernel::running->status = BLOCKED;
	blocked = (PCB*)Kernel::running;
	unlock();
	Kernel::dispatch();
}

void KernelEv::deblock() {
	lock();
	blocked->status = READY;
	Scheduler::put(blocked);
	blocked = 0;
	unlock();
}
