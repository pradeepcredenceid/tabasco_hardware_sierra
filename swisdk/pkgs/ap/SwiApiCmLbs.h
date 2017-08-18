/**
 *    \if CvsId 
 *    $Id: SwiApiCmLbs.h,v 1.9 2010/06/10 00:15:57 epasheva Exp $
 *    \endif
 *    \ingroup cmlbs
 *    \file  SwiApiCmLbs.h
 * 
 *    \brief  Header file for for GSM APIs related to the 
 *            Location Based Services
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __SWIAPI_CM_LBS
#define __SWIAPI_CM_LBS

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"


#define MAX_NUM_LBS_SV         12 /*!< Maximum number of sattelites */
#define LENGTH_LbsReqId        33 /*!< Maximum requestor ID length */
#define LENGTH_LbsClientName   33 /*!< Maximum client name length */
#define LENGTH_TimeStamp       16 /*!< Time stamp string length */

/**
 *
 * Enumeration of PD end status values.
 *
 * \sa SWI_NOTIFY_LbsPdEnd
 * \sa SWI_STRUCT_LbsPdEnd
 * \sa SWI_STRUCT_LbsPdDloadEnd
 *
 */
enum SWI_TYPE_LbsPdEndStatus
{
    SWI_LBSPDENDSTATUS_Offline,
    SWI_LBSPDENDSTATUS_NoService,
    SWI_LBSPDENDSTATUS_NoConnection,
    SWI_LBSPDENDSTATUS_NoData,
    SWI_LBSPDENDSTATUS_SessionBusy,
    SWI_LBSPDENDSTATUS_CdmaLock,
    SWI_LBSPDENDSTATUS_GpsLock,
    SWI_LBSPDENDSTATUS_ConnectionFailed,
    SWI_LBSPDENDSTATUS_ErrorState,
    SWI_LBSPDENDSTATUS_ClientEnded,
    SWI_LBSPDENDSTATUS_UiEnded,
    SWI_LBSPDENDSTATUS_NetworkEnded,
    SWI_LBSPDENDSTATUS_Timeout,
    SWI_LBSPDENDSTATUS_PrivacyLevel,
    SWI_LBSPDENDSTATUS_NetworkAccessError,
    SWI_LBSPDENDSTATUS_FixError,
    SWI_LBSPDENDSTATUS_PdeRejected,
    SWI_LBSPDENDSTATUS_TrafficChannelExited,
    SWI_LBSPDENDSTATUS_E911,
    SWI_LBSPDENDSTATUS_ServerError,
    SWI_LBSPDENDSTATUS_StaleBsInformation
};

/**
 *
 * Enumeration of PD error types
 *
 * \sa SWI_STRUCT_LbsPdGetStatus
 *
 */
enum SWI_TYPE_LbsPdErr
{
    SWI_LBSPDERR_NoError,
    SWI_LBSPDERR_InternalModemError,
    SWI_LBSPDERR_BadServiceType,
    SWI_LBSPDERR_BadSessionType,
    SWI_LBSPDERR_InvalidPrivacy,
    SWI_LBSPDERR_InvalidDataDownload,
    SWI_LBSPDERR_InvalidNetworkAccess,
    SWI_LBSPDERR_InvalidOperationMode,
    SWI_LBSPDERR_InvalidNumberOfFixes,
    SWI_LBSPDERR_InvalidServerInfo,
    SWI_LBSPDERR_InvalidTimeout,
    SWI_LBSPDERR_InvalidQosParameter,
    SWI_LBSPDERR_NoSessionActive,
    SWI_LBSPDERR_SessionAlreadyActive,
    SWI_LBSPDERR_SessionBusy,
    SWI_LBSPDERR_PhoneIsOffline,
    SWI_LBSPDERR_CDMALockError,
    SWI_LBSPDERR_GPSLockError,
    SWI_LBSPDERR_InvalidState,
    SWI_LBSPDERR_ConnectionFailure,
    SWI_LBSPDERR_NoBuffersAvailable,
    SWI_LBSPDERR_SearcherError,
    SWI_LBSPDERR_CannotReportNow,
    SWI_LBSPDERR_ModeNotSupported,
    SWI_LBSPDERR_OtherError,
    SWI_LBSPDERR_FixRateForTrackingTooLarge
};

/**
 *
 * PD service mask values
 *
 */
enum SWI_MASK_LbsPdService
{
    MASK_LbsPdService_Position  = 0x01,
    MASK_LbsPdService_Velocity  = 0x02,
    MASK_LbsPdService_Height    = 0x04
};

/**
 *
 * Enumeration of PD download options
 *
 * \sa SWI_STRUCT_LbsPdDownload
 *
 */
enum SWI_TYPE_LbsPdDloadOption
{
    SWI_LBSPDDLOADOPTION_Periodic = 0x01,
    SWI_LBSPDDLOADOPTION_Single
};

/**
 *
 * Enumeration of session types
 *
 * \sa SWI_STRUCT_LbsPdEndSession
 *
 */
enum SWI_TYPE_LbsSession
{
    SWI_LBSSESSION_LocationFix,
    SWI_LBSSESSION_Download
};

/**
 *
 * LBS location notification status bit masks
 *
 */
