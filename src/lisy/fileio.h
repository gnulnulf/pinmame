// license:BSD-3-Clause

#ifndef _FILEIO_H
#define _FILEIO_H

typedef struct {
    unsigned char gamenr;   // internal game number ( 0..63)
    unsigned char is80B;    // >0 means 80B, taken from dipswitch
    char type_from_csv[10]; //type string from CSV SYS80|SYS80A|SYS80B
    char gamename[10];      // game name mame fromat (8 chars)
    char long_name[40];     // game name from csv
    int gtb_no;             // Gottlieb number from csv
    int throttle;           // throttle value per Gottlieb game, default is 1000
    char comment[80];       // comment from csv
} t_stru_lisy80_games_csv;

typedef struct {
    unsigned char gamenr; // internal game number ( 0..63)
    char gamename[10];    // game name mame fromat (8 chars)
    char long_name[40];   // game name from csv
    char rom_id[10];      // system1 rom-id (1 char)
    int throttle;         // throttle value per Gottlieb game, default is 3000
    double clockscale;    // clockscale value per Gottlieb game, default is  0.5
    char comment[80];     // comment from csv
    char lamp17_is_Coil;  // will be 1 for Joker Poker, Countdown,Pinball-Pool and Buck Rogers
    char lamp18_is_Coil;  // will be 1 for Countdown
} t_stru_lisy1_games_csv;

typedef struct {
    unsigned char gamenr;   // internal game number ( 0..63)
    char gamename[10];      // game name mame fromat (8 chars)
    char long_name[80];     // game name from csv
    int throttle;           // throttle value per Bally game, default is 1000
    int special_cfg;        // is there a special config? (some games )
    int aux_lamp_variant;   //  what AUX lampdriverboard variant the game has
    int soundboard_variant; //  what soundboard variant the game has
    char comment[80];       // comment from csv
} t_stru_lisy35_games_csv;

typedef struct {
    unsigned char gamenr; // internal game number ( 0..63)
    char gamename[10];    // game name mame fromat (8 chars)
    char long_name[80];   // game name from csv
    char type[10];        // type of game ( e.g. SYS7 for Williams system7)
    unsigned char typeno; // type of game , internal number
    int throttle;         // throttle value per game, default is 300
    char comment[80];     // comment from csv
} t_stru_lisymini_games_csv;

typedef struct {
    unsigned char volume;        // volume of this sound (128 is max)
    unsigned char loop;          // sound needs to be playd in a 'loop' (e.g. background sound)
    unsigned char not_int_loops; // sound will not interrupt loop sounds
} t_stru_lisy80_sounds_csv;

typedef struct {
    int soundnumber;         //(pinmame) number of sound
    char* path;              //path to soundfile
    char* name;              //name of soundfile
    int option;              //option
    char* comment;           //comment
    int how_many_versions;   // to handle sounds with alternative versions, contains 1 to 4 (not read from csv file)
    int last_version_played; // last version of the sound played, 0 = never played (not read from csv file)
    int trigger;             //StarShip : play when closed(1) or open(0)
    int wait;                //StarShip: wait until sound is finished
    int delay;               //StarShip: delay after sound start ( block pinmame)
    int onlyactiveingame;    //StarShip: some switches (e.g.coins) need always to produce sound
    int preload;             //StarShip: preload the sound
} t_stru_lisy35_sounds_csv;

int lisy80_file_get_gamename(t_stru_lisy80_games_csv* lisy80_game);
int lisy80_file_get_mpudips(int switch_nr, int debug, char* dip_setting_filename);
unsigned char lisy80_file_get_onedip(int dip_nr, char* dip_comment, char* dip_setting_filenamee, int re_init);
int lisy80_file_write_dipfile(int mode, char* line);
int lisy80_file_get_soundopts(void);
int lisy80_file_get_coilopts(void);

