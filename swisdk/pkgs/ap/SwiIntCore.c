/**
 *
 * $Id: SwiIntCore.c,v 1.6 2010/06/10 00:22:02 epasheva Exp $
 *
 * Filename: SwiApiCore.c
 *
 * Purpose:  Contains core information and utilities required
 *           by the AP package members
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "ap/apidefs.h"    /* API Package definitions */
#include "sl/sludefs.h"

/* This package's global control block */
struct apicb apicbk;   

/**
 *
 * Return a pointer to the AP package's control block 
 *
 * \return  pointer to apicb structure
 *
 */
package struct apicb *SwiApiGetCbp(void)
{
    return &apicbk;
}

/**
 *
 * Determine whether the API has been previously started
 *
 * \return  TRUE  - API is initialized and running
 * \return  FALSE - API was not properly started. Go back and
 *                  start it up
 *
 */
package swi_bool SwiApiRunning( void )
{
    struct apicb *apicbp = SwiApiGetCbp();

    if( apicbp->apiopened == APINITIALIZED )
        return TRUE;

    return FALSE;
}

/**
 *
 * Save the string pointed to by the calling arg. into 
 * the Last Error storage area in case the customer 
 * wants to see it
 *
 * \param erstrp   - Pointer to the first character of the 
 *                   string
 * \param errsize  - Length of the error string (bytes)
 *
 */
global void SwiApSaveError( 
    swi_charp erstrp, 
    swi_uint32 errsize )
{
    struct apicb *apicbp = SwiApiGetCbp();

    /* Make sure we won't overflow the array. 
     * If so, truncate the string accounting
     * for the trailing NULL
     */
    if(errsize > sizeof(apicbp->aplasterror) )
    {
        /* Truncate the string to fit */
        errsize = sizeof( apicbp->aplasterror) - 1;
    }

    /* Lay down a clean bed of NULLs before copying into the buffer */
    slmemset( (char *)&apicbp->aplasterror[0], 0, sizeof( apicbp->aplasterror ));
    slmemcpy( (char *)&apicbp->aplasterror[0], erstrp, errsize );
}


/**
 *
 * Validate the specified timeout value is within the allowable
 * range
 *
 * \param timeout - A candidate timeout value to be validated
 *
 * \return  TRUE  - Timeout is within the allowable range
 * \return  FALSE - Timeout is outside the allowed range
 *
 */
package swi_bool Swi_validatetimeout( swi_uint32 timeout )
{
    /* For now just return the all-clear */
    return TRUE;
}

/*
 * $Log: SwiIntCore.c,v $
 */