enum SWI_MASK_LbsLocNotifyStatus
{
    MASK_LbsPdLocNotifyStatus_Begin         = 0x0001,
    MASK_LbsPdLocNotifyStatus_Data          = 0x0002,
    MASK_LbsPdLocNotifyStatus_End           = 0x0004,
    MASK_LbsPdLocNotifyStatus_Done          = 0x0008,
    MASK_LbsPdLocNotifyStatus_UpdateFailure = 0x0010
};

/**
 *
 * LBS download notification status bit masks
 *
 */
enum SWI_MASK_LbsDloadNotifyStatus
{
    MASK_LbsPdDloadNotifyStatus_Begin       = 0x0001,
    MASK_LbsPdDloadNotifyStatus_Data        = 0x0002,
    MASK_LbsPdDloadNotifyStatus_End         = 0x0004,
    MASK_LbsPdDloadNotifyStatus_Done        = 0x0008
};

/**
 *
 * LBS parameter notificatoin status bit masks
 *
 */
enum SWI_MASK_LbsParamNotifyStatus
{
    MASK_LbsPaParamNotifyStatus_Address     = 0x0001,
    MASK_LbsPaParamNotifyStatus_GpsLock     = 0x0002,
    MASK_LbsPaParamNotifyStatus_PtlmMode    = 0x0004,
    MASK_LbsPaParamNotifyStatus_PortId      = 0x0008,
    MASK_LbsPaParamNotifyStatus_Privacy     = 0x0010,
    MASK_LbsPaParamNotifyStatus_NetAccess   = 0x0020
};

/**
 *
 * An enumeration of LBS notification state
 *
 * \sa SWI_STRUCT_LbsLocNotifyStatus
 * \sa SWI_STRUCT_LbsDloadNotifyStatus
 * \sa SWI_STRUCT_LbsParamNotifyStatus
 *
 */
enum SWI_TYPE_LbsNotifyState
{
    SWI_LBSNOTIFYSTATE_Off,
    SWI_LBSNOTIFYSTATE_On
};

/**
 *
 * Used to enable/disable or check the current notification state of the
 * following group of notifications:
 *  - SWI_NOTIFY_LbsPdData
 *  - SWI_NOTIFY_LbsPdDone
 *  - SWI_NOTIFY_LbsPdEnd
 *  - SWI_NOTIFY_LbsPdBegin
 *  - SWI_NOTIFY_LbsPdUpdateFailure
 *  - SWI_NOTIFY_LbsSatInfo
 *
 * \sa SWI_TYPE_LbsNotifyState
 * \sa SWI_NOTIFY_LbsPdData
 * \sa SWI_NOTIFY_LbsPdDone
 * \sa SWI_NOTIFY_LbsPdEnd
 * \sa SWI_NOTIFY_LbsPdBegin
 * \sa SWI_NOTIFY_LbsPdUpdateFailure
 * \sa SWI_NOTIFY_LbsSatInfo
 * \sa SwiGetLbsLocNotifyStatus
 * \sa SwiSetLbsLocNotifyStatus
 *
 */
typedef struct SWI_STRUCT_LbsLocNotifyStatus
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;    

    /*! An enumeration of type SWI_TYPE_LbsNotifyState which reflects the
        state of the notification */  
    enum SWI_TYPE_LbsNotifyState eNotifyFlag;
}SWI_STRUCT_LbsLocNotifyStatus;

/**
 *
 * Used to enable/disable or check the current notification state of the 
 * following group of notifications:
 *        - SWI_NOTIFY_LbsPdDloadBegin
 *        - SWI_NOTIFY_LbsPdDloadData
 *        - SWI_NOTIFY_LbsPdDloadDone
 *        - SWI_NOTIFY_LbsPdDloadEnd
 *
 * \sa SWI_TYPE_LbsNotifyState
 * \sa SWI_NOTIFY_LbsPdDloadBegin
 * \sa SWI_NOTIFY_LbsPdDloadData
 * \sa SWI_NOTIFY_LbsPdDloadDone
 * \sa SWI_NOTIFY_LbsPdDloadEnd
 * \sa SwiGetLbsDloadNotifyStatus
 * \sa SwiSetLbsDloadNotifyStatus
 *
 */
typedef struct SWI_STRUCT_LbsDloadNotifyStatus
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;   
    
    /*! An enumeration of type SWI_TYPE_LbsNotifyState which reflects the
        state of the notification */
    enum SWI_TYPE_LbsNotifyState eNotifyFlag;
}SWI_STRUCT_LbsDloadNotifyStatus;

/**
 *
 * Used to enable/disable or check the current notification state of the 
 * following group of notifications:
 *        - SWI_NOTIFY_LbsPaIpAddr
 *        - SWI_NOTIFY_LbsPaGpsLock
 *        - SWI_NOTIFY_LbsPaPtlmMode
 *        - SWI_NOTIFY_LbsPaPortId
 *        - SWI_NOTIFY_LbsPaPrivacy
 *        - SWI_NOTIFY_LbsPaNetAccess
 *
 * \sa SWI_TYPE_LbsNotifyState
 * \sa SWI_NOTIFY_LbsPaIpAddr
 * \sa SWI_NOTIFY_LbsPaGpsLock
 * \sa SWI_NOTIFY_LbsPaPtlmMode
 * \sa SWI_NOTIFY_LbsPaPortId
 * \sa SWI_NOTIFY_LbsPaPrivacy
 * \sa SWI_NOTIFY_LbsPaNetAccess
 * \sa SwiGetLbsParamNotifyStatus
 * \sa SwiSetLbsParamNotifyStatus
 *
 */