int lisy1_file_get_gamename(t_stru_lisy1_games_csv* lisy1_game);
int lisy1_file_get_mpudips(int switch_nr, int debug, char* dip_setting_filename);
unsigned char lisy1_file_get_onedip(int dip_nr, char* dip_comment, char* dip_setting_filenamee, int re_init);
int lisy1_file_write_dipfile(int mode, char* line);
int lisy1_file_get_coilopts(void);
int  lisy1_file_get_attractopts(unsigned char command, unsigned char *cmd, long *num, unsigned char *opt, char *textorpath);

int lisy35_file_get_gamename(t_stru_lisy35_games_csv* lisy35_game);
int lisy35_file_get_mpudips(int switch_nr, int debug, char* dip_setting_filename);
unsigned char lisy35_file_get_onedip(int dip_nr, char* dip_comment, char* dip_setting_filenamee, int re_init);
int lisy35_file_write_dipfile(int mode, char* line);
int lisy35_file_get_soundopts(void);
int lisy200_file_get_mpudips(int switch_nr, int debug, char* dip_setting_filename);
unsigned char lisy200_file_get_onedip(int dip_nr, char* dip_comment, char* dip_setting_filenamee, int re_init);

int lisymini_file_get_gamename(t_stru_lisymini_games_csv* lisymini_game);
int lisyapc_file_get_gamename(t_stru_lisymini_games_csv* lisymini_game);
int lisy_m_file_get_hwrules(void);

int lisy_file_get_welcome_msg(char* message);
int lisy_file_get_volume_level(void);

//fadecandy stuff
int lisy_file_get_led_mappings(unsigned char system);

//lisy home stuff
int lisy_file_get_home_mappings(void);
int lisy_file_get_home_ss_lamp_mappings(int variant);
int lisy_file_get_home_ss_coil_mappings(int variant);
int lisy_file_get_home_ss_special_coil_mappings(int variant);
int lisy_file_get_home_ss_special_lamp_mappings(int variant);
int lisy_file_get_home_ss_general(void);
int  lisy_file_get_home_ss_sound_mappings(int variant);

//lisy200 (Starship) stuff
int lisy200_file_get_mpudips(int switch_nr, int debug, char* dip_setting_filename);
unsigned char lisy200_file_get_onedip(int dip_nr, char* dip_comment, char* dip_setting_filenamee, int re_init);
int lisy_file_get_home_ss_GI(int variant);
int lisy200_file_get_soundopts(void);

/* LISY80 stuff */
#define LISY80_GAMES_CSV                   "/boot/lisy/lisy80/cfg/lisy80games.csv"
#define LISY80_GAMES_7DIGIT_CSV            "/boot/lisy/lisy80/cfg/lisy80games_7digit.csv"
#define LISY80_FIRST_80B                   40
#define LISY80_DIPS_PATH                   "/boot/lisy/lisy80/dips/"
#define LISY80_DIPS_FILE                   "_lisy80_dips.csv"
#define LISY80_SOUND_PATH                  "/boot/lisy/lisy80/sounds/"
#define LISY80_SOUND_FILE                  "_lisy80_sounds.csv"
#define LISY80_COIL_PATH                   "/boot/lisy/lisy80/coils/"
#define LISY80_COIL_FILE                   "_lisy80_coils.csv"
#define LISY80_FADECANDY_PATH              "/boot/lisy/lisy80/fadecandy/"
#define LISY80_FADECANDY_LAMP_FILE         "_lisy80_fadecandy_lamps.csv"
#define LISY80_FADECANDY_GI_FILE           "_lisy80_fadecandy_GI.csv"

