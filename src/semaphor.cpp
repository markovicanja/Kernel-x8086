/*
 * semaphor.cpp
 *
 *  Created on: May 13, 2019
 *      Author: OS1
 */

#include "semaphor.h"
#include "krnlsem.h"
#include "lock.h"

Semaphore::Semaphore(int init) {
	lock();
	myImpl=new KernelSem(init);
	unlock();
}

Semaphore::~Semaphore () {
	lock();
	delete myImpl;
	unlock();
}

int Semaphore::wait (Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n) {
	return myImpl->signal(n);
}

int Semaphore::val () const {
	return myImpl->val();
}
