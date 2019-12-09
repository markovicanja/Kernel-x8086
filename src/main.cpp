#include "kernel.h"
#include "lock.h"
#include <stdio.h>
#include <stdlib.h>

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	Kernel::initSystem();
	int ret = userMain(argc, argv);
	lock();
	printf("User main return value = %d\n", ret);
	unlock();
	Kernel::restoreSystem();
	lock();
	printf("main->exit");
	unlock();
	return ret;
}
