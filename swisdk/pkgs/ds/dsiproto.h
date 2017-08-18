/*************
 *
 * $Id: dsiproto.h,v 1.1 2008/09/30 23:31:07 epasheva Exp $
 *
 * Filename:  dsiproto.h
 *
 * Purpose:   This file contains internal prototypes for the ds package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* dsshell.c */
extern struct dsipccb *dsgetipccbp( swi_uint16 chan );
extern swi_uint32      dschantoidx( swi_uint16 chanid );

/*
 * $Log: dsiproto.h,v $
 */
