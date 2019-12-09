/*
 * ksemlist.cpp
 *
 *  Created on: May 13, 2019
 *      Author: OS1
 */

#include "ksemlist.h"
#include "lock.h"
#include "krnlsem.h"
#include "queue.h"

//#include <stdio.h>
//#include <iostream.h>
//#include <stdlib.h>

KSemList::KSemList() {
	first = last = 0;
}

KSemList::~KSemList() {
	deleteAll();
}

void KSemList::deleteAll() {
	Elem* pom = first;
	first = last = 0;
	while (pom) {
		Elem* old = pom;
		pom = pom->next;
		delete old->sem;
		delete old;
	}
}

void KSemList::add(KernelSem* s) {
	lock();
	if (!first) first = last = new Elem(s);
	else last = last->next = new Elem(s);
	unlock();
}

void KSemList::remove(KernelSem* s) {
	Elem *cur = first, *prev=0;
	while(cur && cur->sem != s) {
		prev = cur;
		cur = cur->next;
	}
	if(!cur) return;
	lock();
	if (prev) prev->next = cur->next;
	else first = cur->next;
	if(cur == last) last = prev;
	unlock();
	delete cur;
}

void KSemList::tick() {
	for (Elem* cur = first; cur != 0; cur = cur->next) {
		lock();
		int incVal = cur->sem->blocked->timeTick();
		cur->sem->value += incVal;
		unlock();
	}
}
