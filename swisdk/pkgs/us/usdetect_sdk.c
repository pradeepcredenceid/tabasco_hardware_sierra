/*
 *    $Id: usdetect_sdk.c,v 1.23 2010/08/24 20:31:58 mdejong Exp $
 *
 *    Filename: usdetect_sdk.c
 *
 *    Purpose:  USB Device Detection Task
 *
 *    Copyright Sierra Wireless Inc., 2008 All rights reserved
 *
 *    Notes: none
 */
#include "aa/aaglobal.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "us/usidefs.h"

/* Internal Definitions */
#define BHOLD_LPBK_RETRY_CNT 6          /*CDMA FIRMWARE LOOPBACK RESPONSE
                                         *TOO SOON ERROR WORKAROUND*/
/* Local Storage */
struct usdcb usdcbk;

/* Loopback request data buffer - statically allocated */
swi_uint8 uslbdata[] = {0x01,0x02,0x03,0x04};

/*
 *    Name: usgetdcbkp
 *
 *    Purpose: Returns a pointer to the US detection task's control block
 *             structure
 *
 *    Param:   None
 *
 *    Returns: Pointer to usdcbk
 *
 *    Abort:   None
 *
 *    Notes:   none
 */
package struct usdcb *usgetdcbkp( void )
{
    return &usdcbk;
}

/*
 *    Name: usgetep
 *
 *    Purpose: Return the endpoints to the caller This function should 
 *             only be called once, otherwise if the modem resets before 
 *             the caller uses the endpoint information, the results could 
 *             be invalid
 *
 *    Param:   eptype: enum useptype
 *             ep    : pointer to endpoint structure to return the read & 
 *                     write endpoints
 *
 *    Returns: TRUE:  endpoint available
 *             FALSE: endpoint is not available
 *
 *    Abort:   None
 *
 *    Notes:   This function should be called only just before the 
 *             call to the read or write function, otherwise the 
 *             EP information could be invalid. The function
 *             incorporates a call to usgetinfo() which forces a 
 *             check of the OS package's internal USB state. If the
 *             modem is active, then the state is fine and the EP
 *             returned will be the HIP endpoint. But if the modem
 *             is inactive, this function will return FALSE
 *             and the caller should not try to read or write to the
 *             modem.
 *
 *             There is still a small possibility that the modem can
 *             crash or be reset between the time this function is 
 *             called and the call to the US read or write entry.
 *             But if this happens, the call to the USB read or write 
 *             function would return an error.
 *             
 */
global swi_bool usgetep( enum useptype eptype, struct usbep* ep)
{
    swi_uint32 vendid, prodid;
    struct usdcb *usdcbkp;  /* US package control block pointer */

    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();

    /* Start with invalid endpoint */
    ep->usreadep  = USEPINVALID;
    ep->uswriteep = USEPINVALID ;
    
    /* First determine if a modem has been detected */
    if( !usgetinfo( &vendid, &prodid ) )
    {
        return FALSE;
    }

    /* We may never get here if the above failed but endpoints have already been
     * marked as invalid 
     */ 
    switch (eptype)
    {
        case USEP_HIP:
            ep->usreadep  = usdcbkp->useptab.ushipep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.ushipep.uswriteep ;
        break;
        case USEP_DIAG:
            ep->usreadep  = usdcbkp->useptab.usdiagep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usdiagep.uswriteep ;
        break;
        case USEP_NMEA:
            ep->usreadep  = usdcbkp->useptab.usnmeaep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usnmeaep.uswriteep ;
        break;
        case USEP_ATCMD:
            ep->usreadep  = usdcbkp->useptab.usatcmdep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usatcmdep.uswriteep ;
        break;
        case USEP_DATA1:
            ep->usreadep  = usdcbkp->useptab.usdata1ep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usdata1ep.uswriteep ;
        break;
        case USEP_DATA2:
            ep->usreadep  = usdcbkp->useptab.usdata2ep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usdata2ep.uswriteep ;
        break;
        case USEP_DATA3:
            ep->usreadep  = usdcbkp->useptab.usdata3ep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usdata3ep.uswriteep ;
        break;
        case USEP_MASS_STORAGE:
            ep->usreadep  = usdcbkp->useptab.usmassstorageep.usreadep ;
            ep->uswriteep = usdcbkp->useptab.usmassstorageep.uswriteep ;
        break;
        default:
            return FALSE;
        break;
    }
    
    if (ep->usreadep == USEPINVALID || ep->uswriteep == USEPINVALID)
    {
        return FALSE;
    }
    
    return TRUE;
}

