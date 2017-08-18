/*************
 *
 * $Id: uiiproto.h,v 1.2 2010/06/09 23:54:15 epasheva Exp $
 *
 * Filename:  uiiproto.h
 *
 * Purpose:   This file contains internal prototypes for the ds package
 *
 * Copyright:  2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* uitask_sdk.c */
extern struct uitaskcb *uigetcbp( void );

/* uidl.c */
extern swi_bool uidlctrlon(char *cmdp);
extern swi_bool uidlctrloff(char *cmdp);
extern swi_bool uidlctrllist(char *cmdp);
extern void uiprocuidlreq(swi_uint8 *cmdp);

/* uicmd.c */
extern  char *uistrtok( 
    char *strTokenp, 
    const char *strDelimitp, 
    struct slstrtokblk *strtokblkp);
extern  void uiProcCmd(struct uiCmdTable const *cTablep, char *cmdp);

/* uitables.c */
extern  struct uiCmdTable const *uiGetMainTblp(void);

/* uiutil.c */
extern  void uiOutput(char *msgp);

/*
 * $Log: uiiproto.h,v $
 */
