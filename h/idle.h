/*
 * idle.h
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_

#include "thread.h"

class Idle: public Thread {
public:
	Idle() : Thread(256, 1) {}
protected:
	void run();
};

#endif /* IDLE_H_ */
