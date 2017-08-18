/*
 *  $Id: SwiSerial.c,v 1.1 2009/03/30 21:59:51 blee Exp $
 *
 *  Filename: SwiSerial.c
 *
 *  Purpose:  This file contains routine(s) that deal with opening, configuring
 *            reading and writing to a SWI serial device.
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "SwiSerial.h"

#define BAUDRATE B115200    /* preferred baud rate */

/*
 *
 * Name:     open_dev
 *
 * Purpose:  Opens a device for serial I/O
 *
 * Parms:    devp    - pointer to device name string
 *           oldtiop - pointer to old terminal i/o parameters
 *           newtiop - pointer to new terminal i/o parameters
 *
 * Return:   fd - file descriptor of device
 *           -1 - open failed
 *
 * Notes:    none
 *
 */
int open_dev (char *devp,
              struct termios *oldtiop,
              struct termios *newtiop)
{
    int fd = -1;

    fd = open(devp, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(devp);
        return fd;
    }

   tcgetattr(fd, oldtiop); /* save current port settings */

   bzero(newtiop, sizeof(struct termios));
   newtiop->c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
   newtiop->c_iflag = IGNPAR;
   newtiop->c_oflag = 0;

   /* set input mode (non-canonical, no echo,...) */
   newtiop->c_lflag =  0;

   newtiop->c_cc[VTIME]    = 1;   /* inter-character timer */
   newtiop->c_cc[VMIN]     = 1;   /* blocking read until n chars received */

   tcflush(fd, TCIFLUSH);
   tcsetattr(fd,TCSANOW, newtiop);

   return fd;
}

/*
 *
 * Name:     close_dev
 *
 * Purpose:  Closed a device
 *
 * Parms:    fd      - file descriptor of device
 *           oldtiop - pointer to old terminal i/o parameters
 *
 * Return:   0 successful
 *           EOF ? Otherwise
 *
 * Notes:    none
 *
 */
int close_dev ( int devfd, struct termios *oldtiop )
{
    int rv;

    /* Restore previous settings */
    tcflush(devfd, TCIFLUSH);
    tcsetattr(devfd,TCSANOW, oldtiop);

    rv= close(devfd);

    return rv;
}

/*
 * $Log: SwiSerial.c,v $
 * Revision 1.1  2009/03/30 21:59:51  blee
 * BUG # 15436
 * Initial check-in of Relay Agent for Diagnostics, NMEA and AT services
 *
 *
 */
