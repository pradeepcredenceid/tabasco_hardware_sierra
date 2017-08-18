/*************
 *
 * $Id: dd.c,v 1.7 2010/08/24 20:31:58 mdejong Exp $
 *
 * Filename:  dd
 *
 * Purpose:   System information package. Provides entry points which
 *            supply information about the system. Runs under the 
 *            caller's context, no task
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "aa/aaglobal.h"
#include "us/usudefs.h"
#include "dd/ddudefs.h"

/*************
 *
 * Name: ddairinttype
 *
 * Purpose: Indicate the technology of the air interface device (modem)
 *             
 * Parms:   none
 *
 * Return:  An indication of the wireless technology used by the 
 *          modem, if one is connected
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
global enum ddwitech ddairinttype( void )
{
    swi_uint32 vendid, prodid;
    enum ddwitech retval;

    /* Get the PID of the connected modem from the US package */
    usgetinfo( &vendid, &prodid );

    /* Convert this into a technology type */
    switch( prodid )
    {
        /*--- CDMA Modems ---*/
        case SWI_MC5725GM:    
        case SWI_MC5725LM:   
        case SWI_MC5725HM:  
        case SWI_AC595GM:  
        case SWI_AC595UGM:
        case SWI_C597UM:
        case SWI_MC5727GM:
        case SWI_MC5727LM:
        case SWI_AC597EM:
        case SWI_T598UM:
        case SWI_AC802GM:
        case SWI_T11UM:
        case SWI_AC402GM:
        case SWI_MC5728GM:
        case SWI_SL501XGM:
            retval = DDCDMA;
            break;

        /*--- GSM Modems ---*/
        case SWI_MC8775GM:
        case SWI_MC8775LM:
        case SWI_MC8775CM:
        case SWI_MC8775EM:
        case SWI_AC875G:
        case SWI_AC875UG:
        case SWI_AC875EG:
        case SWI_AC880U:
        case SWI_AC880G:
        case SWI_AC880EG:
        case SWI_AC881G:
        case SWI_AC881UG:
        case SWI_AC881EG:
        case SWI_MC8780:
        case SWI_MC8781:
        case SWI_AC885G:
        case SWI_AC885EG:
        case SWI_MC8785GC:
        case SWI_AC888G:
        case SWI_USBDIP:
            retval = DDGSM;
            break;
    
        default:
            retval = DDUNDEF;
            break;
    }

    return retval;
}


/*
 * $Log: dd.c,v $
 */