typedef struct SWI_STRUCT_LbsParamNotifyStatus
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;     
    
    /*! An enumeration of type SWI_TYPE_LbsNotifyState which reflects the
        state of the notification */
    enum SWI_TYPE_LbsNotifyState eNotifyFlag;
}SWI_STRUCT_LbsParamNotifyStatus;

/**
 *
 * Used to specify the download option and duration.
 * 
 * The host can use the download option to indicate whether the data download
 * should occur just once or periodically, in an effort to keep the data
 * "warm".  If the application that will be requesting this data will be
 * requiring the use of the data over a period of time, the application should
 * indicate that the download should be periodic.
 *
 * \sa SWI_TYPE_LbsPdDloadOption
 * \sa SwiSetLbsPdDownload
 *
 */
typedef struct SWI_STRUCT_LbsPdDownload
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;    
            
    /*! An enumeration of type SWI_TYPE_LbsPdDloadOption which specifies a
        single or periodic download */
    enum SWI_TYPE_LbsPdDloadOption eOption;
            
    /*! Session duration in minutes. Used only with periodic download option. */
    swi_uint16 nDuration;
}SWI_STRUCT_LbsPdDownload;

/**
 *
 * Enumeration of location fix types
 *
 * \sa SWI_STRUCT_LbsPdGetPos
 * \sa SWI_STRUCT_LbsPdTrack
 *
 */
enum SWI_TYPE_LbsFix
{
    /*! Mobile does everything involved in computing its position.  
        No PDE required. */
    SWI_LBSFIX_Autonomous = 1,
    
    /*! Mobile gets assistance from the PDE but the mobile computes 
        position. */
    SWI_LBSFIX_MsBased,
    
    /*! Mobile provides information to the PDE so that the PDE can compute
        the mobiles position. */
    SWI_LBSFIX_MsAssisted,
    
    /*! MS Based or MS Assisted depending on Performance and Accuracy 
        settings. */
    SWI_LBSFIX_OptimizedSpeed,
    
    /*! MS-Assisted preferred, MS-Based if MS-Assisted fails */
    SWI_LBSFIX_OptimizedAccuracy,
    
    /*! MS-Based preferred, MS-Assisted if MS-Based fails */
    SWI_LBSFIX_OptimizedData
};

/**
 *
 * Structure containing location fix data
 *
 * \sa SWI_TYPE_LbsFix
 * \sa SwiSetLbsPdGetPos
 *
 */
typedef struct SWI_STRUCT_LbsPdGetPos
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! An enumeration of type SWI_TYPE_LbsFix which indicates the location 
        fix type */
    enum SWI_TYPE_LbsFix eFixType; 
    
    /*! Time allowed for satellite acquisition in seconds */
    swi_uint16 nPerformance;
    
    /*! Accuracy limit preferred in meters */
    swi_uint32 nAccuracy;
}SWI_STRUCT_LbsPdGetPos;

/**
 *
 * Enumeration of LBS PD status
 *
 * \sa SWI_STRUCT_LbsPdGetStatus
 *
 */
enum SWI_TYPE_LbsPdStatus
{
    SWI_LBSPDSTATUS_None    = 0, 
    SWI_LBSPDSTATUS_Active    = 1, 
    SWI_LBSPDSTATUS_Success = 2, 
    SWI_LBSPDSTATUS_Error   = 3
};

/**
 *
 * Structure containing LBS status information.
 *
 * \sa SWI_TYPE_LbsPdStatus
 * \sa SWI_TYPE_LbsPdErr
 * \sa SwiGetLbsPdStatus
 *
 */
typedef struct SWI_STRUCT_LbsPdGetStatus
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! A NULL terminated string containing the last single fix time stamp */
    swi_char szLastFixTimeStamp[LENGTH_TimeStamp];
    
    /*! An enumeration of type SWI_TYPE_LbsPdStatus containing the last 
        single fix status */
    enum SWI_TYPE_LbsPdStatus eLastFixStatus;  
    
    /*! An enumeration of type SWI_TYPE_LbsPdErr containing the last single
        fix error */               
    enum SWI_TYPE_LbsPdErr eLastFixError;
    
    /*! A NULL terminated string containing the last download time stamp */
    swi_char szLastDownloadTimeStamp[LENGTH_TimeStamp];
    
    /*! An enumeration of type SWI_TYPE_LbsPdStatus containing the last
        download status */
    enum SWI_TYPE_LbsPdStatus eLastDownloadStatus;
    
    /*! An enumeration of type SWI_TYPE_LbsPdErr containing the last 
        download error */
    enum SWI_TYPE_LbsPdErr eLastDownloadError;
    
    /*! A NULL terminated string containing the last tracking time stamp */
    swi_char szLastFixSessionTimeStamp[LENGTH_TimeStamp];  
    
    /*! An enumeration of type SWI_TYPE_LbsPdStatus containing the last
        tracking fix status */
    enum SWI_TYPE_LbsPdStatus eLastFixSessionStatus; 
    
    /*! An enumeration of type SWI_TYPE_LbsPdErr containing the last fix 
        session error */       
    enum SWI_TYPE_LbsPdErr eLastFixSessionError;
    
    /*! A NULL terminated string containing the last download session time
        stamp */        
    swi_char szLastDownloadSessionTimeStamp[LENGTH_TimeStamp];
    
    /*! An enumeration of type SWI_TYPE_LbsPdStatus containing the last
        download session status */
    enum SWI_TYPE_LbsPdStatus eLastDownloadSessionStatus;
    
    /*! An enumeration of type SWI_TYPE_LbsPdErr containing the last download 
        session error */
    enum SWI_TYPE_LbsPdErr eLastDownloadSessionError;
}SWI_STRUCT_LbsPdGetStatus;

