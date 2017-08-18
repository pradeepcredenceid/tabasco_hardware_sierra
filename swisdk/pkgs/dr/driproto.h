/*************
 *
 * $Id: driproto.h,v 1.6 2009/06/01 23:24:31 blee Exp $
 *
 * Filename:  driproto.h
 *
 * Purpose:   This file contains internal prototypes for the dr package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* drshell.c */

/* drtask_sdk.c */
extern enum drdevservicetype drchantodevsrv( swi_uint16 chanid);
extern struct drtaskcb *drgetcbp( void );
extern void drdiagmsg(swi_uint8 *inbufp, swi_uint32 resplength, 
            swi_uint8 *memfreep, swi_uint16 ipcchannel );

/*
 * $Log: driproto.h,v $
 */
