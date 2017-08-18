/*
 * $Id: uiudefs.h,v 1.1 2009/01/27 21:38:25 blee Exp $
 *
 * Filename:  uiudefs.h
 *
 * Purpose:   User definitions for ui package
 *
 * Copyright: C 2009 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef uiudefs_h
#define uiudefs_h


/*************
 *
 * Name:     uisvctype
 *
 * Purpose:  ui service types
 *             
 * Members:  UISVCDLLOG      - Request from the Application
 *           UISVCATCMD      - AT command 
 *           UISVCINVALID    - Invalid service type indication
 * 
 * Notes:    None
 *
 **************/
enum uisvctype
{
    UISVCDLLOG,
    UISVCATCMD,
    UISVCINVALID  /* should be always last */  
};

#include "ui/uiuproto.h"

#endif

/*
 * $Log: uiudefs.h,v $
 */