/*
 *    Name: usmapepbyifnum
 *
 *    Purpose: populate struct usbep based on interface number
 *
 *    Param:   ifnum interface number
 *             ep - usbep structure to populate
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   None
 *             
 */
local void usmapepbyifnum( swi_uint32 ifnum, struct usbep* ep )
{
    ep->usreadep  = usifacetoep( ifnum, SWI_EP_IN );
    ep->uswriteep = usifacetoep( ifnum, SWI_EP_OUT );
}

/*
 *    Name: uslearneps
 *
 *    Purpose: Get all Endpoints for the currently enumerated modem
 *
 *    Param:   modemmode  USMODE_NORMAL for normal operation or 
 *                        USMODE_BOOT   for boot-and-hold operation
 *
 *    Returns: TRUE   - endpoints filled
 *             FALSE  - product id invalid ?
 *
 *    Abort:   None
 *
 *    Notes:   This table is hard-coded to return the read and 
 *             write endpoints on a per-product basis. There are
 *             two styles of USB descriptor in use with Sierra 
 *             modems, 1) the original 3-endpoint model and 2) 
 *             the newer 7-endpoint model used by the 7200 
 *             based chipset products. In model 1, the HIP endpoint
 *             is on the middle of the 3 interfaces, EP4. Note that 
 *             when these modems are operating in Boot Loader, the
 *             HIP traffic switches to EP2 (the first interface).
 *             In the second model, the HIP endpoint is always on 
 *             the first interface for both the boot loader and the
 *             application
 *             
 */