/**
 *
 * Structure containing information related to starting a tracking session.
 *
 * \sa SWI_TYPE_LbsFix
 * \sa SwiSetLbsPdTrack
 *
 */
typedef struct SWI_STRUCT_LbsPdTrack
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! An enumeration of type SWI_TYPE_LbsFix which contains the location
        fix type */
    enum SWI_TYPE_LbsFix eFixType;   
    
    /*! Time allowed for satellite acquisition in seconds */
    swi_uint16 nPerformance;     
    
    /*! Accuracy limit preferred in meters */
    swi_uint32 nAccuracy;    
    
    /*! Number of fixes to obtain between 1 and 999.  use 1000 for continuous
        mode */
    swi_uint16    nFixCount;        
     
    /*! Frequency of fixes in seconds */
    swi_uint32 nFixRate;
}SWI_STRUCT_LbsPdTrack;

/**
 *
 * Structure containing information related to ending a tracking session
 *
 * \sa SWI_TYPE_LbsSession
 * \sa SwiSetLbsPdEndSession
 *
 */
typedef struct SWI_STRUCT_LbsPdEndSession
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;    
    
    /*! An enumeration of type SWI_TYPE_LbsSession which contains the session 
        type to end */
    enum SWI_TYPE_LbsSession eSessionType;
}SWI_STRUCT_LbsPdEndSession;

/**
 *
 * Enumeration of GPS lock values
 *
 * \sa SWI_STRUCT_LbsPaGetParam
 * \sa SWI_STRUCT_LbsPaGpsLock
 *
 */
enum SWI_TYPE_LbsPaGpsLock
{
    SWI_LBSPAGPSLOCK_NoLockActive,
    SWI_LBSPAGPSLOCK_MiSessionsLocked,
    SWI_LBSPAGPSLOCK_MtSessionsLocked,
    SWI_LBSPAGPSLOCK_AllSessionsLocked
};

/**
 *
 * Enumeration of PD transport mechanisms
 *
 * \sa SWI_STRUCT_LbsPaGetParam
 * \sa SWI_STRUCT_LbsPaPtlmMode
 *
 */
enum SWI_TYPE_LbsPaTransMech
{
    SWI_LBSPATRANSMECH_TcpIp,
    SWI_LBSPATRANSMECH_DataBurst
};

/**
 *
 * Enumeration of privacy values.
 *
 * \sa SWI_STRUCT_LbsPaGetParam
 * \sa SWI_STRUCT_LbsPaPrivacy
 *
 */
enum SWI_TYPE_LbsPaPrivacy
{
    SWI_LBSPAPRIVACY_High,
    SWI_LBSPAPRIVACY_Medium,
    SWI_LBSPAPRIVACY_Low
};

/**
 *
 * Enumeration of network access values.
 *
 * \sa SWI_STRUCT_LbsPaGetParam
 * \sa SWI_STRUCT_LbsPaNetAccess
 *
 */
enum SWI_TYPE_LbsPaNetAccess
{
    SWI_LBSPANETACCESS_NetworkOnly,
    SWI_LBSPANETACCESS_NoNetworkAccess,
    SWI_LBSPANETACCESS_NetworkWithDemodulation,
    SWI_LBSPANETACCESS_NetworkWithoutDemodulation
};

/**
 *
 * Structure containing the modem's default parameter settings.
 *
 * Entire set of position determination parameters is returned to the host
 * (IP address, port ID, GPS lock, transport mechanism, privacy, network
 * access).
 *
 * \sa SWI_TYPE_LbsPaGpsLock
 * \sa SWI_TYPE_LbsPaTransMech
 * \sa SWI_TYPE_LbsPaPrivacy
 * \sa SWI_TYPE_LbsPaNetAccess
 * \sa SwiGetLbsPaParam
 *
 */
