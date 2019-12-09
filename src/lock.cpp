/*
 * lock.cpp
 *
 *  Created on: May 27, 2019
 *      Author: OS1
 */
#include "lock.h"
#include "kernel.h"

void lock() {
	Kernel::lockCnt++;
}

void unlock() {
	if(Kernel::lockCnt == 0) return;
	Kernel::lockCnt--;
}
