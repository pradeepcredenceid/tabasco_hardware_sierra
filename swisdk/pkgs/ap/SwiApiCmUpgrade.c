/**
 *    \if CvsId 
 *    $Id: SwiApiCmUpgrade.c,v 1.15 2010/06/10 00:16:17 epasheva Exp $
 *    \endif
 *    \ingroup cmupgrade
 *    \file SwiApiCmUpgrade.c
 * 
 *    \brief  Contains source code for GSM APIs related to the 
 *            Firmware Download
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmUpgrade.h"
#include "ap/apidefs.h"

/* functions */

/**
 *
 * This function is used to backup modem data before a firmware upgrade.  
 *
 * Operation should be set to SWI_NVOPER_Backup and category should be
 * set to SWI_NVCAT_User.  All other fields are ignored.  Caller should
 * enable notification for SWI_NOTIFY_NvStore.  The SWI_NOTIFY_NvStore
 * notification will tell the caller whether or not the operation succeeded. 
 *
 * \param      sNvStore: [IN]    
 *                     Details type of operation and data to backup
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been received by the modem.
 *
 * \return     See SWI_RCODE for other error return codes.
 *
 * \sa struct SWI_STRUCT_NvStore
 * \sa SWI_NOTIFY_NvStore 
 *
 */
SWI_API SWI_RCODE SwiNvStore(
    struct  SWI_STRUCT_NvStore sNvStore, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
  
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );
    
    /* 
     * Pack outgoing parameters 
     * Note that currently object version 1 of this interface is supported
     */
    parmlength = SwiPkNvStore( outparamp, 
                               APOBJVER1, 
                               &sNvStore );
    parmlength = 20; /* this was what it was before conversion to
                      * SwiIntSendnWait -specfied in CNS doc 2110635
                      */
  
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_NVRAM_STORE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 * This function is used to initiate the download of a single firmware
 * image to the device.
 *
 * \param      szFilename [IN]                     
 *                     Name of the file containing the image to be downloaded
 *
 * \param      eFwDwnType [IN]                     
 *                     Type of firmware image to be downloaded
 *
 * \param      bMpDownload [IN]                     
 *                     Flag indicating if this is a MP download request
 *
 * \param      nHIPPayloadSize    [IN]                     
 *                     Block size to be used when copying the image to the 
 *                     device.
 *
 * \param      timeout [IN]
 *                     API timeout parameter in ms. Note, this API ignores
 *                     this parameter
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been received by the modem.
 *
 * \return     See SWI_RCODE for other error return codes.
 *
 * \sa enum SWI_TYPE_FW_IMAGE
 * \sa SWI_NOTIFY_FwDownload
 *
 */
SWI_API SWI_RCODE SwiFirmwareDownload(
    swi_charp szFileName, 
    enum SWI_TYPE_FW_IMAGE eFwDwnType,
    swi_bool bMpDownload,
    swi_uint32 nHIPPayloadSize, 
    swi_uint32 timeout)
{
    /* Send the request on its way - Note, this is a blocking call */
    return(SwiSendnWaitFwdDwnRequest(szFileName, eFwDwnType, 
                                     FALSE, nHIPPayloadSize ));
}

/**
 *
 * This function is used to retrieve the last error that occurred 
 * during the firmware download process.
 *
 * A request is sent to the modem.  The modem's response contains the
 * firmware download error.
 *
 * \param   szError[OUT]:       firmware download error
 *
 * \param   sizeBuffer[IN]:     length of buffer provided for the firmware 
 *                              download error
 *
 * \return  SWI_RCODE_OK - the request has been received by the modem
 * \return  SWI_RCODE_REQUEST_TIMEOUT  - the  request has timed out
 * \return  See SWI_RCODE for other error return codes
 *
 */
SWI_API void SwiGetFwDownloadError(
    swi_charp szError, 
    swi_uint32 sizeBuffer)
{

}

/* 
 * $Log: SwiApiCmUpgrade.c,v $
 */
