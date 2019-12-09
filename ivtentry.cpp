/*
 * ivtentry.cpp
 *
 *  Created on: May 18, 2019
 *      Author: OS1
 */

#include "ivtentry.h"
#include "krnlev.h"
#include <dos.h>

IVTEntry* IVTEntry::IVT[256] = { 0 };

IVTEntry::IVTEntry(IVTNo numEntry, pInterrupt interruptRoutine) {
	oldRoutine = 0;
	ivtNo = -1; //ako je vec postavljen ovaj ulaz
	if (ivtNo >= 0 && ivtNo < 256) {
		if (!IVT[ivtNo]) {
			IVT[ivtNo] = this;
			ivtNo = numEntry;
#ifndef BCC_BLOCK_IGNORE
			asm cli;
			oldRoutine = getvect(ivtNo);
			setvect(ivtNo, interruptRoutine);
			asm sti;
#endif
		}
	}
}

IVTEntry::~IVTEntry() {
	if (ivtNo == -1) return;
#ifndef BCC_BLOCK_IGNORE
	asm cli;
	setvect(ivtNo, oldRoutine);
	asm sti;
#endif
	IVT[ivtNo] = 0;
	//poziv stare prekidne rutine
}

void IVTEntry::signal() {
	if (ivtNo >= 0 && ivtNo < 256 && KernelEv::KernelEvents[ivtNo])
		KernelEv::KernelEvents[ivtNo]->signal();
}

void IVTEntry::callOld() {
	if (ivtNo == -1) return;
	oldRoutine();
}

IVTEntry* IVTEntry::getEntry(IVTNo num) {
	if (num >= 0 && num < 256) return IVT[num];
	return 0;
}
