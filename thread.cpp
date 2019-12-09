/*
 * thread.cpp
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */

#include "thread.h"
#include "kernel.h"
#include "lock.h"
#include "pcb.h"
#include "schedule.h"
#include "queue.h"

//#include <stdio.h>
//#include <stdlib.h>

int Thread::blockedGlobally[16] = { 0 };

Thread::Thread(StackSize stackSize, Time timeSlice) {
	lock();
	myPCB = new PCB(stackSize, timeSlice, this);
	signal1Allowed = 0;
	if (myPCB->myID <= 0) { //za starting i idle postavlja sve na nulu
		parent = 0;
		for (int i = 0; i < 16; i++) {
			blockedSignals[i] = 0;
			hFirst[i] = hLast[i] = 0;
		}
		signalFirst = signalLast = 0;
	}
	else { //u suprotnom kopira sve od roditeljske niti
		parent = Kernel::running->myThread;
		for (int i = 0; i < 16; i++) {
			blockedSignals[i] = parent->blockedSignals[i];
			hFirst[i] = hLast[i] = 0;
			handlerElem* tmp = parent->hFirst[i];
			while (tmp) {
				hLast[i] = (!hFirst[i]? hFirst[i] : hLast[i]->next) = new handlerElem(tmp->handler);
				tmp = tmp->next;
			}
		} //for
		signalFirst = signalLast = 0;
	}//else
	hLast[0] = hFirst[0] = new handlerElem(&Thread::signalHandlerZero);
	unlock();
}

void Thread::start() {
	myPCB->start();
}

Thread::~Thread() {
	//waitToComplete();
	lock();
	Kernel::allProcesses->deleteById(myPCB->myID);
	for (int i = 0; i < 16; i++) {
		while (hFirst[i]) {
			handlerElem* old = hFirst[i];
			hFirst[i] = hFirst[i]->next;
			delete old;
		}
		hFirst[i] = hLast[i] = 0;
	}
	while (signalFirst) {
		signalElem* old = signalFirst;
		signalFirst = signalFirst->next;
		delete old;
	}
	signalFirst = signalLast = 0;
	delete myPCB;
	unlock();
}

void Thread::waitToComplete() { //running se blokira dok this ne zavrsi
	if (myPCB->myID <= 0 || myPCB==(PCB*)Kernel::running || myPCB->status==FINISHED || myPCB->status==NEW) return;
	lock();
	Kernel::running->status = BLOCKED;
	myPCB->waitingProcesses->insert((PCB*)Kernel::running);
	unlock();
	Kernel::dispatch();
}

ID Thread::getId() {
	return myPCB->myID;
}

ID Thread::getRunningId() {
	return Kernel::running->myID;
}

Thread* Thread::getThreadById(ID id) {
	if (id == -1) return Kernel::starting;
	else if (id == 0) return Kernel::idle;
	PCB* p = Kernel::allProcesses->find(id);
	if (p) return p->myThread;
	return 0;
}

void Thread::handleSignals() {
	signalElem *cur = 0, *prev = 0, *old = 0;
	for (cur = signalFirst; cur != 0;) {
		if (blockedGlobally[cur->signal] || blockedSignals[cur->signal]){
			prev = cur; cur = cur->next; continue;
		}
		SignalId signal = cur->signal;
		if (prev) prev->next = cur->next;
		else signalFirst = signalFirst->next;
		if (!signalFirst) signalLast = 0;
		old = cur;
		cur = cur->next;
		delete old;

		for (handlerElem* tmp = hFirst[signal]; tmp != 0; tmp = tmp->next) {
			tmp->handler();
		}
	}
}

void Thread::signal(SignalId signal) {
	if (signal1Allowed == 0 && signal == 1) return; //signal1 moze da se pozove samo od strane sistema, a za signal2 odma pozivam handler
	if (signal == 2 || signal > 15 || hFirst[signal] == 0) return; //ako ne postoji handler, nema efekta
	lock();
	signalLast = (!signalFirst? signalFirst : signalLast->next) = new signalElem(signal);
	unlock();
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	if (signal == 0 || signal > 15) return; //signal 0 vec ima predefinisan handler
	lock();
	hLast[signal] = (!hFirst[signal]? hFirst[signal] : hLast[signal]->next) = new handlerElem(handler);
	unlock();
}

void Thread::unregisterAllHandlers(SignalId id) {
	if (id == 0 || id > 15) return;
	handlerElem* pom=hFirst[id];
	hFirst[id] = hLast[id] = 0;

	while (pom) {
		handlerElem* old = pom;
		pom = pom->next;
		delete old;
	}
}

void Thread::swap(SignalId i, SignalHandler hand1, SignalHandler hand2) {
	if (i == 0 || i > 15) return;
	handlerElem *h1 = 0, *h2 = 0;
	for(handlerElem* cur = hFirst[i]; cur != 0 && (h1 == 0 || h2 == 0); cur = cur->next) {
		if (cur->handler == hand1) h1 = cur;
		if (cur->handler == hand2) h2 = cur;
	}
	if (!h1 || !h2) return; //ako handleri nisu registrovani nema efekta
	lock();
	h1->handler = hand2;
	h2->handler = hand1;
	unlock();
}

void Thread::blockSignal(SignalId signal) {
	if (signal > 15) return;
	blockedSignals[signal] = 1;
}

void Thread::blockSignalGlobally(SignalId signal) {
	if (signal > 15) return;
	blockedGlobally[signal] = 1;
}

void Thread::unblockSignal(SignalId signal) {
	if (signal > 15) return;
	blockedSignals[signal] = 0;
}

void Thread::unblockSignalGlobally(SignalId signal) {
	if (signal > 15) return;
	blockedGlobally[signal] = 0;
}

void Thread::signalHandlerTwo() {
	if (blockedGlobally[2] || blockedSignals[2]) return;
	for (handlerElem* tmp = hFirst[2]; tmp != 0; tmp = tmp->next) {
		Kernel::handlingSignals = 1;
		tmp->handler();
		Kernel::handlingSignals = 0;
	}
}

void Thread::signalHandlerZero() {
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
}
