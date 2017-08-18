/*************
 *
 * $Id: druproto.h,v 1.2 2009/06/01 23:24:31 blee Exp $
 *
 * Filename: druproto.h
 *
 * Purpose:  This file external prototypes for the dr package
 *
 * Copyright: � 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/
/* drtask_sdk.c */
extern void drinit( void );
extern void drtaskinit( void );
extern void dripcrcvhandler( 
    swi_uint8 *inipcmsgp, 
    swi_uint32 inipcmsglen,
    swi_uint8 *memfreep,
    swi_uint16 ipcchannel);
/*
 * $Log: druproto.h,v $
 */
