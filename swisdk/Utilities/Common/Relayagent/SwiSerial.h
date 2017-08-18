/*
 *  $Id: SwiSerial.h,v 1.1 2009/03/30 21:59:51 blee Exp $
 *
 *  Filename: SwiSerial.h
 *
 *  Purpose:  This file contains defines, data structures and forward
 *            declarations of the Serial Interface
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */
#ifndef _SWISERIAL_H
#define _SWISERIAL_H

#include "SwiDataTypes.h"

#define _POSIX_SOURCE 1     /* POSIX compliant source */

/* Forward Declarations */
extern int open_dev (char *devp,
                     struct termios *oldtiop,
                     struct termios *newtiop);
extern int close_dev ( int devfd, struct termios *oldtiop);


#endif

/*
 * $Log: SwiSerial.h,v $
 * Revision 1.1  2009/03/30 21:59:51  blee
 * BUG # 15436
 * Initial check-in of Relay Agent for Diagnostics, NMEA and AT services
 *
 *
 */
