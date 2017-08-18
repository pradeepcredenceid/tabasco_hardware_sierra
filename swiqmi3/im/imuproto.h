/*************
 *
 * Filename: imuproto.h
 *
 * Purpose:  External prototypes for im package
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

#ifndef IMUPROTO_H
#define IMUPROTO_H

#include "imudefs.h"

/*-------------
  Prototypes
 --------------*/
/* imtask_sdk.c */
extern swi_uint8* imgetreqbkp(void);
extern void iminit(void);
extern void fhiminit(void);
extern void imtaskinit( const char *fwimagepath,
                        swi_uint32 isMDM9x15,
                        swi_uint16 imgMask,
                        swi_uint32 forceDownload );

extern void fhtaskinit( const char *pFwpath,
                        swi_uint32 isMDM9x15,
                        swi_uint16 imgMask);

extern void imdssend(   swi_uint8 *txbufp,
                        swi_uint32 pktsize );

extern enum imerrcodes_e imuser_image_info_get(
    struct im_image_info_s *pin);
#endif /* IMUPROTO_H */
