/*************
 *
 * $Id: amuproto.h,v 1.21 2010/06/09 23:29:56 epasheva Exp $
 *
 * Filename: amuproto.h
 *
 * Purpose:  This file external prototypes for the am package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* am.c */
extern swi_uint8 *amgethdrptr( 
    swi_uint8 *parmp, 
    swi_uint16 amtype, 
    swi_uint8 *bufstartp );

extern void amparseamheader(
    swi_uint16 *amtypep,
    swi_uint16 *amheaderversionp,
    swi_uint8  *amresultcodep,
    swi_uint16 *ampacketlengthp,
    swi_uint8 **packetpp);

extern swi_uint8 *amgettagname( void );

/* amcns.c */
extern swi_uint16 ambuildcnsresponse( 
    swi_uint8 **amsgpp, 
    struct amcnsrrparms *amcnsrrparmp);
extern swi_uint16 ambuildcnsnotify( 
    swi_uint8 **amsgpp, 
    struct amcnsrrparms *amcnsrrparmp);
extern void amparsecnsrequest( 
    swi_uint8 **amcnspacketpp, 
    struct amcnsrrparms *amcnsrrparmp );

/* amapi.c */
extern swi_uint8 *amgetparamp( 
    swi_uint16 amcnstype, 
    swi_uint8 *bufstartp );
extern swi_uint8 *amgetreqbufp( void );
extern SWI_RCODE amsendnwait( 
    swi_uint8 *amhdrp, 
    struct amrrparms *amparamsp, 
    swi_uint8 **responsepp );
extern swi_bool amapiwaitnotif(
    swi_uint8 *apipcmsgp, 
    swi_uint32 *apipcbuflngp, 
    struct amrrparms *parmsp,
    swi_uint8  **payloadp,
    swi_uint32 timeout );
extern swi_bool aminitapi( void );

/* amipctask_sdk.c */
extern void aminit( void );
extern void amipctaskinit( void );
extern void amsdk_send( swi_uint8 *ipcbufp, 
                            swi_uint32 ipcbuflen,
                            swi_uint8 ipcchannel );
extern void amsdk_sendnot( swi_uint8 *ipcbufp, 
                            swi_uint32 ipcbuflen,
                            swi_uint8 ipcchannel );
extern void amsdk_sendcnsnot( swi_uint8 *ipcbufp, 
                            swi_uint32 ipcbuflen,
                            swi_uint16 cnsobjid );
extern void amipcdisableallnotif ( void );

/* amadmin.c */
extern swi_uint16 ambuildfwdwnresponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    swi_uint8 *errorstringp,
    swi_uint8 errorstringlen);
extern swi_uint16 ambuildfwdwnnotify( 
    swi_uint8 **amsgpp, 
    enum swi_amfwdwnnotiftype fwdwnstate);
extern swi_uint16 ambuildairchangenotify( 
    swi_uint8 **amsgpp, 
    swi_uint32 devicetype, 
    swi_bool devicepresent,
    swi_bool apprunning,
    swi_uint32 airchgera,
    swi_uint8 *prodstrp,
    swi_uint32 techtype);
extern void amparsefwdwnrequest( 
    swi_uint8 **ampacketpp, 
    struct amfwdwnrrparms *amrrparmp );
extern swi_uint16 ambuildairserverresponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    struct amairserverparms *amairsrvrrparmp);
extern void amparseairserverrequest( 
    swi_uint8 **ampacketpp, 
    struct amairserverparms *amairparmsp);
extern swi_uint16 ambuildmdmresetresp( swi_uint8 **amsgpp, 
                                       struct ammdmresetparms *resetp );
extern swi_uint16 ambuildmdmresetrequest( swi_uint8 **amsgpp, 
                                          struct ammdmresetparms *resetp );
extern void amparsemdmresetresp( swi_uint8 **ampacketpp, 
                                 struct ammdmresetparms *resetp );
extern void amparsemdmresetrequest( swi_uint8 **ampacketpp, 
                                struct ammdmresetparms *resetp );
extern swi_uint16 ambuilddevinforequest( 
    swi_uint8 **amsgpp, 
    struct amdevinfoparms *amdevinforrparmp);
extern swi_uint16 ambuilddevinforesponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    struct amdevinfoparms *amdevinforrparmp);
extern void amparsedevinforesp( swi_uint8 **ampacketpp, 
                                struct amdevinfoparms *devinfop );
extern swi_uint16 ambuildmdmresetrequest( swi_uint8 **amsgpp, 
                                          struct ammdmresetparms *resetp );
extern void amparsedevinforequest( swi_uint8 **ampacketpp, 
                                   struct amdevinfoparms *devinfop );
extern swi_uint16 ambuildusbportnameresponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    struct amusbportnameparms *amusbportnamerparmp);


/* amdemux.c */
extern swi_uint16 ambuilddemuxrequest( 
    swi_uint8 **amsgpp, 
    swi_uint16 deviceservice, 
    swi_uint16 opcode);
extern swi_uint16 ambuilddemuxctlind( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    swi_uint8 *errorstringp,
    swi_uint16 deviceservice,
    swi_uint8 opcode,
    swi_uint8 devicepresent,
    swi_uint32 eranumber);
extern swi_uint32 ambuilddemuxdataind( 
    swi_uint8 **amsgpp,
    swi_uint16 deviceservice);
extern void amparsedemuxrequest( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *devservp,
    swi_uint16 *operationp);
extern void amparsedemuxctlind( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *devservp,
    swi_uint8 *opcodep,
    swi_uint8 *devicepresentp,
    swi_uint32 *eranumber,
    swi_uint8 *errorstringp);
extern void amparsedemuxdataind( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *devservp);
  
/* amui_api.c */
extern swi_uint16 ambuilduirequest( 
    swi_uint8 **amsgpp, 
    swi_uint16 datalen);
extern void amparseuiresponse( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *uidatalenp);

/* amui_sdk.c */
extern void amparseuirequest( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *uidatalenp);
extern swi_uint16 ambuilduiresponse( 
    swi_uint8 **amsgpp, 
    swi_uint16 uidatalen,
    enum swi_amresults amresultcode);
    
/*
 * $Log: amuproto.h,v $
 */
