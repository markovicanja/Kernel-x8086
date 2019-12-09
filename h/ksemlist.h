/*
 * ksemlist.h
 *
 *  Created on: May 13, 2019
 *      Author: OS1
 */

#ifndef KSEMLIST_H_
#define KSEMLIST_H_

class KernelSem;

class KSemList {
public:
	KSemList();
	~KSemList();
	void add(KernelSem* s);
	void remove(KernelSem* s);
	void deleteAll();
	void tick();
private:
	struct Elem {
		KernelSem* sem;
		Elem* next;
		Elem(KernelSem* s) { sem=s; next=0; }
	};
	Elem *first, *last;
};

#endif /* KSEMLIST_H_ */
