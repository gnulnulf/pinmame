#ifndef __MAMEFIFO_H_
#define __MAMEFIFO_H_


#define MAME_SOCKET_VERSION "0.0.1"

#include <stdio.h>
#include <stdlib.h>

//extern core_tGlobals coreGlobals;

int fifofd;
FILE *mamesocket;
char mamesocket_string[4096];

void mame_socket_init();
void mame_socket_bind();
void mame_socket_close();
void mame_socket_send(int s, char *mamesocketstring);
void mame_socket_send_all(char *mamesocketstring );
void mame_socket_loop();
int mame_socket_main(void);

int setNonblocking(int fd);


#endif