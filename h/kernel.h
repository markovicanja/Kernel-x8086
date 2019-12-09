/*
 * kernel.h
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <dos.h>
#include "pcb.h"
#include "thread.h"
#include "queue.h"
#include "ksemlist.h"

typedef void interrupt (*pInterrupt)(...);
extern void tick();
class Queue;

class Kernel {
public:
	static void initSystem();
	static void restoreSystem();
	static void dispatch();
	static volatile PCB* running;
	static Thread* starting;
	static Thread* idle;
	static Queue* allProcesses;
	static pInterrupt oldRoutine;
	static void interrupt timer(...);
	static volatile int handlingSignals;
	static volatile int lockCnt;
protected:
	friend class Queue;
	friend class Thread;
	friend class PCB;
private:
	static volatile int context_switch_on_demand;
	static volatile int counter;
};

#endif /* KERNEL_H_ */