local swi_bool uslearneps( enum usmodemmode modemmode )
{
    swi_uint32 prodid, vendorid;
    struct usdcb *usdcbkp;  /* US package control block pointer */
    swi_bool retval = FALSE;
    
    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();

    /* Start by getting the Product ID so we know
     * which EP to return to the caller. Assumes
     * the modem is present
     */
    if( !usgetinfo( &vendorid, &prodid ) )
    {
        return retval;
    }

    /* Determine the HIP endpoint given the product ID */
    /* See document 2130634 USB Driver Developer's guide for the ep numbers */
    switch( prodid )
    {
        //--- CDMA Modems --- 
        case SWI_MC5725GM:    /* NEED VERIFICATION FROM CB THAT */
        case SWI_MC5725LM:    /* ALL THESE MODEMS USE THE OLDER */
        case SWI_MC5725HM:    /* 3 EP INTERFACE. The MC8785 and */
        case SWI_AC595GM:     /* AC885 PRODUCTS HAVE HIP ON THE */
        case SWI_AC595UGM:    /* OTHER ENDPOINT */
        case SWI_C597UM:
        case SWI_MC5727GM:
        case SWI_MC5727LM:
        case SWI_AC597EM:
        case SWI_T598UM:
        case SWI_AC802GM:
        case SWI_T11UM:
        case SWI_MC5728GM:
        case SWI_SL501XGM:
            /* Normal operating mode */
            if( modemmode == USMODE_NORMAL )
            {
                usdcbkp->useptab.ushipep.usreadep     = 4;
                usdcbkp->useptab.ushipep.uswriteep    = 4;
                usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usnmeaep.usreadep    = 5;
                usdcbkp->useptab.usnmeaep.uswriteep   = 5;
                usdcbkp->useptab.usatcmdep.usreadep   = 2;
                usdcbkp->useptab.usatcmdep.uswriteep  = 2;
                usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
            }
            /* For these modems, HIP endpoint moves 
             * around when modem is in boot-and-hold 
             * state 
             */
            else
            {
                usdcbkp->useptab.ushipep.usreadep     = 2;
                usdcbkp->useptab.ushipep.uswriteep    = 2;
                usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usnmeaep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usnmeaep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usatcmdep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usatcmdep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
            }
            retval = TRUE;
            break;

        case SWI_AC402GM:
            /* Normal operating mode */
            if( modemmode == USMODE_NORMAL )
            {
                usdcbkp->useptab.ushipep.usreadep     = 4;
                usdcbkp->useptab.ushipep.uswriteep    = 4;
                usdcbkp->useptab.usdiagep.usreadep    = 8;
                usdcbkp->useptab.usdiagep.uswriteep   = 8;
                usdcbkp->useptab.usnmeaep.usreadep    = 5;
                usdcbkp->useptab.usnmeaep.uswriteep   = 5;
                usdcbkp->useptab.usatcmdep.usreadep   = 2;
                usdcbkp->useptab.usatcmdep.uswriteep  = 2;
                usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
            }
            /* For these modems, HIP endpoint moves 
             * around when modem is in boot-and-hold 
             * state 
             */
            else
            {
                usdcbkp->useptab.ushipep.usreadep     = 2;
                usdcbkp->useptab.ushipep.uswriteep    = 2;
                usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usnmeaep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usnmeaep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usatcmdep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usatcmdep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
            }
            retval = TRUE;
            break;

        //--- GSM Modems ---
        case SWI_MC8775GM:
        case SWI_MC8775LM:
        case SWI_MC8775CM:
        case SWI_MC8775EM:
        case SWI_AC875G:
        case SWI_AC875UG:
        case SWI_AC875EG:
        case SWI_MC8780:
        case SWI_MC8781:
        case SWI_AC880U:
        case SWI_AC880G:
        case SWI_AC880EG:
        case SWI_AC881G:
        case SWI_AC881UG:
        case SWI_AC881EG:
            /* Normal operating mode */
            if( modemmode == USMODE_NORMAL )
            {
                usdcbkp->useptab.ushipep.usreadep     = 4;
                usdcbkp->useptab.ushipep.uswriteep    = 4;
                usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usnmeaep.usreadep    = 3;
                usdcbkp->useptab.usnmeaep.uswriteep   = 3;
                usdcbkp->useptab.usatcmdep.usreadep   = 5;
                usdcbkp->useptab.usatcmdep.uswriteep  = 5;
                usdcbkp->useptab.usdata1ep.usreadep   = 2;
                usdcbkp->useptab.usdata1ep.uswriteep  = 2;
                usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
            }
            /* For these modems, HIP endpoint moves 
             * around when modem is in boot-and-hold 
             * state 
             */
            else
            {
                usdcbkp->useptab.ushipep.usreadep     = 2;
                usdcbkp->useptab.ushipep.uswriteep    = 2;
                usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usnmeaep.usreadep    = USEPINVALID;
                usdcbkp->useptab.usnmeaep.uswriteep   = USEPINVALID;
                usdcbkp->useptab.usatcmdep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usatcmdep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
                usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
                usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
            }
            retval = TRUE;
            break;

        case SWI_AC885G:
        case SWI_AC885EG:
        case SWI_MC8785GC:
        case SWI_MC8790GC:
        case SWI_MC8777:
        case SWI_AC888G:
        case SWI_USBDIP:
            usmapepbyifnum( 0, &usdcbkp->useptab.ushipep  ); /* HIP */
            usmapepbyifnum( 1, &usdcbkp->useptab.usdiagep ); /* DIAG */
            usmapepbyifnum( 2, &usdcbkp->useptab.usnmeaep ); /* NMEA */
            usmapepbyifnum( 3, &usdcbkp->useptab.usatcmdep); /*  AT  */
            usmapepbyifnum( 4, &usdcbkp->useptab.usdata1ep); /* PDP1 */
            usmapepbyifnum( 5, &usdcbkp->useptab.usdata2ep); /* PDP2 */
            usmapepbyifnum( 6, &usdcbkp->useptab.usdata3ep); /* PDP3 */
            retval = TRUE;
            break;

        default:
            /* 
             * By default we assume the modem follows Static USB Interface
             * numbers. 
             */
            
            /* Static USB Interface numbers */
            usmapepbyifnum( 0, &usdcbkp->useptab.ushipep );  /* HIP */
            usmapepbyifnum( 1, &usdcbkp->useptab.usdiagep ); /* DIAG */
            usmapepbyifnum( 2, &usdcbkp->useptab.usnmeaep ); /* NMEA */
            usmapepbyifnum( 3, &usdcbkp->useptab.usatcmdep); /*  AT  */
            usmapepbyifnum( 4, &usdcbkp->useptab.usdata1ep); /* PDP1 */
            usmapepbyifnum( 5, &usdcbkp->useptab.usdata2ep); /* PDP2 */
            usmapepbyifnum( 6, &usdcbkp->useptab.usdata3ep); /* PDP3 */
            /*              7 Direct IP interface                    */
            /*              8 QMI Interface                          */
            /*              9 Mass Storage                           */
            retval = TRUE;
            break;
    }

    /* By now, the return EP values are either valid or both still zero */

    return retval;
}

