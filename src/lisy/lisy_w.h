// license:BSD-3-Clause

#ifndef LISY_W_H
#define LISY_W_H

void lisy_w_display_handler(void);
void lisy_w_throttle(void);
int lisymini_get_gamename(char* gamename);
int lisyapc_get_gamename(char* gamename);
void lisy_w_solenoid_handler(void);
void lisy_w_direct_solenoid_handler(unsigned char data);
void lisy_w_switch_handler(void);
unsigned char lisy_w_switch_reader(unsigned char* action);
void lisy_w_lamp_handler(void);
void lisy_w_sound_handler(unsigned char board, unsigned char data);

#define LISYW_TYPE_NONE    0
#define LISYW_TYPE_SYS9    1
#define LISYW_TYPE_SYS11A  2
#define LISYW_TYPE_SYS7    3
#define LISYW_TYPE_SYS11   4
#define LISYW_TYPE_SYS11RK 5
#define LISYW_TYPE_SYS11B  6
#define LISYW_TYPE_SYS11C  7
#define LISYW_TYPE_SYS6    8
#define LISYW_TYPE_SYS6A   9
#define LISYW_TYPE_SYS4    10
#define LISYW_TYPE_SYS3    11
#define LISYW_TYPE_SYS11RG 12

#endif /* LISY_W_H */
