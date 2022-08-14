#ifndef __MAMEFIFO_H_
#define __MAMEFIFO_H_


#include <stdio.h>
#include <stdlib.h>

/*
#ifdef __MAMEFIFO_C_
#define EXTERN
#else
#define EXTERN extern
#endif
*/

int fifofd;
FILE *mamefifo;
char mamefifo_string[4096];

void mame_fifo_init();
void mame_fifo_close();
void mame_fifo_send(char *mamefifostring );


#endif