typedef struct SWI_STRUCT_LbsPaGetParam
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! TCP/IP address of PDE.  Only used when eTransportMech is set to 
        TCP/IP. */
    swi_uint32 nIpAddress;
    
    /*! Port ID of the PDE.  Only used when eTransportMech is set to TCP/IP. */
    swi_uint32 nPortId;
    
    /*! GPS lock state */
    enum SWI_TYPE_LbsPaGpsLock eGpsLock;
    
    /*! Transport mechanism (TCP/IP or DBM) */
    enum SWI_TYPE_LbsPaTransMech eTransportMech;    
    
    /*! Privacy state (low, medium, or high) */
    enum SWI_TYPE_LbsPaPrivacy ePrivacy;    
    
    /*! Network access state */    
    enum SWI_TYPE_LbsPaNetAccess eNetworkAccess;
}SWI_STRUCT_LbsPaGetParam;

/**
 *
 * Enumeration of location fix dimensions.
 *
 * \sa SWI_STRUCT_LbsPdData
 *
 */
enum SWI_TYPE_LbsFixDim
{
    SWI_LBSFIXDIM_2D,
    SWI_LBSFIXDIM_3D
};

/**
 *
 * Enumeration representing the Standard Deviation for Position Uncertainty
 *
 * From IS801-1
 *   Values are 0 - 31 and map to the list below it (in Meters)
 *    -  0,  1,2,  3,4,5,6,7,8, 9,10,11,12,13,14,15, 16, 17, 18, 19, 20, 21,  22,  23,  24,  25,  26,  27,  28,   29,   30,31,32
 *    - .5,.75,1,1.5,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,768,1024,1536,2048,3072,4096,6144,8192,12288,12288, 0,0
 *
// Doxygen formatting of the same information presented above
 * <table border="1">
 * <tr>
 *        <td>0</td><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td>
 *        <td>7</td><td>8</td><td>9</td><td>10</td><td>11</td><td>12</td>
 *        <td>13</td><td>14</td><td>15</td><td>16</td><td>17</td><td>18</td>
 *        <td>19</td><td>20</td><td>21</td><td>22</td><td>23</td><td>24</td>
 *        <td>25</td><td>26</td><td>27</td><td>28</td><td>29</td><td>30</td>
 *        <td>31</td><td>32</td>
 * </tr>
 * <tr>
 *        <td>.5</td><td>.75</td><td>1</td><td>1.5</td><td>2</td><td>3</td>
 *        <td>4</td><td>6</td><td>8</td><td>12</td><td>16</td><td>24</td>
 *        <td>32</td><td>48</td><td>64</td><td>96</td><td>128</td><td>192</td>
 *        <td>256</td><td>384</td><td>512</td><td>768</td><td>1024</td>
 *        <td>1536</td><td>2048</td><td>3072</td><td>4096</td><td>6144</td>
 *        <td>8192</td><td>12288</td><td>12288</td><td>0</td><td>0</td>
 * </tr>
 * </table>
 * \sa SWI_STRUCT_LbsPdData
 *
 */
enum SWI_TYPE_SDPU
{
    SWI_SDPU_0Dot5,
    SWI_SDPU_0Dot75,
    SWI_SDPU_1,
    SWI_SDPU_1Dot5,
    SWI_SDPU_2,
    SWI_SDPU_3,
    SWI_SDPU_4,
    SWI_SDPU_6,
    SWI_SDPU_8,
    SWI_SDPU_12,
    SWI_SDPU_16,
    SWI_SDPU_24,
    SWI_SDPU_32,
    SWI_SDPU_48,
    SWI_SDPU_64,
    SWI_SDPU_96,
    SWI_SDPU_128,
    SWI_SDPU_192,
    SWI_SDPU_256,
    SWI_SDPU_384,
    SWI_SDPU_512,
    SWI_SDPU_768,
    SWI_SDPU_1024,
    SWI_SDPU_1536,
    SWI_SDPU_2048,
    SWI_SDPU_3072,
    SWI_SDPU_4096,
    SWI_SDPU_6144,
    SWI_SDPU_8192,
    SWI_SDPU_12288,
    SWI_SDPU_GreaterThan12288,
    SWI_SDPU_NotComputable
};

/**
 *
 * Structure containing position fix information.
 * 
 * This object is sent to notify the registered host client of the result
 * of the position fix.  If the host performs a get on this object when
 * there is no position fix to report the modem will respond with a
 * generic error message.
 *
 * \sa SWI_TYPE_SDPU
 * \sa SWI_TYPE_LbsFixDim
 * \sa SWI_TYPE_SDPU
 * \sa SWI_NOTIFY_LbsPdData
 * \sa SwiGetLbsPdData
 *
 */