/*
 *    Name: usinvalidateep
 *
 *    Purpose: Invalidate the endpoint
 *
 *    Param:   None
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   None
 *             
 */
local void usinvalidateep( void )
{
    struct usdcb *usdcbkp;

    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();
    
    usdcbkp->useptab.ushipep.usreadep     = USEPINVALID;
    usdcbkp->useptab.ushipep.uswriteep    = USEPINVALID;
    usdcbkp->useptab.usdiagep.usreadep    = USEPINVALID;
    usdcbkp->useptab.usdiagep.uswriteep   = USEPINVALID;
    usdcbkp->useptab.usnmeaep.usreadep    = USEPINVALID;
    usdcbkp->useptab.usnmeaep.uswriteep   = USEPINVALID;
    usdcbkp->useptab.usatcmdep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usatcmdep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usdata1ep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usdata1ep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usdata2ep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usdata2ep.uswriteep  = USEPINVALID;
    usdcbkp->useptab.usdata3ep.usreadep   = USEPINVALID;
    usdcbkp->useptab.usdata3ep.uswriteep  = USEPINVALID;
}

/*
 *    Name: usgetdevinfocb
 *
 *    Purpose: This entry is designed as a callback routine that will
 *             unblock the US Detect task in response to usdetecttask()
 *             requesting a get device information request. 
 *             The get device information response contains
 *             an indication of whether the get device information request 
 *             was successful or not.
 *
 *    Param:   resultcode - provided by the get device information handler. 
 *                          Indicates the overall success of the request
 *             userp      - NULL, unused
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   This function unblocks the detect task which has requested
 *             a get device information request be done and then blocks 
 *             itself until the get device information response calls this 
 *             function. 
 *             The parameter passed in is written directly into the US
 *             detection task's control block, but from the caller's 
 *             context, not the US task's. However, since the US task is 
 *             asleep, this should not cause any critical section problems.
 *
 *             This function runs outside the US Detection Task's context
 *
 */
local void usgetdevinfocb( enum hpphstatus resultcode, 
                           void *userp )
{
    struct usdcb *usdcbkp;

    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();

    /* Save this for later inspection when the task resumes */
    usdcbkp->usddevresult = resultcode;

    /* Wake the Detection task now */
    swi_ossdkwaiterwake( &usdcbkp->usdwaiter );

    /* The detection task will continue from where it left off */
}

/*
 *    Name: usdetmodmstate
 *
 *    Purpose: This entry is designed as a callback routine that will
 *             unblock the US Detect task in response to usdetecttask()
 *             requesting a loopback test. The loopback response contains
 *             an indication of whether the modem is operating in Boot and
 *             Hold (Boot Loader) or the application and this information
 *             dictates which endpoint to use for HIP traffic on some types
 *             of modems.
 *
 *    Param:   resultcode - provided by the loopback handler. Indicates 
 *                          the overall success of the request
 *             runningapp - TRUE, the application is running in the modem
 *                          FALSE, the boot loader is running
 *             userp      - NULL, unused
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   This function unblocks the detect task which has requested
 *             a loopback test be done and then blocks itself until the 
 *             loopback is complete and that task calls this function. 
 *             The parameter passed in is written directly into the US
 *             detection task's control block, but from the caller's 
 *             context, not the US task's. However, since the US task is 
 *             asleep, this should not cause any critical section problems.
 *
 *             This function runs outside the US Detection Task's context
 *
 */
local void usdetmodmstate( enum hpphstatus resultcode, 
                           swi_bool runningapp, 
                           void *userp )
{
    struct usdcb *usdcbkp;

    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();

    /* Save this for later inspection when the task resumes */
    usdcbkp->usdlbresult = resultcode;

    /* May or may not be valid, depending upon result code 
     * which is checked in the task when it wakes up
     */
    usdcbkp->usdmdmstate = runningapp;
    
    /* Wake the Detection task now */
    swi_ossdkwaiterwake( &usdcbkp->usdwaiter );

    /* The detection task will continue from where it left off */
}

