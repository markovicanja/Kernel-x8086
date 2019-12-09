#include "intLock.h"
#include "kernel.h"

int mutex_glb = 1;

void mutex_glb_wait(){
 sloop:asm{
	mov ax, 0
	xchg ax, mutex_glb
}
 if(_AX ==0){
	Kernel::dispatch();
	asm jmp sloop;
 }
}
//1 - otkljucano, 0 - zakljucano
//ako je zakljucano, menja se kontekst i vrti se u petlji dok se ne otkljuca
//ako je otkljucano, postavlja 0 u mutex, zakljucava ga i izlazi iz petlje