typedef struct SWI_STRUCT_LbsPdData
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! 2's complement in units of 180/2^25 degrees between +90 and -90 
        degrees */
    swi_int32 nLatitude;    
    
    /*! 2's complement in units of 180/2^25 degrees between +90 and -90 
        degrees */
    swi_int32 nLongitude;   
    
    /*! To convert time: CTime t(nValue+315964800L).  315964800L is the 
        number of seconds between January 6, 1980 and January 1, 1970 */
    swi_uint32 nTimeStamp;   
    
    /*! In units of 5.625 degrees from 0 to 84.375 with 0 being true North 
        increasing to the east. */
    swi_uint16 nLocUncAngle;
        
    /*! Standard deviation of axis along angle specified for position
        uncertainty */
    enum SWI_TYPE_SDPU eLocUncA;  
    
    /*! Standard deviation of axis perpendicular to angle specified 
        for position uncertainty */
    enum SWI_TYPE_SDPU eLocUncP;     
    
    /*! 2-D or 3-D fix */
    enum SWI_TYPE_LbsFixDim eFix;    
    
    /*! 0 = Height info not available, 1 = Height info included */
    swi_uint16 nHeightIncl;    
    
    /*! Valid if iHeightIncl = 1.  Value is in meters */
    swi_int16 nHeight;    
    
    /*! Valid if iHeightIncl = 1, Standard Deviation of Vertical error     */
    enum SWI_TYPE_SDPU eLocUncVe;        
    
    /*! 0 = Velocity not included, 1 = Velocity included */
    swi_uint16 nVelocityIncl;
    
    /*! Valid if iVelocityInc = 1.  Formatted in units of 360/2^10 degrees. */
    swi_uint16 nHeading;    
    
    /*! Valid if iVelocityInc = 1. units of .25m/s in range 0 to 127.75m/s. */
    swi_uint16 nVelHoriz;    
    
    /*! Valid if iVelocityInc = 1 and Fix type is 1, 2's complement value of
        Vertical Velocity in units of .5m/s in range of -64m/s to 63.6m/s */
    swi_int16 nVelVert;
    
    /*! Formatted in integer steps of 0.25 meters */
    swi_uint16 nUncertaintyA;    
    
    /*! Formatted in integer steps of 0.25 meters */
    swi_uint16 nUncertaintyP;    
    
    /*! If Height Included =1, this field is valid. Formatted in integer steps
        of 0.25 meters */
    swi_uint16 nUncertaintyV;    
    
    /*! Formatted in integer steps of 0.1 meters */
    swi_uint16 nHEPE;            
    
    /*! Number of Satellites used in Position Fix (Max 12) */
    swi_uint8 nSatelliteNum;    
}SWI_STRUCT_LbsPdData;

/**
 *
 * Skeleton structure returned when a position determination fix session
 *    has completed.
 *
 * \sa SWI_NOTIFY_LbsPdDone
 *
 */
typedef struct SWI_STRUCT_LbsPdDone
{
    swi_uint32 sizeStruct;    /*!< The size, in bytes, of this structure. */ 
}SWI_STRUCT_LbsPdDone;

/**
 *
 * Structure returned when a position determination fix session encountered
 * an error.
 *
 * \sa SWI_TYPE_LbsPdEndStatus
 * \sa SWI_NOTIFY_LbsPdEnd
 * \sa SwiSetLbsPdEndSession
 *
 */
typedef struct SWI_STRUCT_LbsPdEnd
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! An enumeration of type SWI_TYPE_LbsPdEndStatus which specifies
        the PD end error status. */
    enum SWI_TYPE_LbsPdEndStatus eEndStatus;
}SWI_STRUCT_LbsPdEnd;

/**
 *
 * Skeleton structure returned when a position determination fix session
 * has begun.
 *
 * \sa SWI_NOTIFY_LbsPdBegin
 *
 */
typedef struct SWI_STRUCT_LbsPdBegin
{
    swi_uint32 sizeStruct; /*!< The size, in bytes, of this structure. */
}SWI_STRUCT_LbsPdBegin;

/**
 *
 * Skeleton structure returned when a position determination fix session
 * has failed
 *
 * \sa SWI_NOTIFY_LbsPdUpdateFailure
 *
 */
typedef struct SWI_STRUCT_LbsPdUpdateFailure
{
    swi_uint32 sizeStruct; /*!< The size, in bytes, of this structure. */
}SWI_STRUCT_LbsPdUpdateFailure;

/**
 *
 * Skeleton structure returned when a data download session has begun.
 *
 * \sa SWI_NOTIFY_LbsPdDloadBegin
 *
 */
typedef struct SWI_STRUCT_LbsPdDloadBegin
{
    swi_uint32 sizeStruct; /*!< The size, in bytes, of this structure. */
}SWI_STRUCT_LbsPdDloadBegin;

/**
 *
 * Enumeration of download data types
 *
 * \sa SWI_STRUCT_LbsPdDloadData
 *
 */
enum SWI_TYPE_LbsDloadData
{
    SWI_LBSDLOADDATA_Location,
    SWI_LBSDLOADDATA_AA,
    SWI_LBSDLOADDATA_SA,
    SWI_LBSDLOADDATA_Epemeris,
    SWI_LBSDLOADDATA_Almanac
};

/**
 *
 * Enumeration of download source types
 *
 * \sa SWI_STRUCT_LbsPdDloadData
 *
 */
enum SWI_TYPE_LbsDloadSource
{
    SWI_LBSDLOADSOURCE_Mobile,
    SWI_LBSDLOADSOURCE_Pde
};

/**
 *
 * Structure containing information on downloaded LBS data.
 *
 * This object is sent to notify the registered host client that ephemeris 
 * and almanac data has been downloaded.
 *
 * \sa SWI_TYPE_LbsDloadData
 * \sa SWI_TYPE_LbsDloadSource
 * \sa SWI_NOTIFY_LbsPdDloadData
 *
 */