/* LISY1 stuff */
#define LISY1_GAMES_CSV                    "/boot/lisy/lisy1/cfg/lisy1games.csv"
#define LISY1_DIPS_PATH                    "/boot/lisy/lisy1/dips/"
#define LISY1_DIPS_FILE                    "_lisy1_dips.csv"
#define LISY1_SOUND_PATH                   "/boot/lisy/lisy1/sounds/"
#define LISY1_COIL_PATH                    "/boot/lisy/lisy1/coils/"
#define LISY1_COIL_FILE                    "_lisy1_coils.csv"
#define LISY1_FADECANDY_PATH               "/boot/lisy/lisy1/fadecandy/"
#define LISY1_FADECANDY_LAMP_FILE          "_lisy1_fadecandy_lamps.csv"
#define LISY1_FADECANDY_GI_FILE            "_lisy1_fadecandy_GI.csv"
/* LISY1 attract mode stuff */
#define LISY1_ATTRACT_PATH                 "/boot/lisy/lisy1/attract/"
#define LISY1_ATTRACT_FILE                 "_attract.csv"
#define LISY1_ATTRACT_INIT                 0
#define LISY1_ATTRACT_START                1
#define LISY1_ATTRACT_STOP                 2
#define LISY1_ATTRACT_STEP                 3
#define LISY1_ATTRACT_CMD_TIME             0
#define LISY1_ATTRACT_CMD_TIME_OPT_S       1
#define LISY1_ATTRACT_CMD_TIME_OPT_MS      2
#define LISY1_ATTRACT_CMD_LAMP             3
#define LISY1_ATTRACT_CMD_LAMP_OFF         4
#define LISY1_ATTRACT_CMD_LAMP_ON          5
#define LISY1_ATTRACT_CMD_COIL             6
#define LISY1_ATTRACT_CMD_COIL_PULSE       7
#define LISY1_ATTRACT_CMD_WAV              8
#define LISY1_ATTRACT_CMD_SPEECH           9

/* LISY35 stuff */
#define LISY35_GAMES_CSV                   "/boot/lisy/lisy35/cfg/lisy35games.csv"
#define LISY35_DIPS_PATH                   "/boot/lisy/lisy35/dips/"
#define LISY35_DIPS_FILE                   "_lisy35_dips.csv"
#define LISY35_FADECANDY_PATH              "/boot/lisy/lisy35/fadecandy/"
#define LISY35_FADECANDY_LAMP_FILE         "_lisy35_fadecandy_lamps.csv"
#define LISY35_FADECANDY_GI_FILE           "_lisy35_fadecandy_GI.csv"
#define LISY35_SOUND_PATH                  "/boot/lisy/lisy35/sounds/"
#define LISY35_SOUND_FILE                  "_lisy35_sounds.csv"

/* LISY Home stuff */
#define LISYH_MAPPING_PATH                 "/boot/lisy/lisyH/mapping/"
#define LISYH_LAMP_MAPPING_FILE            "lamps.csv"
#define LISYH_COIL_MAPPING_FILE            "coils.csv"
#define LISYH_SS_LAMP_MAPPING_FILE         "ss_lamps"
#define LISYH_SS_COIL_MAPPING_FILE         "ss_coils"
#define LISYH_SS_SPECIAL_COIL_MAPPING_FILE "ss_special_coils"
#define LISYH_SS_SPECIAL_LAMP_MAPPING_FILE "ss_special_lamps"
#define LISYH_SS_GI_LIST                   "ss_GI_list"
#define LISYH_SS_GENERAL                   "ss_general_parms"
#define LISYH_SS_SOUND_MAPPING_FILE        "ss_sounds"

/* LISYMINI stuff */
#define LISYMINI_GAMES_CSV                 "/boot/lisy/lisy_m/cfg/lisyminigames.csv"

/* APC stuff */
#define APC_HW_RULES_PATH                  "/boot/lisy/lisy_m/coils/"
#define APC_HW_RULES_FILE                  "_APC_hw_rules.csv"

/* generalLISY stuff */
#define LISY_WELCOME_MSG_FILE              "/boot/lisy/lisy/Welcome.msg"
#define LISY_VOLUME_LEVEL                  "/boot/lisy/lisy/volume_level"

/* LISY200 (Starship) stuff */
#define LISY200_DIPS_PATH                  "/boot/lisy/lisyH/dips/"
#define LISY200_DIPS_FILE                  "Starship_dips.csv"
#define LISY200_SOUND_PATH                 "/boot/lisy/lisyH/sounds/StarShip/"
#define LISY200_SOUND_FILE                 "sounds.csv"

#endif // _FILEIO_H
