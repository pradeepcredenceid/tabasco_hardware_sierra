/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides stub functions for the swiril AT-vendor specific
 * RIL code. This is required because several files are used
 * by both the QMI and the AT RIL. Functions in this file
 * must exist to satisfy the linker but need not do anything
 * useful, except maybe to return a specific value to the
 * caller.
 *
 * @author
 * Copyright: © 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */
#include <stdbool.h>
#include <at_channel.h>


/**
 * 
 * Stub function for AT RIL. The AT RIL does not support MC7750-like modems
 * so this function can safely return false all the time. 
 *
 * @return
 *     true  - the module is MC7750-like dual mode modem
 *     false - oherwise
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      none
 */ 
bool isDualModeModule(void)
{
    return false;
}

/**
 * 
 * Stub function for AT RIL. The AT RIL does not support MC7750-like modems
 * so this function can safely return false all the time. 
 *  
 * @return
 *     true  - the module is MC7750-like dual mode modem and registered on LTE network
 *     false - oherwise
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      none
 */ 
bool isDualModeRunningLTE(void)
{
    return false;
}

/**
 * Try using QMI API to authenticate PIN2.
 *
 * It is done using AT command for SL8092 modem, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
unsigned long authenticateSIMPIN2QMI(char *pSIMPIN2)
{
    return 0xFF;
}



/**
 * 
 * AT RIL to reset modem.  
 *  
 * @return
 *     None
 *
 * @par Abort:<br> 
 *      none
 *
 * @note
 *      none
 */ 
void setPowerOff(void)
{
    /* NULL is not defined, so use atresponse to make compiler happy */
    ATResponse *atresponse;
    
    at_send_command("AT!RESET", &atresponse);
    at_response_free(atresponse);
}

/**
 * Determine whether a delayed SIM_READY notification should be used
 *
 * Currently only needed for QMI RIL, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
bool useDelayedSIMReady(void)
{
    return false;
}

/**
 * Try using QMI API to get retry count for PIN.
 *
 * Currently only needed for QMI RIL, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
int getPINRetryCountQMI(void)
{
    return -1;
}

/**
 * It is only needed for QMI RIL, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
void setCLIRVal(int clirVal)
{
    /* Nothing needs to be done because it will never be invoked */
}

/**
 * Try using QMI API to get MSISDN.
 *
 * Currently only needed for QMI RIL, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
unsigned long GetVoiceNumber(
    unsigned char voiceNumberSize,
    char *pVoiceNumber,
    unsigned char minSize,
    char *pMIN )
{
    /* return success */
    return 0;
}


/**
 * It is only needed for QMI RIL, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
int GetModelID(
    char     stringSize,
    char    *pString )
{
    return -1;
}



/**
 * It is only needed for QMI RIL, but defined here because it is called
 * from common code in swicommon/swiril_sim.c
 */
unsigned long GetFirmwareRevision(
    unsigned char     stringSize,
    char    *pString )
{
    return -1;
}

/**
 * 
 * Stub function for AT RIL, it is required only for QMI RIL
 */ 
bool isFirmwareRunningCDMA()
{
    return false;
}

/**
 * 
 * Stub function for AT RIL, it is required only for QMI RIL
 */ 
bool isModemNetworkRegistered()
{
    return false;
}

bool getFirmwareInfoCarrierQMI(int *image)
{
   return false;
}

bool Is9x15Device(void) 
{
    return false;
}

bool Is9x30Device(void) 
{
    return false;
}