typedef struct SWI_STRUCT_LbsPdDloadData
{
    swi_uint32 sizeStruct;         /*!<The size, in bytes, of this structure. */
    swi_uint16 nValidEntryPairNum; /*!<Indicates how many valid entries below */
    enum SWI_TYPE_LbsDloadData eDataType1; /*!< Entry 1 data type */
    enum SWI_TYPE_LbsDloadSource eSource1; /*!< Entry 1 source */
    enum SWI_TYPE_LbsDloadData eDataType2; /*!< Entry 2 data type */
    enum SWI_TYPE_LbsDloadSource eSource2; /*!< Entry 2 source */
    enum SWI_TYPE_LbsDloadData eDataType3; /*!< Entry 3 data type */
    enum SWI_TYPE_LbsDloadSource eSource3; /*!< Entry 3 source */
    enum SWI_TYPE_LbsDloadData eDataType4; /*!< Entry 4 data type */
    enum SWI_TYPE_LbsDloadSource eSource4; /*!< Entry 4 source */
    enum SWI_TYPE_LbsDloadData eDataType5; /*!< Entry 5 data type */
    enum SWI_TYPE_LbsDloadSource eSource5; /*!< Entry 5 source */
}SWI_STRUCT_LbsPdDloadData;

/**
 *
 * Skeleton structure returned when a position determination download
 *    session has completed.
 *
 * \sa SWI_NOTIFY_LbsPdDloadDone
 *
 */
typedef struct SWI_STRUCT_LbsPdDloadDone
{
    swi_uint32 sizeStruct; /*!< The size, in bytes, of this structure. */  
}SWI_STRUCT_LbsPdDloadDone;

/**
 *
 * Structure containing status of a position determination download session.
 *
 * This object is typically sent to notify the registered host clients that a
 * position determination fix session had an error.
 *
 * \sa SWI_NOTIFY_LbsPdDloadEnd
 *
 */
typedef struct SWI_STRUCT_LbsPdDloadEnd
{
    swi_uint32 sizeStruct;       /*!< The size, in bytes, of this structure. */
    enum SWI_TYPE_LbsPdEndStatus eEndStatus; /*!< Status code */
}SWI_STRUCT_LbsPdDloadEnd;

/**
 *
 * Structure containing TCP/IP address used in LBS sessions.
 *
 * \sa SWI_NOTIFY_LbsPaIpAddr
 * \sa SwiGetLbsPaIpAddr
 * \sa SwiSetLbspaIpAddr
 *
 */
typedef struct SWI_STRUCT_LbsPaIpAddr
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! TCP/IP address to connect to for LBS activity when using TCP/IP 
        transport mechanism. */
    swi_uint32 nIpAddress;
}SWI_STRUCT_LbsPaIpAddr;

/**
 *
 * Structure containing GPS lock used in LBS sessions.
 *
 * \sa SWI_TYPE_LbsPaGpsLock
 * \sa SWI_NOTIFY_LbsPaGpsLock
 * \sa SwiGetLbsPaGpsLock
 * \sa SwiSetLbsPaGpsLock
 *
 */
typedef struct SWI_STRUCT_LbsPaGpsLock
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! An enumeration of type SWI_TYPE_LbsPaGpsLock representing the GPS lock
        state. */
    enum SWI_TYPE_LbsPaGpsLock eGpsLock;
}SWI_STRUCT_LbsPaGpsLock;

/**
 *
 * Structure containing mechanism used to transport LBS messages.
 *
 * \sa SWI_TYPE_LbsPaTransMech
 * \sa SWI_NOTIFY_LbsPaPtlmMode
 * \sa SwiGetLbsPaPtlmMode
 * \sa SwiSetLbsPaPtlmMode
 *
 */
typedef struct SWI_STRUCT_LbsPaPtlmMode
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! An enumeration of type SWI_TYPE_LbsPaTransMech representing the
        transportation mechanism used. */
    enum SWI_TYPE_LbsPaTransMech eTransportMech;
}SWI_STRUCT_LbsPaPtlmMode;

/**
 *
 * Structure containing port ID used in LBS sessions.
 *
 * \sa SWI_NOTIFY_LbsPaPortId
 * \sa SwiGetLbsPaPortId
 * \sa SwiSetLbsPaortId
 *
 */
typedef struct SWI_STRUCT_LbsPaPortId
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! Port ID to connect to for LBS activity when using TCP/IP transport
        mechanism */
    swi_uint32 nPortId;
}SWI_STRUCT_LbsPaPortId;

/**
 *
 * Structure containing privacy level used in LBS sessions.
 *
 * \sa SWI_TYPE_LbsPaPrivacy
 * \sa SWI_NOTIFY_LbsPaPrivacy
 * \sa SwiGetLbsPaPrivacy
 * \sa SwiSetLbsPaPrivacy
 *
 */
typedef struct SWI_STRUCT_LbsPaPrivacy
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! An enumeration of type SWI_TYPE_LbsPaPrivacy representing the privacy
        level */
    enum SWI_TYPE_LbsPaPrivacy ePrivacy;
}SWI_STRUCT_LbsPaPrivacy;

/**
 *
 * Structure containing network access permission used during LBS sessions.
 *
 * \sa SWI_TYPE_LbsPaNetAccess
 * \sa SWI_NOTIFY_LbsPaNetAccess
 * \sa SwiGetLbsPaNetAccess
 * \sa SwiSetLbsPaNetAccess
 *
 */