/*
 *    Name: usdoairdipchk
 *
 *    Purpose: This function is used by the Detection Task get the Modem's
 *             technology type and product name string using  HIP after a 
 *             successful a loopback request. It is a blocking call 
 *             and on return the function will have been wakened by the
 *             completion of the HIP requests.
 *
 *    Param:   Prodid - the product ID of the modem to perform Device 
 *                      Information request. Only Products that use the 
 *                      new unified PID will require the information to be
 *                      obtained using HIP.
 *
 *    Returns: TRUE  - successfulquery
 *             FALSE - failed to retrieve device information
 *
 *    Abort:   None
 *
 *    Notes:   This function executes inside the US Detection task's 
 *             context
 *
 *             On exit, the values of HPPH should have updated the Air Server
 *             info that it maintains.
 */
local swi_bool usdoairdipchk( swi_uint32 prodid )
{
    struct usdcb *usdcbkp;

    /* We are only interested Direct IP products here */
    if ( prodid != SWI_USBDIP)
    { 
        return TRUE;
    }
    /* Get a pointer to the US detect task control block */
    usdcbkp = usgetdcbkp();
    
    /* stay in this loop until the HIP get tech type is successful */
    while( 1 )
    {
        /* Request the initiation of a get product string request */
        hpgetdevinfo( usgetdevinfocb, NULL, 
                      HPMDEVREQVERSION, HPMDEVREQTECHTYP, CIINVIPCCHAN);

        /* Block this task waiting for the callback, wait forever */
        swi_ossdkwaiterwait( &usdcbkp->usdwaiter, 0 );

        /* Log a failed result */
        if( usdcbkp->usddevresult == HPOK )
        {
            /* Can exit the while loop */
            break;
        }
        else
        {
            /* Log the error return */
            dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA,
                    "Get Tech type error %d",
                    (swi_uint32)usdcbkp->usddevresult, 
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            return FALSE;
        }
    }

    /* stay in this loop until the HIP get product string is successful */
    while( 1 )
    {
        /* Request the initiation of a get product string request */
        hpgetdevinfo( usgetdevinfocb, NULL, 
                      HPMDEVREQVERSION, HPMDEVREQPRDSTR, CIINVIPCCHAN);

        /* Block this task waiting for the callback, wait forever */
        swi_ossdkwaiterwait( &usdcbkp->usdwaiter, 0 );

        /* Log a failed result */
        if( usdcbkp->usddevresult == HPOK )
        {
            /* Can exit the while loop */
            break;
        }
        else
        {
            /* Log the error return */
            dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA,
                    "Get Product string error %d",
                    (swi_uint32)usdcbkp->usddevresult, 
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            return FALSE;
        }
    }

    return TRUE;
}

/*
 *    Name: usdoloopback
 *
 *    Purpose: This function is used by the Detection Task to initiate
 *             a loopback request. It is a blocking call and on return
 *             the function will have been wakened by the loopback task
 *             when the loopback test results are known.
 *
 *    Param:   Prodid - the product ID of the modem to perform the loopback
 *                      test on
 *
 *    Returns: TRUE, the modem is executing the application, 
 *             FALSE, the boot image
 *
 *    Abort:   None
 *
 *    Notes:   This function executes inside the US Detection task's 
 *             context
 *
 *             On exit, the values of ushipreadep and ushipwriteep
 *             should contain usable endpoint numbers
 */
