/*
 * krnlev.h
 *
 *  Created on: May 18, 2019
 *      Author: OS1
 */

#ifndef KRNLEV_H_
#define KRNLEV_H_

typedef unsigned char IVTNo;

class PCB;

class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv ();
	void wait ();
	void signal();

	static KernelEv *KernelEvents[256];
protected:
	void block();
	void deblock();
private:
	IVTNo ivtNo;
	int value; // 0 ili 1
	PCB* holder;
	PCB* blocked;
};


#endif /* KRNLEV_H_ */
