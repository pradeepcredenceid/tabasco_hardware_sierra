/**
 *    \if CvsId
 *    $Id: SwiApiCdmaIota.c,v 1.3 2009/02/10 01:41:44 bsiu Exp $
 *    \endif
 *    \ingroup cdmaiota
 *    \file SwiApiCdmaIota.c
 * 
 *    \brief SWI CDMA specific IOTA related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaIota.h"
#include "ap/apidefs.h"

/**
 *
 * This function starts Embedded IOTA session
 * Indication of session will be notified through notification.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiStartIOTASession(swi_uint32 timeout)
{
    return SwiIntSet(CNS_IOTA_START_SESSION_LEGACY, SWI_AMCDMA, timeout);
}

/**
 *
 * This function stops Embedded IOTA session.
 * Indication of session will be notified through notification.
 *
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiStopIOTASession(swi_uint32 timeout)
{
    return SwiIntSet(CNS_IOTA_STOP_SESSION_LEGACY, SWI_AMCDMA, timeout);
}

/**
 *
 * This function disables or enables IOTA in the modem.
 *
 * \param bDisable[IN]     TRUE: Disable; FALSE: Enable.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetIOTADisable(swi_bool bDisable, swi_uint32 timeout)
{
    return SwiIntSetU8(CNS_IOTA_DISBALE_LEGACY, (bDisable ? 1 : 0),
                       SWI_AMCDMA, timeout);
}

/**
 *
 * This function gets iota disable/enable state
 *
 * \param pbDisable[OUT]    pointer to save return value.
 *                          TRUE: IOTA is disabled; FALSE: Enabled.
 * \param timeout[IN]       Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetIOTADisable(swi_bool *pbDisable, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8 nValue;
    resultcode = SwiIntGetU8(CNS_IOTA_DISBALE_LEGACY, &nValue,
                             SWI_AMCDMA, timeout);
    if( resultcode == SWI_RCODE_OK )
    {
        *pbDisable = ((0 == nValue) ? FALSE : TRUE);
    }
    return resultcode;
}

/*! Embedded IOTA  log file name. */
#define SWI_EMIOTA_FILE     "iota_log"

/**
 *
 * This function gets IOTA process Log.
 *
 * \param pBuffer[OUT]     Buffer to hold the complete log data.
 * \param nBufLen[IN]      Data size. (Obtained from SwiGetIOTALogSize )
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetIOTALog(swi_uint8* pBuffer, swi_uint16 nBufLen,
                                swi_uint32 timeout)
{
    SWI_RCODE  resultcode; /* Result of call to cnSendWait() */
    
    resultcode = SwiGetEFile(SWI_EMIOTA_FILE, pBuffer, nBufLen, timeout);
    pBuffer[nBufLen] = '\0';
    return resultcode;
}

/**
 *
 * This function gets IOTA process Log file size.
 *
 * \param nBufLen[OUT]     Log data size.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetIOTALogSize(swi_uint16 *nBufLen, swi_uint32 timeout)
{
    SWI_RCODE  resultcode; /* Result of call to cnSendWait() */
    SWI_STRUCT_EFileInfo    sEf;
    sEf.sizeStruct  = sizeof(SWI_STRUCT_EFileInfo);

    resultcode = SwiGetEFileInfo(SWI_EMIOTA_FILE, &sEf, timeout);
    if (SWI_RCODE_OK == resultcode) 
    {
        *nBufLen = (swi_uint16)(sEf.nSize + 1 );
    }
    else
    {
        *nBufLen = 0;
    }

    return resultcode;
}

/* 
 * $Log: SwiApiCdmaIota.c,v $
 */
