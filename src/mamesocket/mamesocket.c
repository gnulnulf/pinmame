#include "mamesocket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "16914"  // port we're listening on

#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

fd_set mame_socket_master;    // master file descriptor list
fd_set mame_socket_read_fds;  // temp file descriptor list for select()
int mame_socket_fdmax;        // maximum file descriptor number

int mame_socket_listener;  // listening socket descriptor
int mame_socket_newfd;     // newly accept()ed socket descriptor
struct sockaddr_storage mame_socket_remoteaddr;  // client address
socklen_t mame_socket_addrlen;

char mame_socket_buf[4096];  // buffer for client data
int mame_socket_nbytes;

FILE *mame_socket_fd;

char mame_socket_remoteIP[INET6_ADDRSTRLEN];

void mame_socket_init() {
    fprintf(stdout, "Init mamesocket server\n");
    mame_socket_bind();
}

void mame_socket_close() { fprintf(stdout, "Close mamesocket server\n"); }

void mame_socket_send_all(char *str) {
    int j;
    int len = strlen(str);
    for (j = 0; j <= mame_socket_fdmax; j++) {
        // send to everyone!
        if (FD_ISSET(j, &mame_socket_master)) {
            // except the listener and ourselves
            if (j != mame_socket_listener && j != mame_socket_listener) {
                if (send(j, str, len, 0) == -1) {
                    perror("send");
                    fprintf(stdout, "SOCKET: %s", mame_socket_buf);
                }
            }
        }
    }
}

void mame_socket_send(int s, char *str) {
    int len = strlen(str);
    if (FD_ISSET(s, &mame_socket_master)) {
        // except the listener and ourselves
        if (s != mame_socket_listener && s != mame_socket_listener) {
            if (send(s, str, len, 0) == -1) {
                perror("send");
                fprintf(stdout, "SOCKET: %s", mame_socket_buf);
            } else {
                fprintf(stdout, "SOCKETSEND: %d = %s", s,mame_socket_buf);
            }
        }
    }
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int setNonblocking(int fd) {
    int flags;
    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

void mame_socket_bind(void) {
    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;
    struct addrinfo hints, *ai, *p;

    FD_ZERO(&mame_socket_master);  // clear the master and temp sets
    FD_ZERO(&mame_socket_read_fds);

    fprintf(stdout, "Bind mamesocket server\n");

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        mame_socket_listener =
            socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (mame_socket_listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(mame_socket_listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int));

        if (bind(mame_socket_listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(mame_socket_listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai);  // all done with this
    // listen
    if (listen(mame_socket_listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(mame_socket_listener, &mame_socket_master);
    setNonblocking(mame_socket_listener);
    // keep track of the biggest file descriptor
    mame_socket_fdmax = mame_socket_listener;  // so far, it's this one
}

void mame_socket_loop() {
    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    int socketnr, j, rv;
    struct addrinfo hints, *ai, *p;

    struct timeval tv;
    // fd_set readfds;
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    // main loop
    mame_socket_read_fds = mame_socket_master;  // copy it
    if (select(mame_socket_fdmax + 1, &mame_socket_read_fds, NULL, NULL, &tv) ==
        -1) {
        perror("select");
        exit(4);
    }

    // run through the existing connections looking for data to read
    for (socketnr = 0; socketnr <= mame_socket_fdmax; socketnr++) {
        if (FD_ISSET(socketnr, &mame_socket_read_fds)) {  // we got one!!
            printf("Got socket data on %d!\n", socketnr);

            if (socketnr == mame_socket_listener) {
                // handle new connections
                mame_socket_addrlen = sizeof mame_socket_remoteaddr;
                mame_socket_newfd =
                    accept(mame_socket_listener,
                           (struct sockaddr *)&mame_socket_remoteaddr,
                           &mame_socket_addrlen);

                if (mame_socket_newfd == -1) {
                    perror("accept");
                } else {
                    FD_SET(mame_socket_newfd,
                           &mame_socket_master);  // add to master set
                    if (mame_socket_newfd >
                        mame_socket_fdmax) {  // keep track of the max
                        mame_socket_fdmax = mame_socket_newfd;
                    }
                    printf(
                        "selectserver: new connection from %s on "
                        "socket %d\n",
                        inet_ntop(
                            mame_socket_remoteaddr.ss_family,
                            get_in_addr(
                                (struct sockaddr *)&mame_socket_remoteaddr),
                            mame_socket_remoteIP, INET6_ADDRSTRLEN),
                        mame_socket_newfd);
                    char bla[4096];
                    sprintf(bla, "# PINMAME_SOCKET version %s\n\n",
                            MAME_SOCKET_VERSION);
                    mame_socket_send(mame_socket_newfd, bla);
                    // mame_socket_send(mame_socket_newfd,"Test2\n");

                    // fprintf(mame_socket_newfd,"# Hello pinmae client!\n");
                    // fflush(mamefifo);
                    /*if (send(mame_socket_newfd, mame_socket_buf,
                       mame_socket_nbytes, 0) == -1) { perror("send");
                                                     fprintf(stdout,"SOCKET:
                       %s",mame_socket_buf);
                                                 } */
                }
            } else {
                // handle data from a client

                if ((mame_socket_nbytes = recv(
                         socketnr, mame_socket_buf, sizeof mame_socket_buf, 0)) <= 0) {
                    // got error or connection closed by client
                    if (mame_socket_nbytes == 0) {
                        // connection closed
                        printf("selectserver: socket %d hung up\n", socketnr);
                    } else {
                        perror("recv");
                    }
                    close(socketnr);                        // bye!
                    FD_CLR(socketnr, &mame_socket_master);  // remove from master set
                } else {
                    // we got some data from a client
                    fprintf(stdout, "SOCKET: %d = %s", socketnr,mame_socket_buf);
                    mame_socket_parse(socketnr,mame_socket_buf);
                    //coreGlobals?
                    /*                        for(j = 0; j <= mame_socket_fdmax;
                       j++) {
                                                // send to everyone!
                                                if (FD_ISSET(j,
                       &mame_socket_master)) {
                                                    // except the listener and
                       ourselves if (j != mame_socket_listener && j != i) { if
                       (send(j, mame_socket_buf, mame_socket_nbytes, 0) == -1) {
                                                            perror("send");
                                                            fprintf(stdout,"SOCKET:
                       %s",mame_socket_buf);
                                                        }
                                                    }
                                                }
                                            } */
                }
            }  // END handle data from client
        }      // END got new incoming connection
    }          // END looping through file descriptors

    // return 0;
}

void mame_socket_parse(int socketnr, char * msg) {
    fprintf(stdout, "PARSE: %d = #%s#", socketnr,msg);
   
    if ( strncmp(msg,"?",1)==0 ) {
         mame_socket_send(socketnr, "HELP\n");
    }
    if ( strncmp(msg,"l",1)==0) {
        char str[80];
        sprintf(str, "lamps1 = %02x\n", 12);
        mame_socket_send(socketnr, str );
    }
}



/*
int mame_socket_main(void)
{
    fd_set mame_socket_master;    // master file descriptor list
    fd_set mame_socket_read_fds;  // temp file descriptor list for select()
    int mame_socket_fdmax;        // maximum file descriptor number

    int mame_socket_listener;     // listening socket descriptor
    int mame_socket_newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage mame_socket_remoteaddr; // client address
    socklen_t mame_socket_addrlen;

    char mame_socket_buf[4096];    // buffer for client data
    int mame_socket_nbytes;

    char mame_socket_remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&mame_socket_master);    // clear the master and temp sets
    FD_ZERO(&mame_socket_read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}

*/