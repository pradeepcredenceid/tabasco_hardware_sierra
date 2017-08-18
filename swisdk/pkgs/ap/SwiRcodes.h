/**
 *    \if CvsId
 *    $Id: SwiRcodes.h,v 1.5 2010/06/10 00:26:12 epasheva Exp $
 *    \endif
 *    
 *    \file SwiRcodes.h
 *
 *    \brief SWI return codes
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWI_RCODES_H
#define __SWI_RCODES_H

/**
 *
 * All function calls will return one of the return codes.  In the event
 * you receive a return code not enumerated in this list, notify Sierra
 * Wireless via e-mail to sdk@sierrawireless.com and report the function
 * call and the return code received. This should be treated as a fatal
 * error in the system.
 *
 * A general interpretation of return codes is provided below. Only 
 * return codes with special handling will be discussed in the function 
 * descriptions.
 *            SWI_RCODE_OK,
 *            SWI_RCODE_NOT_SUP,
 *            SWI_RCODE_SMALL_BUF,
 *            SWI_RCODE_CLOSED,                        
 *            SWI_RCODE_INVALID_PAR,
 *            SWI_RCODE_FAILED,
 *            SWI_RCODE_UNEXPECTED_RESP,
 *            SWI_RCODE_REQUEST_TIMEOUT,
 *            SWI_RCODE_BUSY,
 *            SWI_RCODE_SYSTEM_ERROR,                    
 *            SWI_RCODE_DEVICE_UNAVAIL,
 *            SWI_RCODE_ALREADY_OPEN,                    
 *            SWI_RCODE_DEVICE_INUSE,
 *            SWI_RCODE_SERVICE_INUSE,
 *            SWI_RCODE_DEVICE_LOCKED,
 *            SWI_RCODE_API_SYS_ERR,
 *            SWI_RCODE_API_NOT_OPEN,
 *            SWI_RCODE_NOT_INIT,
 *            SWI_RCODE_NOT_COMPATIBLE,
 *            SWI_RCODE_DEVICE_NOT_SUP,
 *            SWI_RCODE_RESP_PENDING,
 *            SWI_RCODE_DEVICE_NOT_SEL,
 *            SWI_RCODE_TOO_MANY_TRANSPORT_CLIENTS,
 *            SWI_RCODE_BIND_FAILED
 * 
 ***************************************************************************
 *
 *            CDMA API error codes:
 *            SWI_RCODE_INVALID_FUNCPTR,
 *            SWI_RCODE_PWR_CTRL_GET_CTRLPORT_FAILED,
 *            SWI_RCODE_PWR_CTRL_GET_DATAPORT_FAILED,
 *            SWI_RCODE_PWR_CTRL_GET_VSP_PORTS_FAILED
 *
 ***************************************************************************
 *
 *            If the following CNS error codes are received, your API and firmware
 *            versions are most likely incompatible.
 *            SWI_RCODE_CNS_FAILED,                    
 *            SWI_RCODE_CNS_OPERATION_ID_INVALID,
 *            SWI_RCODE_CNS_OBJECT_ID_INVALID,            
 *            SWI_RCODE_CNS_UNEXPECTED_NOTIFICATION,
 *            SWI_RCODE_CNS_ILLEGAL_NOTIFICATION,        
 *            SWI_RCODE_CNS_CALLBACK_FAILURE            
 *
 ***************************************************************************
 *
 *            If the following general API operation failure codes are received, the most
 *            likely causes are:
 *            1) not registering callback immediately after starting the API
 *            2) not releasing the notification thread in a timely manner
 *            SWI_RCODE_FRAME_QUEUE_FULL,
 *            SWI_RCODE_FRAME_QUEUE_EMPTY,        
 *            SWI_RCODE_PUT_TIMEOUT,                    
 *            SWI_RCODE_GET_TIMEOUT,                    
 *            SWI_RCODE_BAD_FRAME,        
 *            SWI_RCODE_CANCEL                        
 *
 ***************************************************************************
 *
 *            If the following CNS error codes are received, your API and firmware
 *            versions are most likely incompatible.
 *            SWI_RCODE_CWE_FAILED,                    
 *            SWI_RCODE_CWE_MSG_ID_INVALID,            
 *            SWI_RCODE_CWE_UNEXPECTED_NOTIFICATION,  
 *            SWI_RCODE_CWE_ILLEGAL_NOTIFICATION,        
 *            SWI_RCODE_CWE_CALLBACK_FAILURE          
 *
 ***************************************************************************
 *
 *            If the following OID error codes are received, your API and driver
 *            versions are most likely incompatible.
 *            SWI_RCODE_OID_BUFFER_ID_INVALID,            
 *            SWI_RCODE_OID_FAILED,    
 *            SWI_RCODE_OID_UNEXPECTED_NOTIFICATION,    
 *            SWI_RCODE_OID_ILLEGAL_NOTIFICATION,        
 *            SWI_RCODE_OID_CALLBACK_FAILURE            
 *
 ***************************************************************************
 *
 *            If the following LINK error codes are received, your API and driver
 *            versions are most likely incompatible.
 *            SWI_RCODE_LINK_FAILED,                    
 *            SWI_RCODE_LINK_MSG_ID_INVALID,            
 *            SWI_RCODE_LINK_NO_PENDING_DATA,            
 *            SWI_RCODE_PORT_OPEN_FAIL,                
 *            SWI_RCODE_PORT_INITIALIZE_FAIL            
 *
 ***************************************************************************
 *
 *            If the following DATA error codes are received, the most likely causes are:
 *            1) an incorrect API version or incompatible driver version is used
 *            2) not registering callback immediately after starting the API
 *            SWI_RCODE_DATA_INIT_FAILED,                
 *            SWI_RCODE_DATA_SEND_FAILED                
 *
 ***************************************************************************
 *
 *            If the following error codes are received, your API and firmware
 *            versions are most likely incompatible.
 *            SWI_RCODE_PROFILE_FAILED,                
 *            SWI_RCODE_PROFILE_MSG_ID_INVALID,        
 *            SWI_RCODE_PROFILE_OP_CODE_INVALID,
 *            SWI_RCODE_REQUEST_REJECTED,                
 *            SWI_RCODE_CAIT_INIT_FAILED                
 *
 ***************************************************************************
 *
 *            MUX return codes:
 *            SWI_RCODE_MUX_CON_FAILED,        
 *            SWI_RCODE_MUX_FAILED            
 *
 ***************************************************************************
 * 
 *            Discovery return codes:
 *            SWI_RCODE_DISCOVERY_INVALID_MESSAGE
 *
 ***************************************************************************
 *
 * \note   You must use the defined enumeration types, not the numeric values. 
 *         Sierra Wireless may change the numbering in future editions of the API.
 *
 */
