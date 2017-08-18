/*************
 *
 * $Id: hpiproto.h,v 1.16 2010/06/09 23:44:34 epasheva Exp $
 *
 * Filename:  hpiproto.h
 *
 * Purpose:   This file contains internal prototypes for the hp package
 *
 * Copyright: � 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* hprxtask_sdk.c */
extern struct hprxcb *hpgetrxcbp( void );

/* hprx.c */
extern void hprxinit(void);

/* hptx.c */
extern struct hptxcb *hpgettxcbp( void );
extern void hptxinit( void );
extern swi_int32 hpconstruct(
    swi_uint8 *packetp,
    swi_uint32 msgdesignator,
    swi_uint8 msgparm,
    swi_uint16 paylength,
    swi_uint8 *payloadp);

/* hputil.c */
extern void hpdesignator2msgid(
    swi_uint32 msgdesignator, 
    swi_uint32 *shortmsgidp, 
    swi_bool *extheaderp, 
    swi_uint32 *extmsgidp);
extern void hpmsgid2designator(
    swi_uint32 *msgdesignatorp, 
    swi_uint8 shortmsgid, 
    swi_uint16 extmsgid);

/* hploopback.c */
extern void hpsetlbtimer( swi_uint32 timeout );
extern void hpphlbrx(   swi_uint8 msgid, 
                         swi_uint8 msgparm,
                         swi_uint8 *freemem,
                         swi_uint8 *payloadp, 
                         swi_uint16 paylength, 
                         swi_bool errorflg );
extern void hplooptimer( void );
extern void hpphprocloopresp(struct hphipevt *hipevtp);
extern void hpphprocloopreq(struct hplbreqevt *lbevtp);
extern void hplbinit( void );
    
/* hpairchange.c */
extern void hpsetdirtimer( swi_uint32 timeout );
extern void hpprocairchg( struct hpphairchgevt *airchgevtp);
extern void hpdirtimer( void );
extern void hpdirinit( void );
extern void hpprocdirreq(struct hpphdevinfoevt *devinfoevtp);
extern void hpprocdirresp(struct hphipevt *hipevtp);

/* hpmdmstate.c */
extern void hpprocmdmreg( struct hpmdmregevt *mdmregreq );
extern void hpmdmkick( enum hpmdmstate mdmstate, 
                       swi_bool apprunning,
                       enum SWI_TYPE_Device devicetype );
extern void hpmdminit( void );
extern swi_bool hpapprunning(void);
extern void hpamportreq(swi_uint8 *packetp, swi_uint8 ipcchannel);
extern void hpamairreq(swi_uint8 *packetp, swi_uint8 ipcchannel);


/* hpphtask_sdk.c */
extern struct hpphcb *hpgetphcbp( void );
extern void           hpphprocessloopresp(struct hpphevtblock *eventbkp);
extern void           hpphprocessloopreq(struct hpphevtblock *eventbkp);
extern void           hpphprocesslinkstaind(struct hpphevtblock *eventbkp);
extern void           hpphinit( void );

/* hpfwdownload.c */
extern  void hpamfwdwnreq(swi_uint8 *packetp);
extern void hpfwdwncbmsg(struct hpphevtblock  *eventbkp);
extern void hpfwdwntimer(void);
extern void hpfwdownloadinit(void);
extern void hpmodestatechangemsg(struct hpphevtblock  *eventbkp);

/*
 * $Log: hpiproto.h,v $
 */
