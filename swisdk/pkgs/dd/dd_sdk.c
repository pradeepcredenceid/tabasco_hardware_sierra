/*************
 *
 * $Id: dd_sdk.c,v 1.9 2010/08/24 20:31:58 mdejong Exp $
 *
 * Filename:  dd_sdk.c
 *
 * Purpose:   Contains Device Information functions that run only
 *            on the SDK side
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "dd/ddidefs.h"

/*************
 *
 * Name: ddgetdevtype
 *
 * Purpose: Return the caller the device type of the currently connected
 *          modem, if any. The function works by fetching the modem's 
 *          product ID and converting from that to an enumerated type
 *          defined within the AP package - SwiApiCmBasic.h
 *             
 * Parms:   none
 *
 * Return:  The device type of the attached modem which will be one of the
 *          enumerations in SWI_TYPE_Device
 *
 * Abort:   none
 *
 * Notes:   For the time being if the incoming PID is USBDIP, then the 
 *          returned value is for the USB306 modem. However, once other
 *          modems implement this same PID, this will result in ambiguous
 *          incoming information. 
 *
 **************/
global enum SWI_TYPE_Device ddgetdevtype( void )
{
    swi_uint32 vendid, prodid;
    enum SWI_TYPE_Device retval;
    
    /* Get the PID of the connected modem from the US package */
    usgetinfo( &vendid, &prodid );

    /* Convert this into a technology type */
    switch( prodid )
    {
        /*--- CDMA Modems ---*/
        case SWI_MC5725GM:    
        case SWI_MC5725LM:   
        case SWI_MC5725HM:  
            retval = SWI_DEVICE_MC5725;
            break;
        case SWI_AC595GM:  
            retval = SWI_DEVICE_AC595;
            break;
        case SWI_AC595UGM:
            retval = SWI_DEVICE_AC595U;
            break;
        case SWI_C597UM:
            retval = SWI_DEVICE_AC597;
            break;
        case SWI_MC5727GM:
        case SWI_MC5727LM:
            retval = SWI_DEVICE_MC5727;
            break;
        case SWI_AC597EM:
            retval = SWI_DEVICE_AC597E;
            break;
        case SWI_T598UM:
            retval = SWI_DEVICE_T598;
            break;
        case SWI_AC802GM:
            retval = SWI_DEVICE_AC802;
            break;
        case SWI_T11UM:
            retval = SWI_DEVICE_T11;
            break;
        case SWI_AC402GM:
            retval = SWI_DEVICE_AC598E;
            break;
        case SWI_MC5728GM:
            retval = SWI_DEVICE_MC5728;
            break;
        case SWI_SL501XGM:
            retval = SWI_DEVICE_SL501X;
            break;
        /*--- GSM Modems ---*/
        case SWI_MC8775GM:
        case SWI_MC8775LM:
        case SWI_MC8775CM:
        case SWI_MC8775EM:
            retval = SWI_DEVICE_MC8775;
            break;
        case SWI_MC8780:
            retval = SWI_DEVICE_MC8780;
            break;
        case SWI_MC8781:
            retval = SWI_DEVICE_MC8781;
            break;
        case SWI_MC8790GC:
            retval = SWI_DEVICE_MC8790;
            break;
        case SWI_AC875G:
        case SWI_AC875UG:
        case SWI_AC875EG:
            retval = SWI_DEVICE_AC875;
            break;
        case SWI_AC880U:
        case SWI_AC880G:
        case SWI_AC880EG:
            retval = SWI_DEVICE_AC880;
            break;
        case SWI_AC881G:
        case SWI_AC881UG:
        case SWI_AC881EG:
            retval = SWI_DEVICE_AC881;
            break;
        case SWI_AC885G:
        case SWI_AC885EG:
            retval = SWI_DEVICE_AC885E;
            break;
        case SWI_MC8785GC:
            retval = SWI_DEVICE_MC8785;
            break;
        case SWI_AC888G:
            retval = SWI_DEVICE_C888;
            break;
        case SWI_MC8777:
            retval = SWI_DEVICE_MC8777;
            break;
        case SWI_USBDIP:
            retval = SWI_DEVICE_UNIFIEDPID;
            break;

        default:
            retval = SWI_DEVICE_None;
            break;
    }

    return retval;
}

/*
 * $Log: dd_sdk.c,v $
 */
