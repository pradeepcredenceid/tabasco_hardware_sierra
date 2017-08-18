/*************
 *
 * $Id: amiproto.h,v 1.6 2010/06/09 23:28:19 epasheva Exp $
 *
 * Filename:  amiproto.h
 *
 * Purpose:   This file contains internal prototypes for the am package
 *
 * Copyright:  2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* am.c */
extern void ambuildamheader(
    swi_uint16 amtype,
    swi_uint16 amheaderversion,
    swi_uint8  amresultcode,
    swi_uint16 ampacketlength,
    swi_uint8 **packetpp);

/* amadmin.c */
extern swi_uint16 ambuildfwdwnrequest( 
   swi_uint8 **amsgpp, 
   struct amfwdwnrrparms *amfwdwnrrparmp);
extern void amparsefwdwnresponse( 
    swi_uint8 **ampacketpp, 
    struct amfwdwnrrparms *amfwdwnrrparmp );
extern void amparsefwdwnnotify( 
    swi_uint8 **ampacketpp, 
    struct amfwdwnrrparms *amparmp );
extern swi_uint16 ambuildairserverrequest( 
    swi_uint8 **amsgpp, 
    struct amairserverparms *amairsrvrrparmp);
extern void amparseairserverresponse( 
    swi_uint8 **ampacketpp, 
    struct amairserverparms *amairparmsp);
extern void amparseairchgnotify( 
    swi_uint8 **amaircpacketpp, 
    struct amairserverparms *amairparmsp );
extern swi_uint16 ambuildusbportnamerequest( 
    swi_uint8 **amsgpp, 
    struct amusbportnameparms *amusbportnamerparmp);
extern void amparseportnameresponse( 
    swi_uint8 **ampacketpp, 
    struct amusbportnameparms *amportnameparmsp);
    
/* amcns.c */
extern void amsetcnsrequestoperationid( 
    swi_uint8 *amcnspacketp, 
    swi_uint8 operationid);
extern swi_uint16 ambuildcnsrequest( 
    swi_uint8 **amsgpp, 
    struct amcnsrrparms *amcnsrrparmp);
extern enum swi_amresults amparsecnsresponse( 
    swi_uint8 **amcnspacketpp, 
    struct amcnsrrparms *amcnsrrparmp );
extern void amparsecnsnotify( 
    swi_uint8 **amcnspacketpp, 
    struct amcnsrrparms *amcnsrrparmp );
/*
 * $Log: amiproto.h,v $
 */
