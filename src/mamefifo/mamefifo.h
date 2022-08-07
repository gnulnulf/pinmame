#ifndef __MAMEFIFO_H_
#define __MAMEFIFO_H_
/*
#ifdef __MAMEFIFO_C_
#define EXTERN
#else
#define EXTERN extern
#endif
*/

int fifofd;
FILE *mamefifo;


void mame_fifo_init();
void mame_fifo_close();
void mame_fifo_send(char *mamefifostring );


#endif
