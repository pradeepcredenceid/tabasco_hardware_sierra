/*************
 *
 * Filename:  amiproto.h
 *
 * Purpose:   This file contains internal prototypes for the am package
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* am.c */
extern void ambuildamheader(
    swi_uint16 amtype,
    swi_uint16 amheaderversion,
    enum eQCWWANError amresultcode,
    swi_uint16 ampacketlength,
    swi_uint8 **packetpp);
/* amsdk.c */

extern void amsdkhandler(
    swi_uint8 *inipcmsgp,
    swi_uint8 *memfreep,
    swi_uint8 ipcchannel );

/* amipctask_sdk.c */
void amQueueLock();
void amQueueUnlock();
struct amipccb *amgetipccbp( void );
void amShortResp(
    swi_uint16 amtypercv,
    enum eQCWWANError resultcode,
    swi_uint8 ipcchannel );
void amreleventbk( struct amevtblock *eventbkp );

/* amtimeout_sdk.c */
void amtimeouttask( void *paramp );
void amreqinittmr( struct amevtblock *eventbkp);
void amupdatetimeout(
        swi_uint8   *amqmipacketp,
        swi_uint32  timeout );
