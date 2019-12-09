/*
 * event.cpp
 *
 *  Created on: May 18, 2019
 *      Author: OS1
 */

#include "event.h"
#include "lock.h"

Event::Event(IVTNo ivtNo) {
	lock();
	myImpl = new KernelEv(ivtNo);
	unlock();
}

Event::~Event () {
	lock();
	if (myImpl) delete myImpl;
	myImpl = 0;
	unlock();
}

void Event::wait () {
	if (myImpl) myImpl->wait();
}

void Event::signal() {
	if (myImpl) myImpl->signal();
}
