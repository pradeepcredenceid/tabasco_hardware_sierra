/*************
 *
 * $Id: cnuproto.h,v 1.5 2010/06/10 00:01:40 epasheva Exp $
 *
 * Filename: cnuproto.h
 *
 * Purpose:  This file external prototypes for the cn package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* cnpkg_api.c */
extern swi_uint8 *cngetreqbkp( void );
extern swi_uint32 cngetreqbksz( void );
extern swi_uint8 *cngetrespbkp( void );
extern swi_uint32 cngetrespbksz( void );
extern swi_uint32 cngetappparm( void );
extern SWI_RCODE cnsendnwait( swi_uint8 *amcnshdrp, 
                              struct amcnsrrparms *amparamsp, 
                              swi_uint8 **responsep );
extern swi_bool cninitapi( void );
extern swi_bool cnapiwaitnotif( swi_uint8 *apipcmsgp, 
                                swi_uint32 *apipcbuflngp, 
                                swi_uint32 timeout );

/* cnrrtask_sdk.c */
extern void cnsendrr( swi_uint8 *inipcmsgp, swi_uint8 *memfreep );
extern void cninit( void );
extern void cnrrtaskinit( void );

/*
 * $Log: cnuproto.h,v $
 */
