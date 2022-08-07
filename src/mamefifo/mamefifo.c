//#ifndef __MAMEFIFO_C_
//#define __MAMEFIFO_C_
//#endif


#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <signal.h>

// mame_fifo globals
    int fifofd;
    char *myfifo = "/tmp/mypipe";
    FILE *mamefifo;

void mame_fifo_init()
{
    fprintf(stdout,"Open mamefifo %s\n",myfifo);
    
    //unlink(myfifo);
    // make pipe
    //mkfifo(myfifo, 0666);

    // open it
    //fifofd = open("/tmp/mypipe",  O_WRONLY | O_NONBLOCK);
    fprintf(stdout,"Waiting for reader on %s\n",myfifo);
    fifofd = open("/tmp/mypipe",  O_WRONLY );
    //fifofd = open(myfifo, O_CREAT | O_RDWR);

    // make non-blocking
    //fcntl(fifofd, F_SETFL, fcntl(fifofd, F_GETFL) | O_NONBLOCK);



    // make a file descriptor for pipe
    mamefifo = fdopen(fifofd, "w");
//    mamefifo = fopen(myfifo, "a");

    fprintf(mamefifo,"Hello mamefifo\n");
    fflush(mamefifo);
    fprintf(mamefifo,"Hello2 mamefifo\n");
    fflush(mamefifo);

}


void mame_fifo_close()
{
    fprintf(mamefifo,"Close mamefifo\n");
    fflush(mamefifo);

    fprintf(stdout,"Close mamefifo\n");
    fclose(mamefifo);

    //unlink(myfifo);
}


/*
static volatile sig_atomic_t keep_going = 1;

static void signal_handler(int signum)
{
    keep_going = 0;
}

int mame_fifo_init()
{
    int fifofd;
    char *myfifo = "/tmp/mypipe";
    char *line;
    ssize_t amount_read;
    ssize_t len;
    FILE *mamefifo;
		char sentence[1000];
		//sentence="Hallo";

    signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);

    // make pipe
    mkfifo(myfifo, 0666);

    // open it
    fifofd = open(myfifo, O_RDWR);

    // make non-blocking
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    // make a file descriptor for pipe
    f = fdopen(fd, "rw");

//printf("Enter a sentence:\n");
 //   fgets(sentence, sizeof(sentence), stdin);
    fprintf(f, "%s", sentence);
    fprintf(stdout,"Hello from mamefifo\n");
		//fprintf(f,"%s",sentence);
 / *   while(keep_going)
    {
        amount_read = getline(&line, &len, f);
        if (amount_read > 0)
            printf("%s",line);
        if (amount_read == -1)
        {
            clearerr(f);
        }
     }
* /
    printf("quitting...\n");

    close(fd);

    unlink(myfifo);

    return 0;
}
*/
