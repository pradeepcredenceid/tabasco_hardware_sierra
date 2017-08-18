/*
 * $Id: dluproto.h,v 1.7 2010/06/09 23:36:29 epasheva Exp $
 *
 * Filename: dluproto.h
 *
 * Purpose: Debug Logger package prototypes
 *
 * NOTES:
 *
 * Copyright Sierra Wireless Inc., 2003. All rights reserved.
 */

extern global void dlregister(
    char *pkgnamep, 
    struct dlusercb * usercb,
    swi_bool masterenable);

extern global void dlLog( 
    struct dlusercb * usercb,
    swi_uint16 class,
    char * stringp,
    swi_uint32 dlparam1,
    swi_uint32 dlparam2,
    swi_uint32 dlparam3,
    swi_uint32 dlparam4
    );

extern global void dlinit( void );

extern void dlmasterenable(struct dlusercb *dlcbkp, swi_bool state);
extern global swi_uint16 dlpackidget(char * userpackidp);
extern global swi_bool dlusercbconfig(struct dlusercb * dlusercbp, 
    swi_bool dlkeepmask);
extern global void dllogenable(swi_bool state);
extern global void dllogentoggle(void);
extern char * dlpackidstrget(swi_uint16 pkgid);

/*****************************************************************
 * $Log: dluproto.h,v $
 *****************************************************************/
