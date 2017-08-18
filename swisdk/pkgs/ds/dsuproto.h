/*************
 *
 * $Id: dsuproto.h,v 1.4 2010/06/09 23:39:51 epasheva Exp $
 *
 * Filename: dsuproto.h
 *
 * Purpose:  This file external prototypes for the ds package
 *
 * Copyright: � 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* dsshell.c */
extern struct icipccb *dsgetipchandle( swi_uint16 ipcchannel );
extern void dsinit( void );
extern void dslaunchshell ( void *shellparmp );
extern void dsshell( swi_uint8 *owneridp,
    void *dsrcvcbckp(
    swi_uint8 *inipcmsgp, swi_uint32 inmsglen,swi_uint8 *memfreep, 
    swi_uint16 ipcchannel ));
extern void dsep_send(
    enum useptype epid,
    swi_uint8 *packetp,
    swi_uint16 paylength);

/* dsipctask_sdk.c */
extern void dssend( swi_uint8 *packetptr, swi_uint32 packetlen, 
    swi_uint16  chanid);
extern void dsclose(swi_uint16 chanid);

/* ds_app.c */
extern SWI_RCODE dsopen( swi_uint8 *callernamep, swi_uint32 channel );
extern SWI_RCODE dsappsend( swi_uint32 channel, 
                    swi_uint8 *senddatap, 
                    swi_uint32 length);
extern SWI_RCODE dsreceive( swi_uint32 channel, 
                      swi_uint8 *rcvdatap, 
                      swi_uint32 *lengthp,
                      swi_uint32 timeout );
extern SWI_RCODE dsgetmaxnumchan( swi_uint16 *numchanp );
 
/*
 * $Log: dsuproto.h,v $
 */
