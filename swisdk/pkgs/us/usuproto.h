/*
 * $Id: usuproto.h,v 1.9 2010/06/09 23:57:31 epasheva Exp $
 *
 * Fileiname: usuproto.h
 *
 * Purpose:   Global prototypes for the US package
 *
 * Copyright Sierra Wireless Inc., 2008 All rights reserved
 *
 */

/* usscan_sdk.c */
extern swi_bool usgetinfo( swi_uint32 *vendidp, swi_uint32 *prodidp);
extern void usreadterms( swi_uint32 endpoint, 
                         swi_uint8 readtermination,
                         swi_uint32 readthreshold );
extern swi_bool usalive( void );
extern void ususbwaitmodem( void );
extern swi_bool usrcvtty( swi_uint16 endpoint, 
                          swi_uint8 *datap, 
                          swi_size_t *dlengthp, 
                          swi_uint32 timeout );
extern swi_bool ussendtty( swi_uint16 endpoint, 
                           swi_uint8 *datap, 
                           swi_size_t dlength );
extern void usscantask( void *paramp);
extern void usstaskinit( void );
extern void usdtaskinit( void );
extern swi_bool usgetep( enum useptype eptype, struct usbep *ep);
extern swi_bool usgetepname(swi_uint32 endpoint, 
                            swi_uint8* namep, 
                            swi_uint8 namelen,
                            swi_bool  inout);
extern void usinit( void );

/*
 * $Log: usuproto.h,v $
 */
