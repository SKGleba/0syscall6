/* 
	0syscall6 by SKGleba
	All Rights Reserved
*/
#include "types.h"

/*
before:
        00804d06 49 da 0c 00     bsr        FUN_0080594e
        00804d0a 00 05           mov        r5,r0

after:
        00804d06 21 c5 00 00     movh       r5,0x0
        00804d0a 01 50           mov        r0,0x1
next:
        00804d0c 04 e0 9e 01     bnei       r0,0x0,LAB_00805048
*/
void _start(void) {
	*(u16_t *)0x00804d06 = (u16_t)0xc521;
	*(u16_t *)0x00804d08 = (u16_t)0;
	*(u16_t *)0x00804d0a = (u16_t)0x5001;
	return;
}
