/*************
 *
 * $Id: hpuproto.h,v 1.17 2010/06/09 23:47:42 epasheva Exp $
 *
 * Filename: hpuproto.h
 *
 * Purpose:  This file external prototypes for the hp package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/
/* hprxtask_sdk.c */
extern void hprxtaskinit( void );

/* hptxtask_sdk.c */
extern void hptxtaskinit( void );

/* hploopback.c */
extern void hplooprequest(
    swi_uint8 *hiploopdatap,
    swi_uint32 hiploopsize,
    void (*hplbcbkfp)(  
         enum hpphstatus resultcode, 
         swi_bool runningapp,
         void *userp),
    void *userp );

/* hptx.c */
extern swi_uint8 *hpgetpayp( swi_uint8 *bufp, swi_uint32 hpmsgid );
extern swi_uint8 *hpgetbuf( void);
extern swi_uint8 *hpgetsizedbuf( swi_uint32 msglen );
extern void hpTxPacket(
    swi_uint32 msgdesignator,
    swi_uint8 msgparm,
    swi_uint8 *hippacketp,
    swi_uint16 paylength);

/* hprx.c */
extern void hpdispatch( swi_uint8 *memrelp, swi_uint8 *bufp, 
                        swi_uint16 buflength, swi_bool errorflg );
extern void hpregisterhip( swi_uint32 msgdesignator, 
                           void (*deliverbuffp)(  swi_uint32 msgdesignator, 
                                                  swi_uint8 msgparm, 
                                                  swi_uint8 *memfreep,
                                                  swi_uint8 *payloadp, 
                                                  swi_uint16 paylength, 
                                                  swi_bool errorflg));
/* hpairchange.c */
extern void hpairchangereq( swi_bool devicestate, swi_bool apprunning );
extern swi_uint8 * hpphgetprodstr( void );
extern swi_uint8 hpphgettechtype( void );
extern void hpgetdevinfo( 
    void (*hpdevcbfp)(  
         enum hpphstatus resultcode, 
         void *userp),
    void *userp,
    swi_uint8 version,
    swi_uint8 reqtype,
    swi_uint16 ipcchannel);
    
/* hpmdmstate.c */
extern void hpmdmregister( void(*cbfp)(enum hpmdmstate modemstate, 
                                       swi_bool apprunning,
                                       enum SWI_TYPE_Device devtype,
                                       void *userp ),
                           struct hpmdmreghndl *reghndlp,
                           char *namep,
                           void *userp );
extern swi_bool hpgetairserverstatus(void);
extern void hpsendairserverrr( 
    swi_uint8 *inipcmsgp,
    swi_uint8 *memfreep,
    swi_uint8 ipcchannel );
extern void hpsendportnamerr( swi_uint8 *inipcmsgp, swi_uint8 *memfreep, 
swi_uint8 ipcchannel);
                                                  
/* hpphtask_sdk.c */
extern void hpphtaskinit( void );

/* hpfwdownload.c */
extern void hpsendfwdwnrr( 
    swi_uint8 *inipcmsgp, 
    swi_uint8 *memfreep, 
    swi_uint8 ipcchannel);

/* hp.c */
extern void hpinit( void );

/* hpreset.c */
extern void hpresetmdm( swi_uint8 *inipcmsgp, 
                        swi_uint8 *memfreep, 
                        swi_uint8 ipcchannel );


/*
 * $Log: hpuproto.h,v $
 */
