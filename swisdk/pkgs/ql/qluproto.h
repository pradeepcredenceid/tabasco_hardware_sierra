/*
 * $Id: qluproto.h,v 1.4 2008/07/26 00:13:01 epasheva Exp $
 *
 * Fileiname: qluproto.h
 *
 * Purpose:   global prototypes for the QL package
 *
 * Copyright Sierra Wireless Inc., 2008 All rights reserved
 *
 */
extern void qlinitlnk( struct qllink *linkp, void *userp );
extern void qladdnext(struct qllink *curlinkp, struct qllink *newlinkp);
extern void qladdprev(struct qllink *curlinkp, struct qllink *newlinkp);
extern void *qlgetnext( struct qllink *linkp);
extern void *qlgetprev( struct qllink *linkp);
extern void *qlremnext( struct qllink *curlinkp);
extern void *qlremprev( struct qllink *curlinkp);
extern void *qlremcur( struct qllink *curlinkp);

/* 
 * $Log: qluproto.h,v $
 */
