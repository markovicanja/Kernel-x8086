/*
 * krnlsem.h
 *
 *  Created on: May 13, 2019
 *      Author: OS1
 */

#ifndef KRNLSEM_H_
#define KRNLSEM_H_

#include "thread.h"

class Queue;
class KSemList;

class KernelSem {
public:
	KernelSem(int init);
	~KernelSem();
	int wait (Time maxTimeToWait);
	int signal(int n);
	int val () const { return value; }

	static KSemList allKernelSems;
	static void semTick();
protected:
	void block();
	void deblock();
private:
	friend class KSemList;

	Queue* blocked;
	int value;
};


#endif /* KRNLSEM_H_ */
