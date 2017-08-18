/*
 * $Id: usiproto.h,v 1.5 2010/06/09 23:56:46 epasheva Exp $
 *
 * Filename: usiproto.h
 *
 * Purpose:   global prototypes for the US package
 *
 * Copyright Sierra Wireless Inc., 2008 All rights reserved
 *
 */

/* usscan_sdk.c */
extern void usclosetty( struct usbep *ep );

/* usdetect_sdk.c */
extern void usdetecttask( void *paramp);
extern struct usdcb *usgetdcbkp( void );
extern int usifacetoep( swi_uint32 iface, enum swi_endpointdirection dir );

/*
 * $Log: usiproto.h,v $
 */
