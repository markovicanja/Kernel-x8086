/*
 * queue.cpp
 *
 *  Created on: May 12, 2019
 *      Author: OS1
 */

#include "queue.h"
#include "lock.h"
//#include <iostream.h>
//#include <stdio.h>
//#include <stdlib.h>

Queue::~Queue() {
	size = 0;
	Elem* tmp = head;
	head = tail = 0;
	while (tmp != 0) {
		Elem* old = tmp;
		tmp = tmp->next;
		delete old;
	}
}

void Queue::insert(PCB* pcb) {
	Elem* tmp = 0;
	for (tmp = head; tmp && tmp->pcb->myID != pcb->myID; tmp = tmp->next);
	if (tmp != 0) return; //vec je ubacen pcb
	lock();
	if (head == 0) head = tail = new Elem(pcb);
	else tail = tail->next = new Elem(pcb);
	size++;
	unlock();
}

PCB* Queue::deleteFirst() {
	PCB* ret = 0;
	if (head != 0) {
		lock();
		Elem* old = head;
		size--;
		head = head->next;
		if (head == 0) tail = 0;
		ret = old->pcb;
		delete old;
		unlock();
	}
	return ret;
}

PCB* Queue::find(ID id) {
	Elem* temp = 0;
	for (temp = head; temp && temp->pcb->myID != id; temp = temp->next);
	if (temp && temp->pcb->myID==id) return temp->pcb;
	return 0;
}

PCB* Queue::deleteById(ID id) {
	PCB* ret = 0;
	Elem *tmp = head, *prev = 0;
	if (tmp && tmp->pcb->myID == id) {
		lock();
		head = head->next;
		ret = tmp->pcb;
		if (head == tail) tail = head = 0;
		size--;
		delete tmp;
		unlock();
		return ret;
	}
	while (tmp && tmp->pcb->myID!=id) {
		prev = tmp;
		tmp = tmp->next;
	}
	if (!tmp) return 0;
	lock();
	prev->next = tmp->next;
	if (tail == tmp) tail = prev;
	ret = tmp->pcb;
	size--;
	delete tmp;
	unlock();
	return ret;
}

int Queue::timeTick() {
	int incVal = 0;
	PCB* p = 0;
	Elem* cur = head;
	int j = size;
	for (int i = 0; i < j; i++) {
		if(cur->pcb->sleepTime > 0) cur->pcb->sleepTime--;
		if(cur->pcb->sleepTime == 0 && cur->pcb->waitTime == 1) { //odblokira se zbog isteka vremena
			lock();
			incVal++;
			p = deleteById(cur->pcb->myID);
			p->status = READY;
			Scheduler::put(p);
			unlock();
		}
		cur = cur->next;
	}
	return incVal;
}
