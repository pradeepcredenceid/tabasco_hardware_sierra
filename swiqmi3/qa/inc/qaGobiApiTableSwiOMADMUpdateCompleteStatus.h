/**
 * \ingroup swioma
 *
 * \file    qaGobiApiTableSwiOMADMUpdateCompleteStatus.h
 *
 * \brief   Update Complete Status table
 *
 * \section Table12 OMA DM Update Complete Status (Update Complete Status - Meaning - Usage)
 * \li 200 - Successful - The request has succeeded
 * \li 250-299 - Successful(vendor specified) - successful operation with vendor specified ResultCode
 * \li 400 - Management Client Error - Management Client error - based on User or Device behavior
 * \li 401 - User Cancelled - User chose not to accept the operation when prompted
 * \li 402 - Corrupted Firmware Update Package - Corrupted firmware update package did not store correctly. Detected for example, by mismatch CRCs between actual and expected
 * \li 403 - Firmware UpdatePackage( Device Mismatch ) - Wrong firmware update package delivered to device based on current device characteristics
 * \li 404 - Failed Firmware Update Package Validation - Failure to positively validate digital signature of firmware update package
 * \li 405 - Firmware Update Package Not acceptable - firmware update package is not acceptable
 * \li 406 - Alternate Download Authentication Failure - authentication required but authentication failure was encountered when downloading firmware update package
 * \li 407 - Alternate Download Request Timeout - client has encountered a timeout when downloading firmware update package
 * \li 408 - Not Implemented - the device does not support the requested operation
 * \li 409 - Undefined Error - indicates failure not defined by any other error code
 * \li 410 - Firmware Update Failed - firmware update operation failed in device
 * \li 411 - Malformed or Bad URL - the URL provided for alternate download is bad
 * \li 412 - Alternate Download Server Unavailable - the alternate download server is unavailable or does not respond
 * \li 450 - Client Error ( OMADM General ) - Vendor defined client error
 * \li 451 - Client Error ( OMADM SyncML ) - Vendor defined client error
 * \li 452 - Client Error ( OMADM Auth ) - Vendor defined client error
 * \li 453 - Client Error ( OMADM Protocol )- Vendor defined client error
 * \li 454 - Client Error ( OMADM Tree ) - Vendor defined client error
 * \li 455 - Client Error ( OMADM DStore ) - Vendor defined client error
 * \li 456 - Client Error ( OMADM Trigger ) - Vendor defined client error
 * \li 457 - Client Error ( OMADM Fumo ) - Vendor defined client error
 * \li 458 - Client Error ( OMADM Comms ) - Vendor defined client error
 * \li 459 - Client Error ( OMADM Parse ) - Vendor defined client error
 * \li 460 - Client Error ( OMADM TNDS ) - Vendor defined client error
 * \li 461 - Client Error ( OMADM SCM ) - Vendor defined client error
 * \li 462 - Client Error ( OMADM Impl ) - Vendor defined client error
 * \li 463-499 - Client Error ( Vendor Specified ) - client error encountered for operation with vendor specified result code
 * \li 500 - Alternate Download Server Error - Alternate download server error encountered
 * \li 501 - Download fails due to device out of memory - The download fails due to insufficient memory in the device to save the firmware update package
 * \li 502 - Firmware update fails due to device out of memory - The update fails because there isn't sufficient memory to update the device
 * \li 503 - Download fails due to network issues - The download fails due to network/transport level errors
 * \li 550-599 - Alternate Download Server Error (vendor specified)- Alternate download server error encountered for operation with vendor specified result code
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_OMADM_UPDATE_COMPLETE_STATUS_H__
#define __GOBI_API_OMADM_UPDATE_COMPLETE_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_OMADM_UPDATE_COMPLETE_STATUS_H__ */
