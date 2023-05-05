// license:BSD-3-Clause

/*
 LISY35.c
 June 2017
 bontango
*/

#include "lisy35.h"
#include "coils.h"
#include "displays.h"
#include "driver.h"
#include "eeprom.h"
#include "fileio.h"
#include "hw_lib.h"
#include "lisy.h"
#include "lisy_home.h"
#include "lisy_mame.h"
#include "sound.h"
#include "switches.h"
#include "utils.h"
#include "wheels.h"
#include "wpc/core.h"
#include "wpc/sndbrd.h"
#include "xmame.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <wiringPi.h>

extern char debugbuf[256]; // see hw_lib.c

//global var for internal game_name structure,
//set by  lisy80_get_gamename in unix/main
t_stru_lisy35_games_csv lisy35_game;

//global var for timing and speed
int g_lisy35_throttle_val = 5000;

//global var for sound options
unsigned char lisy35_has_soundcard = 0;    //there is a pHat soundcard installed
unsigned char lisy35_has_own_sounds = 0;   //play own sounds rather then usinig piname sound emulation
unsigned char StarShip_has_own_sounds = 0; //play StarShip sounds
t_stru_lisy35_sounds_csv lisy35_sound_stru[256];

//internal switch Matrix for system1, we need 7 elements
//as pinmame internal starts with 1
//swMatrix 6 for SLAM and other special switches
unsigned char swMatrixLISY35[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

//for special cases, where pins are doings strobes
//we need to be aware and filter out
unsigned char lisy35_J4PIN5_is_strobe = 0;
unsigned char lisy35_J4PIN8_is_strobe = 0;

//Okaegi mod ?
unsigned char lisy35_okaegi_mod = 0;

//from lisy.c set in lisy_home.c
extern unsigned char lisy_home_ss_ignore_credit;

//from coils.c
extern unsigned char lisy35_bally_hw_check_finished;

//internal for lisy35.c
//to have an delayed nvram write
static unsigned char want_to_write_nvram = 0;
// switch to sound raw mode ( for special cfg 8 )
static unsigned char lisy35_sound_raw = 0;

//init SW portion of lisy35 Starship special
void
lisy35_ss_init(void) {
    int i, sb, dip_value;
    char s_lisy_software_version[16];
    unsigned char sw_main, sw_sub, commit;
    char infostr[25];

    //set signal handler
    lisy80_set_sighandler();

    //show up on calling terminal
    lisy_get_sw_version(&sw_main, &sw_sub, &commit);
    sprintf(s_lisy_software_version, "%d%02d %02d", sw_main, sw_sub, commit);
    fprintf(stderr, "This is LISY (Lisy35 Starship) by bontango, Version %s\n", s_lisy_software_version);

    //show the 'boot' message
    display_show_boot_message_lisy35(s_lisy_software_version);

    //check sound options
    if (ls80opt.bitv.JustBoom_sound) {
        lisy35_has_soundcard = 1;
        if (ls80dbg.bitv.sound)
            lisy80_debug("internal soundcard to be activated");
        //do we want to use pinamme sounds?
        if (ls80opt.bitv.test) {
            if (ls80dbg.bitv.sound)
                lisy80_debug("we try to use pinmame sounds");
        } else {
            lisy35_has_own_sounds = 1;
            if (ls80dbg.bitv.sound)
                lisy80_debug("we try to use our own sounds");
        }
    }

    // try say something about LISY35 if soundcard is installed
    if (lisy35_has_soundcard) {
        char message[200];

        //set volume according to poti
        lisy_adjust_volume();
        //try to read welcome message from file
        if (lisy_file_get_welcome_msg(message) >= 0) {
            if (ls80dbg.bitv.basic) {
                sprintf(debugbuf, "Info: welcome Message is: %s", message);
                lisy80_debug(debugbuf);
            }
            sprintf(debugbuf, "/bin/echo \"%s\" | /usr/bin/festival --tts", message);
            system(debugbuf);
        }
    }

    //boot ss event handler (will activate i.e. bootsound )
    lisy_home_ss_event_handler(LISY_HOME_SS_EVENT_BOOT, lisy35_has_soundcard, 0, 0);

    //show green ligth for now, lisy35 is running
    lisy80_set_red_led(0);
    lisy80_set_yellow_led(0);
    lisy80_set_green_led(1);

    //init own sounds if requested
    if (lisy35_has_own_sounds) {
        //first try to read sound opts, as we NEED them
        if (lisy200_file_get_soundopts() < 0) {
            fprintf(stderr, "no sound opts file; sound init failed, sound emulation disabled\n");
            lisy35_has_own_sounds = 0;
        } else {
            fprintf(stderr, "info: sound opt file read OK\n");

            if (ls80dbg.bitv.sound) {
                int i;
                for (i = 1; i <= 255; i++) {
                    if (lisy35_sound_stru[i].soundnumber != 0)
                        fprintf(stderr, "Switch[%d]: %s %s opt:%d trigger:%d wait:%d delay:%d preload:%d \n", i,
                                lisy35_sound_stru[i].path, lisy35_sound_stru[i].name, lisy35_sound_stru[i].option,
                                lisy35_sound_stru[i].trigger, lisy35_sound_stru[i].wait, lisy35_sound_stru[i].delay,
                                lisy35_sound_stru[i].preload);
                }
            }
        }
    }

    if (lisy35_has_own_sounds) {
        //now open soundcard, and init soundstream
        if (StarShip_sound_stream_init() < 0) {
            fprintf(stderr, "sound init failed, sound emulation disabled\n");
            lisy35_has_own_sounds = 0;
        } else {
            fprintf(stderr, "info: sound init done\n");
        }
    }

    //Starship mspecific inits

    //RTH temp switch to Starship sounds
    if (lisy35_has_own_sounds) {
        lisy35_has_own_sounds = 0;
        StarShip_has_own_sounds = 1;
    }

    //read the dip setting
    dip_value = display_get_ss_dipsw_value();
    fprintf(stderr, "Info: Starship: Dip setting is %d\n", dip_value);
    //send string to LCD
    sprintf(infostr, "DIP setting is:%02d   ", dip_value);
    display_ss_LCD_string2row(3, infostr);
    //prepare value for mapping (0..3)
    dip_value = dip_value / 4;

    // do the mapping
    lisy_file_get_home_ss_lamp_mappings(dip_value);
    lisy_file_get_home_ss_coil_mappings(dip_value);
    lisy_file_get_home_ss_special_coil_mappings(dip_value);
    lisy_file_get_home_ss_special_lamp_mappings(dip_value);
    // get LEDs and colourcodes for GI
    lisy_file_get_home_ss_GI(dip_value);
    //select solenoidboard by default
    lisyh_coil_select_solenoid_driver();
    lisyh_coil_select_led_driver_line(1);
    // send colorcodes to LED driver
    lisy_home_ss_send_led_colors();
    //read the csv file for Starship general parameters
    lisy_file_get_home_ss_general();
    //read sound mapping
    lisy_file_get_home_ss_sound_mappings(dip_value);
    //deactivate all special solenoids with a mapping
    lisyh_init_special_coils();

    //init ss event handler (will activate i.e. GI and reset credit wheels )
    lisy_home_ss_event_handler(LISY_HOME_SS_EVENT_INIT, 0, 0, 0);

    //collect latest informations and start the lisy logger
    lisy_env.has_soundcard = lisy35_has_soundcard;
    lisy_env.has_own_sounds = StarShip_has_own_sounds;
    lisy_logger();
}

//init SW portion of lisy35
void
lisy35_init(void) {
    int i, sb;
    char s_lisy_software_version[16];
    unsigned char sw_main, sw_sub, commit;

    //do the init on vars
    //for (i=0; i<=36; i++) lisy1_lamp[i]=0;

    //set signal handler
    lisy80_set_sighandler();

    //show up on calling terminal
    lisy_get_sw_version(&sw_main, &sw_sub, &commit);
    sprintf(s_lisy_software_version, "%d%02d %02d", sw_main, sw_sub, commit);
    fprintf(stderr, "This is LISY (Lisy35) by bontango, Version %s\n", s_lisy_software_version);

    //show the 'boot' message
    display_show_boot_message_lisy35(s_lisy_software_version);

    //check sound options
    if (ls80opt.bitv.JustBoom_sound) {
        lisy35_has_soundcard = 1;
        if (ls80dbg.bitv.sound)
            lisy80_debug("internal soundcard to be activated");
        //do we want to use pinamme sounds?
        if (ls80opt.bitv.test) {
            if (ls80dbg.bitv.sound)
                lisy80_debug("we try to use pinmame sounds");
        } else {
            lisy35_has_own_sounds = 1;
            if (ls80dbg.bitv.sound)
                lisy80_debug("we try to use our own sounds");
        }
    }

    // try say something about LISY35 if soundcard is installed
    if (lisy35_has_soundcard) {
        char message[200];

        //set volume according to poti
        lisy_adjust_volume();
        //try to read welcome message from file
        if (lisy_file_get_welcome_msg(message) >= 0) {
            if (ls80dbg.bitv.basic) {
                sprintf(debugbuf, "Info: welcome Message is: %s", message);
                lisy80_debug(debugbuf);
            }
            sprintf(debugbuf, "/bin/echo \"%s\" | /usr/bin/festival --tts", message);
            system(debugbuf);
        }
    }

    //show green ligth for now, lisy35 is running
    lisy80_set_red_led(0);
    lisy80_set_yellow_led(0);
    lisy80_set_green_led(1);

    //init own sounds if requested
    if (lisy35_has_own_sounds) {
        //first try to read sound opts, as we NEED them
        if (lisy35_file_get_soundopts() < 0) {
            fprintf(stderr, "no sound opts file; sound init failed, sound emulation disabled\n");
            lisy35_has_own_sounds = 0;
        } else {
            fprintf(stderr, "info: sound opt file read OK\n");

            if (ls80dbg.bitv.sound) {
                int i;
                for (i = 1; i <= 255; i++) {
                    if (lisy35_sound_stru[i].soundnumber != 0)
                        fprintf(stderr, "Sound[%d]: %s %s %d \n", i, lisy35_sound_stru[i].path,
                                lisy35_sound_stru[i].name, lisy35_sound_stru[i].option);
                }
            }
        }
    }

    if (lisy35_has_own_sounds) {
        //now open soundcard, and init soundstream
        if (lisy35_sound_stream_init() < 0) {
            fprintf(stderr, "sound init failed, sound emulation disabled\n");
            lisy35_has_own_sounds = 0;
        } else
            fprintf(stderr, "info: sound init done\n");
    }

    //collect latest informations and start the lisy logger
    lisy_env.has_soundcard = lisy35_has_soundcard;
    lisy_env.has_own_sounds = lisy35_has_own_sounds;
    lisy_logger();
}

//read the csv file on /lisy partition and the DIP switch setting
//give back gamename accordently and line number
// -1 in case we had an error
//this is called early from unix/main.c
int
lisy35_get_gamename(char* gamename) {

    int ret;

    //use function from fileio to get more details about the gamne
    ret = lisy35_file_get_gamename(&lisy35_game);

    //give back the name and the number of the game
    strcpy(gamename, lisy35_game.gamename);

    //store throttle value from gamelist to global var
    g_lisy35_throttle_val = lisy35_game.throttle;

    //give this info on screen
    if (ls80dbg.bitv.basic) {
        sprintf(debugbuf, "Info: LISY35 Throttle value is %d for this game", g_lisy35_throttle_val);
        lisy80_debug(debugbuf);
    }

    //other infos are stored in global var
    //store what we know already
    strcpy(lisy_env.variant, "LISY35");
    lisy_env.selection = ret;
    strcpy(lisy_env.gamename, lisy35_game.gamename);
    strcpy(lisy_env.long_name, lisy35_game.long_name);
    lisy_env.throttle = lisy35_game.throttle;
    lisy_env.clockscale = 0; //not used

    return ret;
}

//help functions
//for bcd value PIC
int
my_seg2bcd(uint16_t seg) {
    int bcd;

    switch (seg) {
        case 0x3f:
            bcd = 0;
            break;
        case 0x06:
            bcd = 1;
            break;
        case 0x5b:
            bcd = 2;
            break;
        case 0x4f:
            bcd = 3;
            break;
        case 0x66:
            bcd = 4;
            break;
        case 0x6d:
            bcd = 5;
            break;
        case 0x7d:
            bcd = 6;
            break;
        case 0x07:
            bcd = 7;
            break;
        case 0x7f:
            bcd = 8;
            break;
        case 0x6f:
            bcd = 9;
            break;
        default:
            bcd = 15;
            break; //space
    }
    return bcd;
}

//for debug
char
by_seg2char(uint16_t seg) {
    char mychar;

    switch (seg) {
        case 0x3f:
            mychar = '0';
            break;
        case 0x06:
            mychar = '1';
            break;
        case 0x5b:
            mychar = '2';
            break;
        case 0x4f:
            mychar = '3';
            break;
        case 0x66:
            mychar = '4';
            break;
        case 0x6d:
            mychar = '5';
            break;
        case 0x7d:
            mychar = '6';
            break;
        case 0x07:
            mychar = '7';
            break;
        case 0x7f:
            mychar = '8';
            break;
        case 0x6f:
            mychar = '9';
            break;
        default:
            mychar = ' ';
            break; //space
    }
    return mychar;
}

char
my_datchar2(int dat) {
    char datchar;

    switch (dat) {
        case 0:
            datchar = '0';
            break;
        case 1:
            datchar = '1';
            break;
        case 2:
            datchar = '2';
            break;
        case 3:
            datchar = '3';
            break;
        case 4:
            datchar = '4';
            break;
        case 5:
            datchar = '5';
            break;
        case 6:
            datchar = '6';
            break;
        case 7:
            datchar = '7';
            break;
        case 8:
            datchar = '8';
            break;
        case 9:
            datchar = '9';
            break;
        case 10:
            datchar = ' ';
            break;
        case 15:
            datchar = ' ';
            break;
        default:
            datchar = '-';
            break;
    }

    return datchar;
}

//display handler
//we use segment value now
/*
void lisy35_display_handler( int index, uint16_t seg, int myval )
{

static uint16_t myseg[56]
= { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

int i,value;
int has_changed = 0;


if ( index > 55 )
  {
    fprintf(stderr,"LISY35_DISP_HANDLER: index out of range index:%d  seg:%x\n",index,seg);
    return;
  }

if ( seg != myseg[index] )
  {
    has_changed = 1; //for possible debugging only
    //remember
    myseg[index] = seg;
    //calc bcd value
    value = my_seg2bcd(seg);

switch(index) //reverse order is handled by switch PIC
   {
     case 1 ... 7: //Bally player 1
        display35_show_int( 1, index, value);
        break;
     case 9 ... 15: //Bally player 2
        display35_show_int( 2, index-8, value);
        break;
     case 17 ... 23: //Bally player 3
        display35_show_int( 3, index-16, value);
        break;
     case 25 ... 31: //Bally player 4
        display35_show_int( 4, index-24, value);
        break;
     case 33 ... 39: //Bally staus ( full player display )
        display35_show_int( 0, index-32, value);
        break;
     case 41 ... 47: //Bally player 5
        display35_show_int( 5, index-40, value);
        break;
     case 49 ... 55: //Bally player 6
        display35_show_int( 6, index-48, value);
        break;
     default : printf("unknown index:%d (value:%d)\n",index,value);
	break;
   }//switch
  }


if ( ls80dbg.bitv.displays )
 {
 if ( has_changed )
  {
    //printf("----- Index %d changed to %d -----\n",index,value);
    printf("----- Index %d changed to %d (bcd:%d) -----\n",index,value,myval);

    printf("Player1:>");
    for (i=1; i<=7; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
    printf("Player2:>");
    for (i=9; i<=15; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
    printf("Player3:>");
    for (i=17; i<=23; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
    printf("Player4:>");
    for (i=25; i<=31; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
    printf(" Status:>");
    for (i=33; i<=39; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
    printf("Player5:>");
    for (i=41; i<=47; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
    printf("Player6:>");
    for (i=49; i<=55; i++) printf("%c",by_seg2char(myseg[i]));
    printf("<\n");
  }
 }//if display debug
}//display_handler
*/

//display handler
void
lisy35_display_handler(int index, int value) {

    static int myvalue[56] = {80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,
                              80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,
                              80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80};

    int i;
    int has_changed = 0;

    if (index > 55) {
        fprintf(stderr, "LISY35_DISP_HANDLER: index out of range index:%d  value:%d\n", index, value);
        return;
    }

    if (value != myvalue[index]) {
        has_changed = 1; //for possible debugging only
        switch (index)   //reverse order is handled by switch PIC
        {
            case 1 ... 7: //Bally player 1
                display35_show_int(1, index, value);
                break;
            case 9 ... 15: //Bally player 2
                display35_show_int(2, index - 8, value);
                break;
            case 17 ... 23: //Bally player 3
                display35_show_int(3, index - 16, value);
                break;
            case 25 ... 31: //Bally player 4
                display35_show_int(4, index - 24, value);
                break;
            case 33 ... 39: //Bally staus ( full player display )
                display35_show_int(0, index - 32, value);
                break;
            case 41 ... 47: //Bally player 5
                display35_show_int(5, index - 40, value);
                break;
            case 49 ... 55:      //Bally player 6
                if (value == 15) //special handling (pinmame bug?) (6mill dollar man only)
                {
                    value = myvalue[index]; //keep old value
                    has_changed = 0;        //reset debug flag
                } else
                    display35_show_int(6, index - 48, value);
                break;
            default:
                printf("unknown index:%d (value:%d)\n", index, value);
                break;
        } //switch
        //remember
        myvalue[index] = value;
    }

    if (ls80dbg.bitv.displays) {
        if (has_changed) {
            printf("----- Index %d changed to %d -----\n", index, value);

            printf("Player1:>");
            for (i = 1; i <= 7; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
            printf("Player2:>");
            for (i = 9; i <= 15; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
            printf("Player3:>");
            for (i = 17; i <= 23; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
            printf("Player4:>");
            for (i = 25; i <= 31; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
            printf(" Status:>");
            for (i = 33; i <= 39; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
            printf("Player5:>");
            for (i = 41; i <= 47; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
            printf("Player6:>");
            for (i = 49; i <= 55; i++)
                printf("%c", my_datchar2(myvalue[i]));
            printf("<\n");
        }
    } //if display debug

} //display_handler

/*
  switch update
  set via core_setSw
  swMatrix[0] is pinmame internal (sound?)
  swMatrix[1..6] is bally,
  swMatrix[7] is  'special switches' bit7:Test; bit6:S33;
*/
void
lisy35_switch_update(void) {
    int ret;
    unsigned char strobe, returnval, action;
    static int simulate_coin_flag = 0;

    //read values from pic
    //check if there is an update first
    ret = lisy35_switch_reader(&action);
    //if debug mode is set we get our reedings from udp switchreader in additon
    //but do not overwrite real switches
    if ((ls80dbg.bitv.basic) & (ret == 80)) {
        if ((ret = lisy_udp_switch_reader(&action, 0)) != 80) {
            sprintf(debugbuf, "LISY35 Switch_reader: (UDP Server Data received: %d", ret);
            lisy80_debug(debugbuf);
            //we start internally with 0, so substract one
            --ret;
        }
    }

    //running on Starship? switchnumber has to be increased
    if ((lisy_hardware_revision == 200) & (ret != 80))
        lisy_home_ss_event_handler(LISY_HOME_SS_EVENT_SWITCH, ret + 1, action, 0);

    //ignore credit switch if we running on Starship
    //and 2canplay lamp is ON
    if ((ret == 5) & (action == 1)) {
        if ((lisy_hardware_revision == 200) & (lisy_home_ss_ignore_credit == 1)) {
            ret = 80;

            if (ls80dbg.bitv.switches) {
                lisy80_debug("LISY35 Switch_reader: Ball one AND 2canplay activ: credit ignored");
            }
        }
    }

    //NOTE: system has has 6*8==40 switches in maximum, counting 1...48; ...
    //we use 'internal strobe 6' to handle special switches in the same way ( TEST=49,S33=50 )
    if (ret < 80) //ret is switchnumber
    {

        //calculate strobe & return
        //Note: this is different from system80
        strobe = ret / 8;
        returnval = ret % 8;

        //set the bit in the Matrix var according to action
        // action 1 means set the bit
        // any other means delete the bit
        if (action) //set bit
	  {
            SET_BIT(swMatrixLISY35[strobe + 1], returnval);
	  }
        else //delete bit
	  {
            CLEAR_BIT(swMatrixLISY35[strobe + 1], returnval);
	  }
	//RTH Todo change internal lisy handling from matrix to switch number

        //set the switch
        core_setSw(ret+1, action);

        if (ls80dbg.bitv.switches) {
            sprintf(debugbuf, "LISY35_SWITCH_HANDLER [%d][%d]Switch#:%d action:%d\n", strobe+1,returnval, ret+1, action);
            lisy80_debug(debugbuf);
		}

    } //if ret < 80 => update internal matrix

    //do we need a 'special' routine to handle that switch?
    //system35 Test switch is separate but mapped to strobe:6 ret:7
    //so matrix 7,7 to check
    if (CHECK_BIT(swMatrixLISY35[7], 7)) //is bit set?
    {
        //after 3 secs we initiate shutdown; internal timer 0
        //def lisy_timer( unsigned int duration, int command, int index)
        if (lisy_timer(3000, 0, 0))
            lisy_time_to_quit_flag = 1;
    } else // bit is zero, reset timer index 0
    {
        lisy_timer(0, 1, 0);
    }

    if (ls80opt.bitv.freeplay == 1) //only if freeplay option is set
    {
        //system35 (Credit) Replay switch strobe:0 ret:5, so matrix 1,5 to check

        //set volume each time replay is pressed
	/* deactivated because of problems with switch detection
        if (lisy35_has_soundcard) {
            if (CHECK_BIT(swMatrixLISY35[1], 5)) //is bit set?
            {
                lisy_adjust_volume();
                if (ls80dbg.bitv.basic)
                    lisy80_debug("Volume setting initiated by REPLAY Switch");
            }
        }
	*/
        if (CHECK_BIT(swMatrixLISY35[1], 5)) //is bit set?
        {
            //after 2 secs we simulate coin insert via Chute#1; internal timer 1
            //def lisy_timer( unsigned int duration, int command, int index)
            if (lisy_timer(2000, 0, 1)) {
                simulate_coin_flag = 1;
                lisy_timer(0, 1, 1);
            }
        } else // bit is zero, reset timer index 0
        {
            lisy_timer(0, 1, 1);
        }

        //do we need to simalte coin insert?
        if (simulate_coin_flag) {
            //simulate coin insert for 50 millisecs via timer 2
            // chute#1 is switch10; strobe 1 ret 2, so matrix is 2,2 to set
            // chute#1 Stern is switch1; strobe 0 ret 0, so matrix is 1,0 to set
            //def lisy_timer( unsigned int duration, int command, int index)
            if (core_gameData->gen & (GEN_STMPU200 | GEN_STMPU100))
                SET_BIT(swMatrixLISY35[1], 0);
            else
                SET_BIT(swMatrixLISY35[2], 2);
            if (lisy_timer(50, 0, 2)) {
                if (core_gameData->gen & (GEN_STMPU200 | GEN_STMPU100)) {
                    CLEAR_BIT(swMatrixLISY35[2], 2);
                    if (ls80dbg.bitv.basic)
                        lisy80_debug("Stern coin simulated due to push&hold REPLAY Switch");
                } else {
                    CLEAR_BIT(swMatrixLISY35[2], 2);
                    if (ls80dbg.bitv.basic)
                        lisy80_debug("Bally coin simulated due to push&hold REPLAY Switch");
                }
                simulate_coin_flag = 0;
            }
        } else // bit is zero, reset timer index 0
        {
            lisy_timer(0, 1, 2);
        }
    } //freeplay option set
  
    return;
}

//get the status of the 'Selftest switch' (from internal matrix)
int
lisy35_get_SW_Selftest(void) {
    //Selftest is internal strobe 6, return 7
    return (CHECK_BIT(swMatrixLISY35[7], 7));
}

//get the status of the 'S33 switch' (from internal matrix)
//this is 'BY35_SWCPUDIAG' via NMI to clear statictics during selftest
int
lisy35_get_SW_S33(void) {
    //S33 Switch is internal strobe 6, return 6
    return (CHECK_BIT(swMatrixLISY35[7], 6));
}

void
lisy35_lamp_handler(int blanking, int board, int input, int inhibit) {
    int i, index, nu;
    // Aux board 1 is always a  AS-2518-23 60 lamps
    static unsigned char lamp[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static unsigned char old_lamp[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // five possible variants for Aux Board 2
    // NO_AUX_BOARD 0  // no aux board
    // AS_2518_43_12_LAMPS 1  // AS-2518-43 12 lamps
    // AS_2518_52_28_LAMPS 2  // AS-2518-52 28 lamps
    // AS_2518_23_60_LAMPS 3  // AS-2518-23 60 lamps
    //  AS_2518_147_LAMP_COMBO 4 //Lamp Solenoid Combo Goldball and Grandslam
    static unsigned char lamp2[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static unsigned char old_lamp2[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    //if hw check is not finished we do nothing
    if (!lisy35_bally_hw_check_finished)
        return;

    if (blanking == 0) //we set internal the matrix
    {
        if (board
            == 0) //main board is a AS-2518-23 60 lamps or a AS-2518-147 Lamp Solenoid Combo Goldball and Grandslam
        {
            if (lisy35_game.aux_lamp_variant == AS_2518_147_LAMP_COMBO) {
                //we use 'upper half' of main board, as Combo Board uses 30 lamps with PB2 & PB3 decoded
                //is it U3 ?
                if (inhibit & 0x04) {
                    index = input + 30;
                    lamp[index] = 1;
                }
                //is it U4 ?
                if (inhibit & 0x08) {
                    index = input + 45;
                    lamp[index] = 1;
                }
            } else {
                //is it U1 ?
                if (inhibit & 0x01) {
                    index = input;
                    lamp[index] = 1;
                }
                //is it U2 ?
                if (inhibit & 0x02) {
                    index = input + 15;
                    lamp[index] = 1;
                }
                //is it U3 ?
                if (inhibit & 0x04) {
                    index = input + 30;
                    lamp[index] = 1;
                }
                //is it U4 ?
                if (inhibit & 0x08) {
                    index = input + 45;
                    lamp[index] = 1;
                }
            }
        } //this was board 0, main board with 60 lamps
        else if (board == 1) {
            switch (lisy35_game.aux_lamp_variant) {
                case NO_AUX_BOARD:
                    //sanity check
                    if (ls80dbg.bitv.basic) {
                        sprintf(debugbuf, "input:%d out of range lamphandler board 1 but no AUX board", input);
                        lisy80_debug(debugbuf);
                    }
                    //signal that error
                    lisy80_set_red_led(1);
                    break;

                case AS_2518_147_LAMP_COMBO:
                    //sanity check
                    if (ls80dbg.bitv.basic) {
                        //is it U3 ?
                        if (inhibit & 0x04) {
                            index = input + 30;
                        }
                        //is it U4 ?
                        if (inhibit & 0x08) {
                            index = input + 45;
                        }

                        sprintf(debugbuf, "index:%d lamphandler board 1: AS_2518_147_LAMP_COMBO, ignored", index);
                        lisy80_debug(debugbuf);
                    }
                    break;

                case AS_2518_43_12_LAMPS:
                    //check index
                    if (input >= 3)
                        break; //index == 3 for this board is rest postion
                    //if( input > 3) { sprintf(debugbuf,"input:%d out of range lamphandler board 1",input); lisy80_debug(debugbuf);  return; }
                    //is it U1 ?
                    if (inhibit & 0x01) {
                        index = input;
                        lamp2[index] = 1;
                    }
                    //is it U2 ?
                    if (inhibit & 0x02) {
                        index = input + 3;
                        lamp2[index] = 1;
                    }
                    //is it U3 ?
                    if (inhibit & 0x04) {
                        index = input + 6;
                        lamp2[index] = 1;
                    }
                    //is it U4 ?
                    if (inhibit & 0x08) {
                        index = input + 9;
                        lamp2[index] = 1;
                    }
                    break;
                case AS_2518_52_28_LAMPS:
                    //check index
                    if (input >= 7)
                        break; //index == 7 for this board is rest postion
                    //is it U2 ?
                    if (inhibit & 0x01) {
                        index = input;
                        lamp2[index] = 1;
                    }
                    //is it U3 ?
                    if (inhibit & 0x02) {
                        index = input + 7;
                        lamp2[index] = 1;
                    }
                    //is it U4 ?
                    if (inhibit & 0x04) {
                        index = input + 14;
                        lamp2[index] = 1;
                    }
                    //is it U5 ?
                    if (inhibit & 0x08) {
                        index = input + 21;
                        lamp2[index] = 1;
                    }
                    break;
                case AS_2518_23_60_LAMPS:
                    //check index
                    if (input >= 15)
                        break; //index == 15 for this board is rest postion
                    //is it U1 ?
                    if (inhibit & 0x01) {
                        index = input;
                        lamp2[index] = 1;
                    }
                    //is it U2 ?
                    if (inhibit & 0x02) {
                        index = input + 15;
                        lamp2[index] = 1;
                    }
                    //is it U3 ?
                    if (inhibit & 0x04) {
                        index = input + 30;
                        lamp2[index] = 1;
                    }
                    //is it U4 ?
                    if (inhibit & 0x08) {
                        index = input + 45;
                        lamp2[index] = 1;
                    }
                    break;
                default:
                    if (ls80dbg.bitv.basic) {
                        sprintf(debugbuf, "unknow type of Aux board:%d", lisy35_game.aux_lamp_variant);
                        lisy80_debug(debugbuf);
                    }
                    //signal that error
                    lisy80_set_red_led(1);
                    break;
            }
        }  // second board
    } else //that is blanking, check internal matrix, set lamps and blank matrix afterwards
    {
        //check board 0
        for (i = 0; i <= 59; i++) {
            if (lamp[i] != old_lamp[i])
                lisy35_lamp_set(0, i, lamp[i]);
        }
        //we do blanking
        //store old lampvalue
        memcpy(old_lamp, lamp, sizeof(lamp));
        //and set lampmatirx to zero
        memset(lamp, 0, sizeof(lamp));

        //check board 1
        if (lisy35_game.aux_lamp_variant != NO_AUX_BOARD) {
            switch (lisy35_game.aux_lamp_variant) {
                case AS_2518_43_12_LAMPS:
                    nu = 11;
                    break;
                case AS_2518_52_28_LAMPS:
                    nu = 27;
                    break;
                case AS_2518_23_60_LAMPS:
                    nu = 59;
                    break;
                default:
                    nu = 0;
                    break;
            }
            for (i = 0; i <= nu; i++) {
                if (lamp2[i] != old_lamp2[i])
                    lisy35_lamp_set(1, i, lamp2[i]);
            }
            //we do blanking
            //store old lampvalue
            memcpy(old_lamp2, lamp2, sizeof(lamp2));
            //and set lampmatirx to zero
            memset(lamp2, 0, sizeof(lamp2));
        } //if second board present
    }
}

//shutdown lisy35
//RTH incomplete yet!
void
lisy35_shutdown(void) {
    struct timeval now;
    long seconds_passed;
    int tickles_per_second;

    //see what time is now
    //gettimeofday(&now,(struct timezone *)0);
    //calculate how many seconds passed
    //seconds_passed = now.tv_sec - lisy80_start_t.tv_sec;
    //and calculate how many tickles per second we had
    //tickles_per_second = no_of_tickles / seconds_passed;

    fprintf(stderr, "LISY35 graceful shutdown initiated\n");
    //fprintf(stderr,"run %ld seconds in total\n",seconds_passed);
    //fprintf(stderr,"we had appr. %d tickles per second\n",tickles_per_second);
    //fprintf(stderr,"and %ld throttles\n",no_of_throttles);
    //show the 'shutdown' message
    //display_show_shut_message();
    //put all coils and lamps to zero
    lisy35_coil_init();
    lisy35_lamp_init();
    lisy80_hwlib_shutdown();
}

/*
 throttle routine as with sound disabled
 lisy35 runs faster than the org game :-0
*/
void
lisy35_throttle(void) {
    static int first = 1;
    unsigned int now;
    int sleeptime;
    static unsigned int last;

    if (first) {
        first = 0;
        //store start time first, which is number of microseconds since wiringPiSetup (wiringPI lib)
        last = micros();
        //now we have core data so let us set internal soundboard type
        if (lisy_hardware_revision != 200) //not fo starship
            lisy35_set_soundboard_variant();
    }

    //if hw_check is not finished run with full speed, will reduce booting time
    if (!lisy35_bally_hw_check_finished)
        return;

    //do some usefull stuff
    //do we need to write to nvram?
    if (want_to_write_nvram) {
        if (lisy_timer(1000, 0, 3)) {
            lisy_timer(0, 1, 3); //reset timer
            want_to_write_nvram = 0;
            //lisy35_nvram_handler( 1, NULL);
            lisy_nvram_write_to_file();
            //debug
            if (ls80dbg.bitv.basic) {
                lisy80_debug("timer experid, nvram delayed write");
            }
        }
    }

    //if we are faster than throttle value which is per default 8000 usec (8 msec)
    // we choose 8msec as the throttle routine is placed within the zero-cross interrupt
    //we need to slow down a bit

    //for Starship use the time to update wheels
    if (lisy_hardware_revision == 200)
        wheels_refresh();

    //see how many micros passed
    now = micros();
    //beware of 'wrap' which happens each 71 minutes
    if (now < last)
        now = last; //we had a wrap

    //calculate difference and sleep a bit
    sleeptime = g_lisy35_throttle_val - (now - last);
    if (sleeptime > 0)
        delayMicroseconds(sleeptime);

    //store current time for next round with speed limitc
    last = micros();
}

//do set bally special game variant on the PICs
//called early from unix/main.c
void
lisy35_set_variant(void) {
    if (ls80dbg.bitv.basic) {
        sprintf(debugbuf, "Info: LISY35 will use special config variant:%d", lisy35_game.special_cfg);
        lisy80_debug(debugbuf);
    }

    //sanity action: we set all concurrent used I/Os to inputs at all times first
    lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_INPUT);
    lisy35_coil_set_direction_J4PIN8(LISY35_PIC_PIN_INPUT);
    lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_INPUT);
    lisy35_switch_pic_init(0);     //variant 0, only 4 strobes
    lisy35_display_set_variant(0); // * 0 default: 4 player display plus 1 status (6-digit)
    lisy35_J4PIN5_is_strobe = 0;
    lisy35_J4PIN8_is_strobe = 0;
    lisy35_display_set_variant(30); //Okaegi mod default off

    //inform display PIC (at least v406 needed) about different controlling
    //in case Okaegi 6to7digit conversion is used
    //we have added '10' to special cfg in csv in this case
    if (lisy35_game.special_cfg > 9) {
        if (ls80dbg.bitv.basic) {
            sprintf(debugbuf, "Info: LISY35 Okaegi mod activated (cfg is %d)", lisy35_game.special_cfg);
            lisy80_debug(debugbuf);
        }
        lisy35_okaegi_mod = 1;
        lisy35_game.special_cfg = lisy35_game.special_cfg - 10;
    }

    switch (lisy35_game.special_cfg) {
        case 1: // 4 player display plus 1 status (7-digit)
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            //switch matrix default
            lisy35_display_set_variant(1); //7-digit
                                           //all outputs from coil pic to activate
            lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J4PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            break;
        case 2: //six million dollar player5 & player6 strobe
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            //switch matrix default
            lisy35_display_set_variant(4);
            lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_OUTPUT);
            lisy35_J4PIN8_is_strobe = 1;
            break;
        case 3: //Fathom
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            lisy35_switch_pic_init(2);     //J4Pin8
            lisy35_display_set_variant(1); //7-digit
            lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_J4PIN8_is_strobe = 1;
            break;
        case 4: //Medusa
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            lisy35_switch_pic_init(2); //J4Pin8
            lisy35_display_set_variant(2);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_J4PIN5_is_strobe = 1;
            lisy35_J4PIN8_is_strobe = 1;
            break;
        case 5: //Elektra & Pac Man
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            //switch matrix default
            lisy35_display_set_variant(3);
            lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_J4PIN8_is_strobe = 1;
            break;
        case 6: //Vector
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            lisy35_switch_pic_init(1); //J4Pin5
            lisy35_display_set_variant(3);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_J4PIN5_is_strobe = 1;
            lisy35_J4PIN8_is_strobe = 1;
            break;
        case 7: //Spectrum & centaur
            fprintf(stderr, "Info: SPECIAL config for %s\n", lisy35_game.long_name);
            lisy35_switch_pic_init(1);     //J4Pin5
            lisy35_display_set_variant(1); //7-digit
            lisy35_coil_set_direction_J4PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_J4PIN5_is_strobe = 1;
            break;
        case 8: //Dolly Parton, Harlem, Paragon ; default with sound raw mode
            fprintf(stderr, "Info: default with Sound RAW for %s\n", lisy35_game.long_name);
            //switch matrix default
            //display default
            //all outputs from coil pic to activate
            lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J4PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            //sound in raw mode because of timing
            lisy35_coil_set_sound_raw(1);
            lisy35_sound_raw = 1;
            break;
        default:
            fprintf(stderr, "Info: NO special config for %s\n set to default", lisy35_game.long_name);
            //switch matrix default
            //display default
            //all outputs from coil pic to activate
            lisy35_coil_set_direction_J4PIN5(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J4PIN8(LISY35_PIC_PIN_OUTPUT);
            lisy35_coil_set_direction_J1PIN8(LISY35_PIC_PIN_OUTPUT);
            break;
    }

    //Okaegi mod?
    //this is handled like 7digit in pinmame
    //but we need to inform display PIC about different handling
    if (lisy35_okaegi_mod == 1)
        lisy35_display_set_variant(31);

    //set the right AUX Lampdriverboard variant
    lisy35_coil_set_lampdriver_variant(lisy35_game.aux_lamp_variant);
    if (ls80dbg.bitv.basic) {
        sprintf(debugbuf, "Info: LISY35 will use AUX lamp variant:%d", lisy35_game.aux_lamp_variant);
        lisy80_debug(debugbuf);
    }

    //set the right soundboard variant for lisy35
    //this is done separatly as we want to read SB version from pinmame core
}

//set the right soundboard variant for lisy35
//this is done separatly called by lisy35_throttle (once)
//as we want to read SB version from pinmame core
//we use by35.h:#define BY35GD_NOSOUNDE 0x01 // doesn't use SOUNDE
void
lisy35_set_soundboard_variant(void) {
    //read Soundboard type it from pinmame core
    if (core_gameData->hw.soundBoard == 0)
        lisy35_game.soundboard_variant = LISY35_SB_CHIMES; //no soundboard
    else if ((core_gameData->hw.gameSpecific1 & 0x01) == 0)
        lisy35_game.soundboard_variant = LISY35_SB_STANDARD;
    else
        lisy35_game.soundboard_variant = LISY35_SB_EXTENDED;

    //if it is an extended soundboard, set the subtype as timing is different for 2581-51 and S&T
    if (lisy35_game.soundboard_variant == LISY35_SB_EXTENDED) {
        if (core_gameData->hw.soundBoard == SNDBRD_BY51)
            lisy35_coil_set_extended_SB_type(0);
        //else if (core_gameData->hw.soundBoard == SNDBRD_BY56) lisy35_coil_set_extended_SB_type(0); //Xenon
        else
            lisy35_coil_set_extended_SB_type(1);
    }

    //debug?
    if (ls80dbg.bitv.basic) {
        switch (lisy35_game.soundboard_variant) {
            case LISY35_SB_CHIMES:
                sprintf(debugbuf, "Info: LISY35 will use soundboard variant 0 (Chimes)");
                break;
            case LISY35_SB_STANDARD:
                if (lisy35_sound_raw)
                    sprintf(debugbuf, "Info: LISY35 will use soundboard variant 1 (standard SB in RAW Mode)");
                else
                    sprintf(debugbuf, "Info: LISY35 will use soundboard variant 1 (standard SB)");
                break;
            case LISY35_SB_EXTENDED:
                //if ((core_gameData->hw.soundBoard == SNDBRD_BY51) | (core_gameData->hw.soundBoard == SNDBRD_BY56))
                if ((core_gameData->hw.soundBoard == SNDBRD_BY51))
                    sprintf(debugbuf, "Info: LISY35 will use soundboard variant 2 (EXTENDED SB Type 2581-51)");
                else
                    sprintf(debugbuf, "Info: LISY35 will use soundboard variant 2 (EXTENDED SB Type S&T)");
                break;
        }
        lisy80_debug(debugbuf);
    }
}

//read dipswitchsettings for specific game/mpu
//and give back settings or -1 in case of error
//switch_nr is 0..3
//do also start routine to collect HW params out of pinmame
int
lisy35_get_mpudips(int switch_nr) {

    int ret;
    char dip_file_name[80];
    static unsigned char first_time = 1;

    //if hw check is not finished we do nothing
    //and return -1, by35.c will then use pinmame settings
    if (!lisy35_bally_hw_check_finished)
        return -1;

    //use function from fileio.c
    if (lisy_hardware_revision == 200)
        ret = lisy200_file_get_mpudips(switch_nr, ls80dbg.bitv.basic, dip_file_name);
    else
        ret = lisy35_file_get_mpudips(switch_nr, ls80dbg.bitv.basic, dip_file_name);
    //debug?
    if (first_time) //only one Info line
    {
        fprintf(stderr, "Info: DIP switch settings according to %s\n", dip_file_name);
    }

    //set back flag
    first_time = 0;

    //return setting, this will be -1 if 'first_time' failed
    return ret;
}

//lisy35_nvram_handler
//store pos of bally nvram for later use
lisy35_cmos_data_t* lisy_by35_CMOS;

void
lisy35_nvram_handler(void* by35_CMOS_Bally) {
    //remember address for subsequent actions
    lisy_by35_CMOS = by35_CMOS_Bally;
}

//handling of nvram via eeprom
//read_or_write = 0 means read
//saw never 1 with Bally for write, will be '2' out of cmos routine in by35.c
//we trust eeprom write routine which will only write changed bytes
int
lisy35_nvram_handler_old(int read_or_write, uint8_t* by35_CMOS_Bally) {

    static unsigned char first_time = 1;
    static eeprom_block_t nvram_block;
    static eeprom_block_t lisy35_block;
    static uint8_t* by35_CMOS;
    unsigned char sw_main, sw_sub, commit;
    int i, ret;

    //check content at first time
    if (first_time) {
        //we need a read for the init, so ignore write requests for now
        if (read_or_write)
            return 0;

        //remember address for subsequent actions
        by35_CMOS = by35_CMOS_Bally;

        first_time = 0;
        //do we have a valid signature in block 1? -> 1 =yes
        //if not init eeprom note:game nr is 80 here
        if (!lisy_eeprom_checksignature()) {
            ret = lisy_eeprom_init();
            if (ls80dbg.bitv.basic) {
                sprintf(debugbuf, "write nvram INIT done:%d", ret);
                lisy80_debug(debugbuf);
            }
        }
        //read lisy80 block
        lisy_eeprom_256byte_read(lisy35_block.byte, 1);

        //if the stored game number is not the current one
        //initialize nvram block with zeros and write to eeprom
        if (lisy35_block.content.gamenr != lisy35_game.gamenr) {
            for (i = 0; i <= 255; i++)
                nvram_block.byte[i] = '\0';
            ret = lisy_eeprom_256byte_write(nvram_block.byte, 0);
            if (ls80dbg.bitv.basic) {
                sprintf(debugbuf, "stored game nr not current one, we init with zero:%d", ret);
                lisy80_debug(debugbuf);
            }

            //prepare new gane number to write
            lisy35_block.content.gamenr = lisy35_game.gamenr;
        }

        //get sw version
        lisy_get_sw_version(&sw_main, &sw_sub, &commit);

        //now update statistics
        lisy35_block.content.starts++;
        if (ls80dbg.bitv.basic)
            lisy35_block.content.debugs++;
        if (lisy35_game.gamenr > 63) {
            lisy35_block.content.counts[63]++;
        } //RTH Limitation from LISY80
        else {
            lisy35_block.content.counts[lisy35_game.gamenr]++;
        }

        lisy35_block.content.Software_Main = sw_main;
        lisy35_block.content.Software_Sub = sw_sub;
        ret = lisy_eeprom_256byte_write(lisy35_block.byte, 1);
        if (ls80dbg.bitv.basic) {
            sprintf(debugbuf, "nvram statistics updated for game:%d", lisy35_block.content.gamenr);
            lisy80_debug(debugbuf);
        }

    } //first time only

    if (read_or_write) //1 = write
    {

        ret = lisy_eeprom_256byte_write((uint8_t*)by35_CMOS, 0);
        /* RTH debug in eeprom pic routine
 if ( ls80dbg.bitv.basic )
  {
        sprintf(debugbuf,"LISY35 write nvram done:%d",ret);
        lisy80_debug(debugbuf);
  }
*/
    } else //we want to read
    {
        ret = lisy_eeprom_256byte_read(nvram_block.byte, 0);
        //cop to mem if success
        if (ret == 0)
            memcpy((char*)by35_CMOS, nvram_block.byte, 256);
        if (ls80dbg.bitv.basic) {
            sprintf(debugbuf, "read nvram done:%d", ret);
            lisy80_debug(debugbuf);
        }
    }

    return (ret);
}

//solenoid handler
//called from by35.c
void
lisy35_solenoid_handler(unsigned char data, unsigned char soundselect) {

    static unsigned char old_moment_data = 0x0f; //from init 15 is rest (all OFF)
    static unsigned char old_cont_data = 0x0f;   //from init activ low
    unsigned char cont_data, moment_data;

    //if hw check is not finished we do nothing
    //if ( !lisy35_bally_hw_check_finished) return;  RTH:really?

    cont_data = data & 0xf0;
    cont_data >>= 4; //we need the lower 4 bits
    moment_data = data & 0x0f;

    //special cases as bally use some normally 'unused' pins for switch or display strobe
    //with LISY35 we filter ithese out to limit I2C traffic, as strobes are all done by PICs themself
    if (lisy35_J4PIN5_is_strobe)
        cont_data &= 0x0e;
    if (lisy35_J4PIN8_is_strobe)
        cont_data &= 0x07;

    //handle continuous solenoids
    //new cont data?
    if ((old_cont_data != cont_data) & (lisy35_bally_hw_check_finished == 1)) {
        //running on Starship?
        if (lisy_hardware_revision == 200)
            lisy_home_ss_event_handler(LISY_HOME_SS_EVENT_CONT_SOL, cont_data, 0, 0);

        //check for flipper disable
        if (CHECK_BIT(cont_data, 2) && !CHECK_BIT(old_cont_data, 2)) {
            //lisy35_nvram_handler( 1, NULL);
            //want_to_write_nvram = 1; RTH test no delayd write
            lisy_nvram_write_to_file();
            //debug
            if (ls80dbg.bitv.basic) {
                //lisy80_debug("flipper disabled we do ASK for a nvram write");
                lisy80_debug("flipper disabled we do a nvram write");
            }
        }

        //debug?
        if (ls80dbg.bitv.coils) {
            if (CHECK_BIT(cont_data, 3) != CHECK_BIT(old_cont_data, 3)) {
                if (CHECK_BIT(cont_data, 3))
                    lisy80_debug("cont sol changed: bit 3 set to 1");
                else
                    lisy80_debug("cont sol changed: bit 3 set to 0");
            }
            if (CHECK_BIT(cont_data, 2) != CHECK_BIT(old_cont_data, 2)) {
                if (CHECK_BIT(cont_data, 2))
                    lisy80_debug("cont sol changed: flipper disabled");
                else
                    lisy80_debug("cont sol changed: flipper enabled");
            }
            if (CHECK_BIT(cont_data, 1) != CHECK_BIT(old_cont_data, 1)) {
                if (CHECK_BIT(cont_data, 1))
                    lisy80_debug("cont sol changed: coin lockout ON");
                else
                    lisy80_debug("cont sol changed: coin lockout OFF");
            }
            if (CHECK_BIT(cont_data, 0) != CHECK_BIT(old_cont_data, 0)) {
                if (CHECK_BIT(cont_data, 0))
                    lisy80_debug("cont sol changed: bit 0 set to 1");
                else
                    lisy80_debug("cont sol changed: bit 0 set to 0");
            }
        }

        old_cont_data = cont_data;
        //send to PIC
        lisy35_cont_coil_set(cont_data);
    } //if new cont data available

    //we look at momentary data only if Solenoids are selected
    if (!soundselect) {
        //new momentary data?
        if ((old_moment_data != moment_data)) {
            old_moment_data = moment_data;

            //just send solenoid data to PIC
            lisy35_mom_coil_set(moment_data);
            if (ls80dbg.bitv.coils) {
                if (moment_data == 15) {
                    sprintf(debugbuf, "momentary solenoids OFF");
                } else {
                    sprintf(debugbuf, "momentary solenoids: %d", moment_data + 1);
                }

                lisy80_debug(debugbuf);
            }

            //running on Starship?
            if (lisy_hardware_revision == 200)
                lisy_home_ss_event_handler(LISY_HOME_SS_EVENT_MOM_SOL, moment_data, 0, 0);

            //JustBoom Sound? in case of chimes we may want to play wav files here
            if (lisy35_has_own_sounds) {
		//play sound if Q1,Q5,6,7, this maps to moment data 1..4
                if ((lisy35_game.soundboard_variant == LISY35_SB_CHIMES) & (moment_data >= 1) & (moment_data <= 4)) {
                    lisy35_play_wav(moment_data);
                }
            } //has own sounds

        } //if new momentary data available
    }     //solenoids selected
} //solenoid_handler

//sound handler
//called from by35.c
//with data if sounds are selected or with ctrl ( sound_E & Sound_select )
//the IDs for the sound handler (called from by35.c)
//LISY35_SOUND_HANDLER_IS_DATA (0)  -> sound data
//LISY35_SOUND_HANDLER_IS_CTRL (1)  -> Sound_e in bit 1, Sound_select in bit 0
void
lisy35_sound_handler(unsigned char type, unsigned char data) {

    static unsigned char last_sound_E = 7; //inital value to make sure first one is send
    static unsigned char last_sound_select = 0;
    static unsigned char sound_int_occured = 0;
    static unsigned char count = 0; //we collect two bytes for soundboard
    static unsigned char first_nybble;
    unsigned char sound_select;
    unsigned char sound_E;

    //control data
    if (type == LISY35_SOUND_HANDLER_IS_CTRL) {
        if (CHECK_BIT(data, 0))
            sound_select = 1;
        else
            sound_select = 0;
        if (CHECK_BIT(data, 1))
            sound_E = 1;
        else
            sound_E = 0;

        //what soundboardvariant do we have
        if (lisy35_game.soundboard_variant != LISY35_SB_EXTENDED) {
            //standard SB or chimes just send
            // send  select state
            //we only send this if it changed from last value
            if (last_sound_select != sound_select) {
                //set in sound_raw mode only
                if (lisy35_sound_raw)
                    lisy35_coil_set_sound_select(sound_select);
                //was there a move from 0->1 for sound select status?
                if ((last_sound_select == 0) & (sound_select == 1)) {
                    sound_int_occured = 1;
                    if (ls80dbg.bitv.sound)
                        lisy80_debug("sound interrupt occured");
                }
                last_sound_select = sound_select;
            } //sound_select

            //send sound_E
            //we only send this if it changed from last value and HW check is over
            if ((last_sound_E != sound_E) & (lisy35_bally_hw_check_finished == 1)) {
                lisy35_display_set_soundE(sound_E);
                last_sound_E = sound_E;
                //debug
                if (ls80dbg.bitv.sound) {
                    sprintf(debugbuf, "switch sound E line to:%d", sound_E);
                    lisy80_debug(debugbuf);
                }
            } //if sound_E changed
        }     //no extended SB
        else {
            //was there a move from 0->1 for sound select status?
            if ((last_sound_select == 0) & (sound_select == 1)) {
                sound_int_occured = 1;
            }

            //store the status
            last_sound_select = sound_select;
        }
    }

    //data for soundboard if we had an sound interrupt
    if ((type == LISY35_SOUND_HANDLER_IS_DATA) & (sound_int_occured)) {
        //what soundboardvariant do we have
        if (lisy35_game.soundboard_variant != LISY35_SB_EXTENDED) {
            //do we have raw or cooked mode
            if (lisy35_sound_raw) {
                //send sound to PIC
                lisy35_sound_std_sb_set(data);
                //debug?
                if (ls80dbg.bitv.sound) {
                    sprintf(debugbuf, "sound data(standard sb RAW mode): 0x%02x)", 16 * last_sound_E + data);
                    lisy80_debug(debugbuf);
                }
                count++;        //count the bytes
                if (count == 2) //second byte, reset int condition
                {
                    count = 0;
                    sound_int_occured = 0;
                }
            } //raw mode
            else {
                //send sound to PIC, which will handle timing in cooked mode
                lisy35_sound_std_sb_set(data);
                // reset int condition
                sound_int_occured = 0;
                //JustBoom Sound? we may want to play wav files here
                if (lisy35_has_own_sounds)
                    lisy35_play_wav(16 * last_sound_E + data);
                //Starship? inform even handler
                //if ( StarShip_has_own_sounds ) -> decision made in lisy_home.c
                lisy_home_ss_event_handler(LISY_HOME_SS_EVENT_SOUND,16*last_sound_E + data,0,0);
                //debug?
                if (ls80dbg.bitv.sound) {
                    sprintf(debugbuf, "sound data(standard sb): 0x%02x)", 16 * last_sound_E + data);
                    lisy80_debug(debugbuf);
                }
            } //cooked mode
        }     //standard SB or chimes
        else {
            //extended soundboard, we do expect two nibble
            if (count == 0) //first nibble, store it
            {
                first_nybble = data;
                count = 1;
            } else {
                //we got second nybble, send with first nybble both to PIC
                lisy35_sound_ext_sb_set(first_nybble); //LSB
                lisy35_sound_ext_sb_set(data);         //MSB
                //JustBoom Sound? we may want to play wav files here
                if (lisy35_has_own_sounds)
                    lisy35_play_wav(16 * data + first_nybble);
                //debug?
                if (ls80dbg.bitv.sound) {
                    sprintf(debugbuf, "sound data(extended sb): 0x%x%x", data, first_nybble);
                    lisy80_debug(debugbuf);
                }
                count = 0;
                sound_int_occured = 0;
            }
        } //extended SB
    }     //data for soundboard
} //sound_handler
