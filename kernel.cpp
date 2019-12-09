/*
 * kernel.cpp

 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */

#include "kernel.h"
#include "lock.h"
#include "idle.h"
#include "schedule.h"
#include "krnlsem.h"

//#include <stdio.h>
//#include <stdlib.h>

volatile int Kernel::handlingSignals = 0;
volatile int Kernel::counter = 6;
volatile int Kernel::context_switch_on_demand = 0;
Thread* Kernel::starting = 0;
volatile PCB* Kernel::running = 0;
Thread* Kernel::idle = 0;
Queue* Kernel::allProcesses = 0;
pInterrupt Kernel::oldRoutine = 0;
volatile int Kernel::lockCnt = 0;

void interrupt Kernel::timer(...) {
	static unsigned tsp, tss, tbp;

	if (!context_switch_on_demand && counter > 0) counter--;

	if (!context_switch_on_demand) {
		tick();
		KernelSem::semTick();
	}

	if (((counter == 0 && running->timeSlice != 0) || context_switch_on_demand) && !handlingSignals) {
		if (lockCnt == 0) {
			context_switch_on_demand = 0;
	#ifndef BCC_BLOCK_IGNORE
			asm{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}
	#endif
			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;

			if (running && running->myID > 0 && running->status==READY) //ne stavlja starting i idle u scheduler
				Scheduler::put((PCB*)running);

			while(1) {
				PCB *newProcess = Scheduler::get();

				if (newProcess == 0) {
					if(starting && starting->myPCB->status == READY)
						newProcess = starting->myPCB;
					else newProcess = idle->myPCB;
				}

				if (newProcess->status == READY) {
					running = newProcess;

					tsp = running->sp;
					tss = running->ss;
					tbp = running->bp;
					counter = running->timeSlice;

	#ifndef BCC_BLOCK_IGNORE
					asm{
						mov sp, tsp
						mov ss, tss
						mov bp, tbp
					}
	#endif

					handlingSignals = 1;
					running->myThread->handleSignals();
					handlingSignals = 0;
					if(running->status == FINISHED) continue;
					break;
				}
			} //while
		}//if (lockCnt==0)
		else context_switch_on_demand = 1;
	} //if
	if (!context_switch_on_demand) {
#ifndef BCC_BLOCK_IGNORE
		asm int 60h;
#endif
	}
}

void Kernel::dispatch() {
	context_switch_on_demand = 1;
#ifndef BCC_BLOCK_IGNORE
		asm int 8h;
#endif
}

void Kernel::initSystem() {
#ifndef BCC_BLOCK_IGNORE
	asm cli;
	oldRoutine = getvect(0x8); //prekid od tajmera je na 08h
	setvect(0x8, timer);
	setvect(0x60, oldRoutine); //od 60h krecu korisnicke prekidne rutine
	allProcesses = new Queue();
	starting = new Thread(maxStackSize, 1);
	starting->myPCB->status = READY;
	running = starting->myPCB;
	idle = new Idle();
	idle->myPCB->status = READY;
	asm sti;
#endif
}

void Kernel::restoreSystem() {
#ifndef BCC_BLOCK_IGNORE
	asm cli;
	setvect(0x8, oldRoutine);
	asm sti;
#endif
	KernelSem::allKernelSems.deleteAll();
	delete allProcesses;
	delete idle;
	delete starting;
}