typedef enum
{
    SWI_RCODE_OK,   /*!< The function executed properly.  Any output data returned is
                    *  deemed valid.  Some functions do have additional status 
                    *  outputs to indicate the validity of the returned data. It is 
                    *  possible that the function can execute normally but due to modem
                    *  or network conditions, the output is not meaningful. Such cases
                    *  are noted in the function descriptions. */
    SWI_RCODE_NOT_SUP,  /*!< The function is defined for other technologies and is not
                        *  supported on this particular product. */
    SWI_RCODE_SMALL_BUF,    /*!< A buffer length passed for an output string parameter was not 
                            *  large enough to receive the output. The return data is invalid.
                            *  A repeat call with a longer buffer should succeed. */
    SWI_RCODE_CLOSED,   /*!< Not used. */
    SWI_RCODE_INVALID_PAR,  /*!< The function included a parameter outside of the valid range.
                             *  This is commonly caused by a null pointer, or a zero timeout
                             *  setting.  This is also returned when trying to connect to a
                             *  device in mass storage mode.  Communication with a device in mass
                             *  storage mode is not possible. */
    SWI_RCODE_FAILED,   /*!< The function failed to execute correctly.  Any return data
                        *  should be ignored. A retry may yield better results. */
    SWI_RCODE_UNEXPECTED_RESP,  /*!< The function is returning data without detecting a processing 
                                 *  fault, but the data does not match the expected range or format.
                                 *  A retry of the function may yield better results. */
    SWI_RCODE_REQUEST_TIMEOUT,  /*!< The function could not complete within the time allowed by the 
                                *  timeout parameter.  A retry of the function with a longer
                                *  timeout value may yield better results. */
    SWI_RCODE_BUSY,     /*!< An older Sierra API instance is running.  The older Sierra API
                         *  instance will not work concurrently with the API you are
                         *  currently using. */
    SWI_RCODE_SYSTEM_ERROR,     /*!< A Windows operating system error has occurred. You can use the
                                *  Windows GetLastError command to retrieve additional information. */
    SWI_RCODE_API_NOT_OPEN,     /*!< Operation can not be completed because prerequisites have not been
                                 *  completed.  Ensure that the API has been successfully initialized 
                                 *  before calling any API functions. */
    SWI_RCODE_DEVICE_UNAVAIL,   /*!< Device is not loaded onto the system.  Check that the device and
                                *  drivers are installed on the computer correctly. */
    SWI_RCODE_ALREADY_OPEN,     /*!< API is already initialized.  SwiSelectAirServer can be used to
                                *  select an air server. */
    SWI_RCODE_DEVICE_INUSE,     /*!< Device is present but locked for exclusive use by another 
                                *  application. */
    SWI_RCODE_SERVICE_INUSE,    /*!< Device is present but the service you requested is locked for 
                                *  exclusive use by another application. */
    SWI_RCODE_DEVICE_LOCKED,    /*!< Device is unavailable and is locked for exclusive use by
                                *  another application. */
    SWI_RCODE_API_SYS_ERR,      /*!< An internal API system error occurred.  Can be caused if the 
                                 *  system is starved of resources or if an incorrect SDK version 
                                 *  is used. */
    SWI_RCODE_NOT_INIT,     /*!< Not initialized.  Operation can not be completed because 
                             *  prerequisites have not been completed.  Ensure that the correct
                             *  air server has been selected. */
    SWI_RCODE_NOT_COMPATIBLE,   /*!< API components are not compatible with each other.  Make sure the 
                                *  latest SDK components are installed. */
    SWI_RCODE_DEVICE_NOT_SUP,   /*!< The API does not support the specified device type. */
    SWI_RCODE_RESP_PENDING,     /*!< A response to a synchronous request is pending.  No other requests
                                *  can be made until the first request completes or timeout. */
    SWI_RCODE_DEVICE_NOT_SEL,   /*!< Request can not be processed because a device has not been selected.
                                *  Use SwiSelectAirServer to select a device to use. */
    SWI_RCODE_TOO_MANY_TRANSPORT_CLIENTS,   /*!< We have exceeded the maximum number of application clients for 
                                             *  this device's transport layer. This value is currently set as
                                             *  MAX_TRANSPORT_CLIENTS in SwiApiCommon.h. */
    SWI_RCODE_BIND_FAILED,  /*!< This host (PC or mobile device) has failed to bind to a 
                             *  discovered device. Probably because that device is no longer
                             *  present or has already been bound to another host. */

    /* CDMA SDK API Wrapper error codes */
    SWI_RCODE_INVALID_FUNCPTR = 75,     /*!< Invalid function pointer. */
    SWI_RCODE_PWR_CTRL_GET_CTRLPORT_FAILED, /*!< Failed to get control port. */
    SWI_RCODE_PWR_CTRL_GET_DATAPORT_FAILED, /*!< Failed to get data port. */
    SWI_RCODE_PWR_CTRL_GET_VSP_PORTS_FAILED,    /*!< Failed to get virtual serial port. */

    /* CnS return codes */
    SWI_RCODE_CNS_FAILED = 100,     /*!< CNS layer failure */
    SWI_RCODE_CNS_OPERATION_ID_INVALID,     /*!< Invalid CNS operation ID. */
    SWI_RCODE_CNS_OBJECT_ID_INVALID,    /*!< Invalid CNS object ID. */
    SWI_RCODE_CNS_UNEXPECTED_NOTIFICATION,  /*!< Notification received but not asked for. */
    SWI_RCODE_CNS_ILLEGAL_NOTIFICATION, /*!< Notification not supported. */
    SWI_RCODE_CNS_CALLBACK_FAILURE, /*!< Callback function invalid. */

    /* Frame error codes */
    SWI_RCODE_FRAME_QUEUE_FULL   = 150, /*!< Frame queue full. */
    SWI_RCODE_FRAME_QUEUE_EMPTY,    /*!< Frame queue empty. */
    SWI_RCODE_PUT_TIMEOUT,  /*!< Put into queue failed due to timeout. */
    SWI_RCODE_GET_TIMEOUT,  /*!< Get from queue failed due to timeout.  This may be caused if a
                             *  response is not received from the modem firmware within the
                             *  specified timeout value.  Using a longer timeout value may
                             *  yield better results. */
    SWI_RCODE_BAD_FRAME,    /*!< A defective frame was passed to a layer. */
    SWI_RCODE_CANCEL,   /*!< Request canceled.  This may be caused by the following:
                         *  1) Device removal
                         *  2) Device reset
                         *  3) Device powering down
                         *  4) API terminating */

    /* CWE return codes */
    SWI_RCODE_CWE_FAILED = 200, /*!< CWE layer failure. */
    SWI_RCODE_CWE_MSG_ID_INVALID,   /*!< CWE invalid message ID. */
    SWI_RCODE_CWE_UNEXPECTED_NOTIFICATION,  /*!< Notification received but not asked for. */
    SWI_RCODE_CWE_ILLEGAL_NOTIFICATION, /*!< Spurious notification not handled by CWE. */
    SWI_RCODE_CWE_CALLBACK_FAILURE, /*!< Callback function invalid. */

    /* OID return codes */
    SWI_RCODE_OID_BUFFER_ID_INVALID = 225,  /*!< OID buffer ID is invalid. */
    SWI_RCODE_OID_FAILED,   /*!< OID layer failure. */
    SWI_RCODE_OID_UNEXPECTED_NOTIFICATION,  /*!< Notification received but not asked for. */
    SWI_RCODE_OID_ILLEGAL_NOTIFICATION, /*!< Spurious notification not handled by OID. */
    SWI_RCODE_OID_CALLBACK_FAILURE, /*!< Callback function invalid. */

    
    /* MMM return codes */

    /* PACKET return codes */
    SWI_RCODE_PACKET_FAILED            = 250,
    SWI_RCODE_PACKET_MSG_ID_INVALID,
    SWI_RCODE_PACKET_UNEXPECTED_NOTIFICATION,
    SWI_RCODE_PACKET_ILLEGAL_NOTIFICATION,
    SWI_RCODE_PACKET_CALLBACK_FAILURE,

    /* MUX return codes */
    SWI_RCODE_MUX_CON_FAILED = 275,     /*!< API service does not exist or has refused the connection.  Ensure
                                         *  that the API service is installed and running correctly.  The API
                                         *  service may refuse a connection if it is terminating or if the
                                         *  maximum number of client applications have been reached. */
    SWI_RCODE_MUX_FAILED,   /*!< API service failed to initialize.  Make sure all API components are 
                             *  installed correctly. */
    
    /* STL return codes */
    SWI_RCODE_STL_FAILED = 300,
    SWI_RCODE_STL_MSG_ID_INVALID,
    SWI_RCODE_STL_UNEXPECTED_NOTIFICATION,
    SWI_RCODE_STL_ILLEGAL_NOTIFICATION,
    SWI_RCODE_STL_CALLBACK_FAILURE,

    /* Link layer return codes */
    SWI_RCODE_LINK_FAILED = 350,    /*!< Link layer failure. */
    SWI_RCODE_LINK_MSG_ID_INVALID,  /*!< Invalid link message ID. */
    SWI_RCODE_LINK_NO_PENDING_DATA, /*!< Link layer response to indicate no pending data. */
    SWI_RCODE_PORT_OPEN_FAIL,       /*!< Failed to open COMM port. */
    SWI_RCODE_PORT_INITIALIZE_FAIL, /*!< Failed to initialize COMM port. */

    SWI_RCODE_DATA_INIT_FAILED = 400,   /*!< Failure to initialize the data layer. */
    SWI_RCODE_DATA_SEND_FAILED, /*!< Data layer send failed. */

    /* HIM return codes */
    SWI_RCODE_HIM_FAILED            = 450,
    SWI_RCODE_HIM_MSG_ID_INVALID,
    SWI_RCODE_HIM_UNEXPECTED_NOTIFICATION,
    SWI_RCODE_HIM_ILLEGAL_NOTIFICATION,
    SWI_RCODE_HIM_CALLBACK_FAILURE,

    SWI_RCODE_PROFILE_FAILED = 500,     /*!< Profile layer failure. */
    SWI_RCODE_PROFILE_MSG_ID_INVALID,   /*!< Profile invalid message ID. */
    SWI_RCODE_PROFILE_OP_CODE_INVALID,  /*!< Profile invalid operation code. */

    SWI_RCODE_REQUEST_REJECTED = 600,   /*!< The modem rejected the operation.  Call SwiGetLastError to retrieve
                                        *  the error string returned by the modem firmware. */

    /* CAIT return codes */
    SWI_RCODE_CAIT_INIT_FAILED = 700,   /*!< Failure to initialize CAIT layer. */

    /* Discovery return codes */
    SWI_RCODE_DISCOVERY_INVALID_MESSAGE = 800,  /*!< A received discovery packet was defective. If this happens 
                                                 *  frequently, it is likely that a rogue protocol on the LAN
                                                 *  is issuing packets of ethernet type 0x88b7 and OUI subtype which
                                                 *  is Sierra Wireless specific. That protocol would be in violation
                                                 *  of IEEE specification. */

    /* DrvInterface return codes. SW MUX Drivers only */
    SWI_RCODE_MUXINT_LOAD_FAILED = 900,
    SWI_RCODE_MUXINT_INCORRECT_VER,
    SWI_RCODE_MUXINT_INTERNAL_FAIL,
    SWI_RCODE_MUXINT_ACQLOCK_FAIL,
    SWI_RCODE_MUXINT_MUXMODE_FAIL,
    SWI_RCODE_MUXINT_DEVICE_NOT_FOUND,
    SWI_RCODE_MUXINT_INVALID_OPENPARAMS,
    SWI_RCODE_MUXINT_QUERYTYPE_INVALID,
    SWI_RCODE_MUXINT_QUERYDLCI_NOTSUP,
    SWI_RCODE_MUXINT_PORT_ALREADY_ACQUIRED,
    SWI_RCODE_MULTI_DEVICE,
    SWI_RCODE_MUXINT_NDIS_NOTSUPPORTED,
    SWI_RCODE_MUXINT_CALL_INPROGRESS,
    SWI_RCODE_MUXINT_NIC_DEVICE_DISABLED,

    /* LBS PA response code - PAErrType */
    SWI_RCODE_LBS_PA_NO_ERROR = 1000,
    SWI_RCODE_LBS_PA_INTERNAL_ERROR,
    SWI_RCODE_LBS_PA_INVALID_CLIENT_ID,
    SWI_RCODE_LBS_PA_ERROR_IN_PARAM_TO_SET,
    SWI_RCODE_LBS_PA_ERROR_IN_LOCK_TYPE,
    SWI_RCODE_LBS_PA_PHONE_OFFLINE,
    SWI_RCODE_LBS_PA_SET_COMMAND_ALREADY_ACTIVE,
    SWI_RCODE_LBS_PA_INCORRECT_APP_INFO,

    /* LBS PD response code - PDErrType */
    SWI_RCODE_LBS_PD_NO_ERROR = 1050,
    SWI_RCODE_LBS_PD_INTERNAL_MODEM_ERROR,
    SWI_RCODE_LBS_PD_BAD_SERVICE_TYPE,
    SWI_RCODE_LBS_PD_BAD_SESSION_TYPE,
    SWI_RCODE_LBS_PD_INVALID_PRIVACY,
    SWI_RCODE_LBS_PD_INVALID_DATA_DOWNLOAD,
    SWI_RCODE_LBS_PD_INVALID_NETWORK_ACCESS,
    SWI_RCODE_LBS_PD_INVALID_OPERATION_MODE,
    SWI_RCODE_LBS_PD_INVALID_NUMBER_OF_FIXES,
    SWI_RCODE_LBS_PD_INVALID_SERVER_INFO,
    SWI_RCODE_LBS_PD_INVALID_TIMEOUT,
    SWI_RCODE_LBS_PD_INVALID_QOS_PARAMETER,
    SWI_RCODE_LBS_PD_NO_SESSION_ACTIVE,
    SWI_RCODE_LBS_PD_SESSION_ALREADY_ACTIVE,
    SWI_RCODE_LBS_PD_SESSION_BUSY,
    SWI_RCODE_LBS_PD_PHONE_OFFLINE,
    SWI_RCODE_LBS_PD_CDMA_LOCK_ERROR,
    SWI_RCODE_LBS_PD_GPS_LOCK_ERROR,
    SWI_RCODE_LBS_PD_INVALID_STATE,
    SWI_RCODE_LBS_PD_CONNECTION_FAILURE,
    SWI_RCODE_LBS_PD_NO_BUFFERS_AVAILABLE,
    SWI_RCODE_LBS_PD_SEARCHER_ERROR,
    SWI_RCODE_LBS_PD_CANNOT_REPORT_NOW,
    SWI_RCODE_LBS_PD_ERROR_RESOURCE_CONTENTION,
    SWI_RCODE_LBS_PD_MODE_NOT_SUPPORTED,
    SWI_RCODE_LBS_PD_AUTHENTICATION_FAIL,
    SWI_RCODE_LBS_PD_OTHER_ERROR,
    SWI_RCODE_LBS_PD_FIX_RATE_TOO_LARGE,

    /* OMADM Error */
    SWI_RCODE_OMADM_ERROR     = 1100

} SWI_RCODE;

#endif /* __SWI_RCODES_H */

/*
 * $Log: SwiRcodes.h,v $
 */
