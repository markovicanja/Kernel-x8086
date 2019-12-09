/*
 * thread.h
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */
#ifndef THREAD_H_
#define THREAD_H_

typedef void (*SignalHandler)();

typedef unsigned SignalId;

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms

typedef int ID;

class PCB; // Kernel's implementation of a user's thread

class Thread {
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);

	void handleSignals();
	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
	static void signalHandlerZero();
protected:
	friend class PCB;
	friend class Kernel;

	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() {}

private:
	void signalHandlerTwo();
	PCB* myPCB;
	Thread* parent;
	int signal1Allowed;

	struct signalElem {
		SignalId signal;
		signalElem* next;
		signalElem(SignalId s) { signal = s; next = 0; }
	};

	signalElem *signalFirst, *signalLast;

	struct handlerElem {
		SignalHandler handler;
		handlerElem* next;
		handlerElem(SignalHandler s) { handler = s; next = 0; }
	};
	handlerElem *hFirst[16], *hLast[16];

	int blockedSignals[16];
	static int blockedGlobally[16];
};

#endif /* THREAD_H_ */
