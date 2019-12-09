/*
 * queue.h
 *
 *  Created on: May 12, 2019
 *      Author: OS1
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "pcb.h"
#include "thread.h"

class Queue {
public:
	Queue() { head = tail = 0;  size = 0; }
	~Queue();
	PCB* deleteFirst();
	PCB* deleteById(ID id);
	void insert(PCB*);
	PCB* find(ID id);
	int getSize() const { return size; }
	int timeTick();
private:
	struct Elem {
		PCB* pcb;
		Elem* next;
		Elem(PCB* p) { pcb=p; next=0; }
	};
	Elem *head, *tail;
	int size;
};

#endif /* QUEUE_H_ */
