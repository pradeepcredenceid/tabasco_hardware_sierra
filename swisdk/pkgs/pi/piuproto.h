/*
 *    $Id: piuproto.h,v 1.4 2008/10/15 21:12:44 YGao Exp $
 *
 *    Filename:    piuproto.h
 *
 *    Purpose:     external function prototypes
 *
 *    Copyright: C 2000 Sierra Wireless
 *            All rights reserved
 *
 */

extern  void piput16(swi_uint16 field, swi_uint8 **packetpp);
extern  void piput16s(const swi_uint16 *field, swi_uint16 num, swi_uint8 **packetpp);
extern  void piput32(swi_uint32 field, swi_uint8 **packetpp);
extern  void piput32s(const swi_uint32 *field, swi_uint16 num, swi_uint8 **packetpp);
extern  void piput64(swi_uint64 field, swi_uint8 **packetpp);
extern  void piputmulti(const swi_uint8 *fieldsp, swi_uint8 **packetpp, swi_uint16 numfields);
extern  swi_uint16 piget16(swi_uint8 **packetpp);
extern  void piget16s(swi_uint16 *field, swi_uint16 num, swi_uint8 **packetpp);
extern  swi_uint32 piget32(swi_uint8 **packetpp);
extern  void piget32s(swi_uint32 *field, swi_uint16 num, swi_uint8 **packetpp);
extern  swi_uint64 piget64(swi_uint8 **packetpp);
extern  void pigetmulti(swi_uint8 **packetpp, swi_uint8 *bufferp, swi_uint16 numfields);
extern  swi_uint16 piget16_noinc(swi_uint8 **packetpp);
extern  swi_uint32 piget32_noinc(swi_uint8 **packetpp);
extern  void piput16_host(swi_uint16 field, swi_uint8 **packetpp);
extern  void piput16_noinc(swi_uint16 field, swi_uint8 **packetpp, swi_uint32 inc);
extern  swi_uint16 piget16_host(swi_uint8 **packetpp);

/*
 * $Log: piuproto.h,v $
 */
