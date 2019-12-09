/*
 * ivtentry.h
 *
 *  Created on: May 18, 2019
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "event.h"

typedef void interrupt (*pInterrupt)(...);

#define PREPAREENTRY(i,flag)\
	void interrupt inter##i(...);\
	IVTEntry e##i = IVTEntry(i,inter##i);\
	void interrupt inter##i(...){\
		e##i.signal();\
		if (flag == 1)\
		e##i.callOld();\
	}

class IVTEntry{
public:
	IVTEntry(IVTNo no, pInterrupt interrupt);
	~IVTEntry();

	void signal();
	void callOld();

	static IVTEntry* IVT[256];
	static IVTEntry* getEntry(IVTNo i);
private:
	pInterrupt oldRoutine;
	IVTNo ivtNo;
};

#endif /* IVTENTRY_H_ */