local swi_bool usdoloopback( swi_uint32 prodid )
{
    struct usdcb *usdcbkp;
    enum usmodemmode modemmode;    /* Selects calling param for uslearnhipep() */
    enum usmodemmode oldmode;      /* Selects calling param for uslearnhipep() */
    struct usbep eps[USMODE_MAX];  /* Remember the EP's for each iteration */
    int retry = 0;

    /* Get a pointer to the US detect task control block */
    usdcbkp = usgetdcbkp();
    
    /* Initialize the result code to invalid */
    usdcbkp->usdlbresult = HPINVALID;

    /* Here's the situation: we know the modem has just enumerated but
     * we don't know if it's running in boot and hold mode or normal
     * mode. In order to find this out, we need to send it a loopback
     * request - the response to which indicates the modem's mode. But,
     * we also need to know which endpoint to send the loopback test to
     * and this can change for certain types of Sierra Wireless modems, 
     * depending on whether they are running in boot or normal mode. So
     * there's a chicken-and-egg kind of problem. We have no choice but
     * to use trial-and-error to figure out the endpoint. So we'll stay
     * in this function alternating between endpoints until we get a 
     * response to the loopback request. That response contains positive
     * ID about which endpoint to use
     */

    /* Initialize the EP struct to invalid values */
    eps[USMODE_BOOTHOLD].usreadep = USEP_INVALID;
    eps[USMODE_BOOTHOLD].uswriteep = USEP_INVALID;
    eps[USMODE_NORMAL].usreadep = USEP_INVALID;
    eps[USMODE_NORMAL].uswriteep = USEP_INVALID;

    /* Initially assume the modem is in boot and hold mode */
    oldmode = USMODE_NORMAL;
    modemmode = USMODE_BOOTHOLD;

    /* stay in this loop until the HIP loopback test is successful */
    while( 1 )
    {
        /* Assign the endpoints for this iteration. Any package that
         * calls usgetep() after this will be given the EPs assigned
         * right here
         */
        uslearneps( modemmode );

        /* Keep a local copy of the EP we'll be trying */
        usgetep( USEP_HIP, &eps[modemmode] );

        /* Request the initiation of a loopback request */
        hplooprequest( uslbdata, sizeof( uslbdata ), usdetmodmstate, NULL );

        /* Block this task waiting for the callback, wait forever */
        swi_ossdkwaiterwait( &usdcbkp->usdwaiter, 0 );

        /* Log the result */
        if(
            ((usdcbkp->usdlbresult == HPOK) && (retry >= BHOLD_LPBK_RETRY_CNT)
             && (modemmode == USMODE_BOOTHOLD)) ||
            ((usdcbkp->usdlbresult == HPOK) && (modemmode == USMODE_NORMAL))
          )
        {
            /* Log the outcome before leaving */
            dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA,
                    "Loopback complete. Mode: %d, Read EP: %d, Write EP: %d",
                    (swi_uint32)usdcbkp->usdmdmstate, 
                    (swi_uint32)usdcbkp->useptab.ushipep.usreadep, 
                    (swi_uint32)usdcbkp->useptab.ushipep.uswriteep,
                    (swi_uint32)NULL );

            /* Can exit the while loop */
            if (usdoairdipchk( prodid ))
                break;
        }
        else
        {
            /* Log the error return */
            dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA,
                    "LB test ret'd error %d, retrying on ep %d",
                    (swi_uint32)usdcbkp->usdlbresult, 
                    (swi_uint32)usdcbkp->useptab.ushipep.uswriteep,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            /* Remember the one that *didn't* work - we might need to 
             * close the port later when we're done with the loopback
             * test
             */
            oldmode = modemmode;
        }

        /* Switch our assumption to the opposite of the current one. 
         * Don't change "enum usmodemmode" without fixing this logic!!
         */
        if( ++modemmode >= USMODE_MAX )
            modemmode = USMODE_MIN;

        retry++;
    }

    /* If the BOOT mode and NORMAL mode EPs are not the same, 
     * close whichever one is NOT the one we locked on to. This
     * will be the one whose values are associated with "oldmode"
     */
    if( eps[USMODE_NORMAL].usreadep != eps[USMODE_BOOTHOLD].usreadep )
    {
        /* The following check catches the case where the first
         * loopback was answered immediately and we never even
         * needed to load the local structure with the alternate
         * EP values. Sufficient to check one of the pair of EPs
         */
        if( eps[oldmode].uswriteep != USEP_INVALID )
        {
            dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA,
                    "Closing alternate read/write EP: %d/%d", 
                    (swi_uint32)eps[oldmode].usreadep,
                    (swi_uint32)eps[oldmode].uswriteep,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            usclosetty( &eps[oldmode] );
        }
    }
    
    return usdcbkp->usdmdmstate;
}

