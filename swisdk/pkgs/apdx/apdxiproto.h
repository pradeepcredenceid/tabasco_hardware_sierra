/**
 * \if CvsId
 * $Id: apdxiproto.h,v 1.3 2009/03/25 17:55:11 bsiu Exp $
 * \endif
 * 
 * \file      apdxiproto.h
 *
 * \brief     This file contains internal prototypes for the demux application
 *            in the ap package
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 */

/* apdxtest.c */
extern swi_uint8 *SwiUtilDevToName( swi_uint16 deviceservice );
extern void apdxdatacallback( swi_uint16 deviceservice, swi_uint8 *datap, 
    swi_uint32 datalen, void *userp);
extern void apdxctlcallback( swi_uint16 deviceservice, swi_uint8 *ctldatap, 
    void *userp);

/*
 * $Log: apdxiproto.h,v $
 */