typedef struct SWI_STRUCT_LbsPaNetAccess
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! An enumeration of type SWI_TYPE_LbsPaNetAccess representing the
        network access permissions */
    enum SWI_TYPE_LbsPaNetAccess eNetAccess;
}SWI_STRUCT_LbsPaNetAccess;

/**
 *
 * Structure containing base station information.
 *
 * \sa SWI_NOTIFY_LbsPaBsInfo
 * \sa SwiGetLbsPaBsInfo
 *
 */
typedef struct SWI_STRUCT_LbsPaBsInfo
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application */
    swi_uint32 sizeStruct;

    /*! False if info not state, true otherwise */
    swi_bool bInfoStale;
   
    /*! Base station ID */
    swi_uint32 nId;

    /*! System ID */
    swi_uint32 nSid;
  
    /*! Network ID */
    swi_uint32 nNid;

    /*! Time stamp high DWORD */
    swi_uint32 nInfoTimeHigh;

    /*! Time stamp low DWORD */
    swi_uint32 nInfoTimeLow;

    /*! Base station latitude */
    swi_int32 nLatitude;

    /*! Base station longitude */
    swi_int32 nLongitude;
}SWI_STRUCT_LbsPaBsInfo;
/**
 *
 * Structure containing satellite information
 *
 * \sa SWI_STRUCT_LbsSatInfo
 *
 */
typedef struct SWI_STRUCT_SatView
{
    /*! Satellite bitmask
        - Bit 7: "1" indicates the satellite is used in latest fix
        - Bits 0-6: Satellite Number (0-32) */    
    swi_uint8 nSvNumber;
    swi_uint8 nSvElevation; /*!< Satellite Elevation in degrees (0-90) */    
    swi_uint16 nSvAzimuth;  /*!< Satellite Azimuth in degrees (0-360) */
    swi_uint8 nSvSnr;       /*!< Satellite Signal/Noise Ratio in dB (0-99) */
} SWI_STRUCT_SatView;

/**
 *
 * Structure containing base station information.
 *
 * Used by SwiGetLbsSatInfo to retrieve information about the current
 * base station currently being accessed.
 *
 * \sa SWI_NOTIFY_LbsSatInfo
 * \sa SwiGetLbsSatInfo
 *
 */
typedef struct SWI_STRUCT_LbsSatInfo
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! Number of satellites in view (max 12) */
    swi_uint8 nSvInView;

    /*! Array of satellite information */
    struct SWI_STRUCT_SatView sSatView[MAX_NUM_LBS_SV];
}SWI_STRUCT_LbsSatInfo;

/**
 *
 * Structure containing clear assistance parameters
 *
 * Used by SwiSetLbsClearAssistance to clear various location assistance
 * parameters to simulate a cold start.
 *
 * \sa SwiSetLbsClearAssistance
 *
 */
typedef struct SWI_STRUCT_LbsClearAssistance
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! Set to true to clear satellite ephemeris data. */
    swi_bool bClrEphemerisData; 
    
    /*! Set to true to clear satellite almanac data. */        
    swi_bool bClrAlmanacData;
    
    /*! Set to true to clear position and time data. */
    swi_bool bClrPosData;
    
    /*! Set to true to clear standalone self learning data base. */
    swi_bool bClrSelfLearnDb;
    
    /*! Set to true to clear standalone serving system data base. */
    swi_bool bClrServSysDb;    
    
   /*! Set to true to clear standalone time reference. */    
   swi_bool bClrTimeRef;        
    
   /*! Set to true to reset MSB throttling parameters. */
   swi_bool bResetThrottleParams; 
}SWI_STRUCT_LbsClearAssistance;

/* prototypes */
SWI_API SWI_RCODE SwiSetLbsPdGetPos(
                  const struct SWI_STRUCT_LbsPdGetPos *psLbsPdGetPos, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPdStatus(
                  struct SWI_STRUCT_LbsPdGetStatus *psLbsPdGetStatus, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPdTrack(
                  const struct SWI_STRUCT_LbsPdTrack *psLbsPdTrack, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPdEndSession(
                  const struct SWI_STRUCT_LbsPdEndSession *psLbsPdEndSession, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaParam(
                  struct SWI_STRUCT_LbsPaGetParam *psLbsPdGetParam, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPdData(
                  struct SWI_STRUCT_LbsPdData *psLbsPdData, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaIpAddr(
                  struct SWI_STRUCT_LbsPaIpAddr *psPaIpAddr,
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPaIpAddr(
                  const struct SWI_STRUCT_LbsPaIpAddr *psPaIpAddr, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsPaPortId(
                  struct SWI_STRUCT_LbsPaPortId *psPaPortId, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsPaPortId(
                  const struct SWI_STRUCT_LbsPaPortId *psPAPortID, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetLbsSatInfo(
                  struct SWI_STRUCT_LbsSatInfo *psSatInfo, 
                  swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetLbsClearAssistance(
             const struct SWI_STRUCT_LbsClearAssistance *psLbsClearAssistance, 
             swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_CM_LBS */
/* 
 * $Log: SwiApiCmLbs.h,v $
 */