/*
 *    Name: usdetecttask
 *
 *    Purpose: The US task responsible for performing device detection.
 *             This task depends upon the usscan task for correct 
 *             operation, without that task, this one would block forever
 *             if no modem is connected.
 *
 * Parms:      paramp - pointer to data that may be passed in when this task
 *                      is started.
 *
 *    Returns: None
 *
 *    Abort:   Noneknown
 *
 *    Notes:   If no modem available, block until one appears. Then, 
 *             update the HIP endpoint number for other packages to
 *             use. 
 *
 *             When there is no modem, either because it has disappeared
 *             or is resetting, etc, client threads that interact with 
 *             the modem's USB endpoints must suspend operation until they
 *             are given the go ahead to proceed. This go-ahead is in the
 *             form of a valid EP in response to their request for an EP.
 *
 *             This task is slaved off of the scanning task. Once that task
 *             has started and initialized, this one is kicked off 
 */
package void usdetecttask( void *paramp )
{
    swi_uint32 vendid, prodid;
    struct usdcb *usdcbkp;
    swi_bool modemmode;

    /* Get a pointer to the US package control block */
    usdcbkp = usgetdcbkp();

    /* Initialize the task's waiter structure */
    swi_ossdkwaiterinit( &usdcbkp->usdwaiter);

    /* Register with DL for logging, initially enabled */
    dlregister( "USDT", &usdcbkp->usdldcb, TRUE );

    /* Log our start */
    dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA, 
            "Detection task startup\n",
             (swi_uint32)NULL,
             (swi_uint32)NULL,
             (swi_uint32)NULL,
             (swi_uint32)NULL);

    /* Call the SU callback to let the next task in line go */
    sutaskinitcb( SDK_USDPKG );
 
    /* Endless loop */
    for( ;; )
    {
        /* Sleep a little between detection updates */
        swi_ossdkwaiterwait( &usdcbkp->usdwaiter, USSLEEPDETECT );
 
        /* If the information is valid, the modem is present */
        if(usgetinfo( &vendid, &prodid ))
        {
            /* Ensure we only do this on a change of state */
            if( usdcbkp->useptab.ushipep.usreadep == USEPINVALID )
            {
                /* Log that we've seen a modem */
                dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA, 
                        "usdet: Modem detected. VID %x, PID %x\n", 
                        (swi_uint32)vendid, 
                        (swi_uint32)prodid,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);

                /* begin the loopback test and don't proceed from
                 * this point until the successful result is known
                 */
                modemmode = usdoloopback( prodid );

                /* 
                 * If we got a Direct IP modem, we need to go and get
                 * the Product string to map to a modem type later.
                 */
                /* Generate an airserver change notification */
                hpairchangereq( TRUE, modemmode );

                /* Configure the read termination parameters
                 * for the HIP endpoint. This is done here 
                 * instead of the HP Rx task because here we
                 * only need to do it once when the modem is
                 * first detected. In the Rx task, without a
                 * lot of complexity we'd have to do it every
                 * time through the Rx loop. Configure the 
                 * HIP read ep to return when matching the
                 * flag character
                 */
                 usreadterms( usdcbkp->useptab.ushipep.usreadep, 0x7E,
                              USNOTHRESHOLD );
            }
        }
        /* No modem yet available */
        else
        {
            /* Only do this on a change of state */
            if( usdcbkp->useptab.ushipep.usreadep != USEPINVALID )
            {
                /* Log that we've seen a modem */
                dlLog(  &usdcbkp->usdldcb, USLOG_CLASSA, 
                        "usdet: Modem disconnected, invalidating EPs\n",
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);

                /* Generate an airserver change notification. NOTE, 
                 * the second arg is invalid since the modem is offline
                */
                hpairchangereq( FALSE, FALSE );

                /* Clear out the previous read termination 
                 * settings in case the next modem is a 
                 * different type than the one that just 
                 * disappeared
                 */
                usreadterms(usdcbkp->useptab.ushipep.usreadep,
                            USNOTERMINATION,USNOTHRESHOLD);

                /* Invalidate the EPs */
                usinvalidateep();

            }
        }
    }
}

/*
 *    Name: usdtaskinit
 *
 *    Purpose: To create the US detect task
 *
 *    Param:   None
 *
 *    Returns: None
 *
 *    Abort:   None
 *
 *    Notes:   None
 */
global void usdtaskinit( void )
{
    swi_ossdkthreadcreate( usdetecttask, NULL );
}

/*
 * $Log: usdetect_sdk.c,v $
 */
