/**
 *    \if CvsId 
 *    $Id: SwiApiNotify.h,v 1.31 2011/02/11 22:09:02 ygao Exp $
 *    \endif
 *    \ingroup notify
 *    \file SwiApiNotify.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiNotify.c file
 * 
 *    Copyright (c) 2011 Sierra Wireless, Inc.  All rights reserved
 *
 */

#ifndef __SWI_API_NOTIFY_H__
#define __SWI_API_NOTIFY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmNetwork.h"
#include "SwiApiGsmSim.h"
#include "SwiApiGsmSs.h"
#include "SwiApiGsmPhonebook.h"
#include "SwiApiGsmStk.h"
#include "SwiApiGsmVoice.h"
#include "SwiApiCmBasic.h"
#include "SwiApiCmUpgrade.h"
#include "SwiApiCmLbs.h"
#include "SwiApiCmDm.h"
#include "SwiApiGsmLbs.h"
#include "SwiApiCdmaBasic.h"
#include "SwiIntCdma.h"
#include "SwiApiCdmaIota.h"
#include "SwiApiCdmaLbs.h"
#include "SwiApiCdmaNetwork.h"
#include "SwiApiCdmaRuim.h"
#include "SwiApiCdmaSms.h"
#include "SwiApiCdmaVoice.h"
#include "SwiApiCmDm.h"


/* defines */
#define LENGTH_MaxPPPString                 33
#define LENGTH_IPAddress                    16
#define LENGTH_LogFileName                 128
#define LENGTH_SdkPathName                 128
#define LENGTH_CommPort                      7
#define LENGTH_UsbdVersion                  85
#define LENGTH_UsbdBuildDate                 9
#define LENGTH_BootBuildDate                 9
#define NUMVALUE_WIMAX                   80216
#define MAX_NOTIF_LIST                     122
#define LENGTH_DriverName                  100
#define LENGTH_MacAddress                    6
#define LENGTH_ProductName                  20
#define LENGTH_ModemName                    75
#define LENGTH_HardwareId                   50
#define MAX_NUM_LBS_SV                      12
#define LENGTH_TimeStamp                    16
#define MAX_BAND_GROUPS                     50
#define MAX_BAND_LIST                       50
#define MAX_AIR_SERVERS                     50  /* Must be < than a frame. */


/* structure declarations */

/**
 *
 * This object is sent to the host to indicate when a profile has 
 * changed
 *
 */
struct SWI_STRUCT_GsmProfileChange
{
   /*! This is the size of this structure */
   swi_uint32                     sizeStruct;                     
   /*! Profile ID (1-16) */
   swi_uint8                      nProfileId;                     
   /*! Profile info type */
   enum SWI_TYPE_ProfileInfo      eType;                      

   /*!  Only 2 filters are currently supported (1-2).
    *   Only valid if eType is SWI_RPOFILEINFO_Tft 
    */
   swi_uint32                     nFilterId;                              
};


/**
 *
 * This structure contains the default Profile ID.
 *
 * \sa SWI_NOTIFY_GsmDefaultChange
 *
 */
struct SWI_STRUCT_GsmDefaultChange
{
   /*! The size, in bytes, of this structure.  This value must always be 
       populated by the calling application. */
   swi_uint32                     sizeStruct;                     

   /*! The new default Profile ID.  Valid range is from 1 to 16. */
   swi_uint8                      nProfileId;                                              
};

/**
 *
 * This enumerated data type defines a variety of notification types.
 *
 * API notifications are always enabled while modem originated notifications
 * must be enabled before an update is sent form the modem.  Notifications are
 * disabled by default at power up and after a reset.  Therefore, application
 * must re-enable all desired notifications when device powers up or a reset
 * is detected.
 *
 * The following common API notifications are always enabled and do not 
 * require enabling.  Modem originated notifications must be enabled
 * before an update is sent from the modem.
 *             - SWI_NOTIFY_AirServerChange
 *             - SWI_NOTIFY_LockAirServer
 *             - SWI_HW_NOTIFY_Power
 *             - SWI_NOIIFY_Power
 *             - SWI_NOTIFY_MUX_Shutdown
 *
 */
enum SWI_TYPE_Notify
{
    /*! Invalid notification.  This is simply a place holder. */
    SWI_NOTIFY_NULL,    

    /*! Air server insertion/removal notification.  This notification is 
        always enabled and can not be disabled. 
        \sa The corresponding user structure is SWI_STRUCT_AirServerChange */
    SWI_NOTIFY_AirServerChange,

    /*! Air server lock notification.  This notification is always enabled
        and can not be disabled. 
        \sa The corresponding user structure is SWI_STRUCT_LockAirServer*/
    SWI_NOTIFY_LockAirServer,   

    /*! Hardware power state change. No need to re-enable mask.  This 
        notification is always enabled and can not be disabled.  Only supported
        on CDMA devices.
        \sa The corresponding user structure is SWI_HW_STRUCT_Power */
    SWI_HW_NOTIFY_Power = 4,

    /*! Device power notification.  Previously used to indicate device 
        removal.  SWI_NOTIFY_AirServerChange should now be used instead. 
        This notification is always enabled and can not be disabled. 
        \sa The corresponding user structure is SWI_STRUCT_Power */         
        SWI_NOTIFY_Power,

    /*! API service has shutdown.  API service must be restarted and 
        SwiApiClose and SwiApiOpen must be called before normal operation
        can proceed.  This notification is always enabled and can not be
        disabled.  This notification does not have an associated user 
        structure. */
        SWI_NOTIFY_MUX_Shutdown,

    /*! Provides a mechanism to assure the application that the modem is still
        connected and running normally.  This event is forwarded approximately
        every 10 seconds.  It is recommended that this object no longer be used
        as it prevents the host USB bus controller from entering into low
        power state to conserve battery life.
        \sa The corresponding user structure is SWI_STRUCT_HeartBeat 
        \sa This notification is not implemented in LinuxSDK but
        \sa may be in the future - DO NOT USE yet */
        SWI_NOTIFY_HeartBeat = 100, 

    /*! Notifies the app of headset attachment state.
        \sa The corresponding user structure is SWI_STRUCT_HeadsetIcon */
    SWI_NOTIFY_HeadsetIcon,

    /*! Notifies the application of the headset state.  Headset state is not
        supported on the SB555. 
        \sa The corresponding user structure is SWI_STRUCT_HeadsetState */
    SWI_NOTIFY_HeadsetState,

    /*! The Received Signal Strength Indicator (RSSI) is reported only when the
        value changes.  The notification is issued at the slot cycle index as
        set by the service provider.  For larger slot cycle indexes, the 
        notification is issued less often.  A slot cycle index of 1 generates
        this notification at about 2-second intervals. 
        \sa The corresponding user structure is SWI_STRUCT_Rssi */
    SWI_NOTIFY_Rssi,

    /*! Indicates to the application that there is a problem with the antenna.
        It should be assumed that the problem will not persist longer than 10
        seconds.  If the antenna problem continues another transmit alert event
        will be generated by the modem. 
        \sa The corresponding user structure is SWI_STRUCT_TransmitAlert */
    SWI_NOTIFY_TransmitAlert,

    /*! Reports to the application when the modem resets. This notification is
        received when modem is inserted, after power up, after a firmware
        download and during any process which causes a modem reset. Modem 
        notifications which were previously enabled will default back to their
        original state (off).  When received, the application must re-enable 
        all desired notifications again.  This notification is always enabled 
        and can not be disabled. 
        \sa The corresponding user structure is SWI_STRUCT_Reset */
    SWI_NOTIFY_Reset,   

    /*! Provides the application with band status updates, typically occurring
        after the frequency band has changed. 
        \sa The corresponding user structure is SWI_STRUCT_Band*/
    SWI_NOTIFY_Band,

    /*! Not supported in this version of the SDK.
        \sa The corresponding user structure is SWI_STRUCT_Debug */
    SWI_NOTIFY_Debug,

    /*! Provides firmware download progress status.  Errors during the firmware
        download process are also reported. 
        \sa The corresponding user structure is SWI_STRUCT_FwDownload*/
    SWI_NOTIFY_FwDownload,

    /*! Not supported in this version of the SDK.
        \sa The corresponding user structure is SWI_STRUCT_Dhcp */
    SWI_NOTIFY_Dhcp,

    /*! Not supported in this version of the SDK.
        \sa The corresponding user structure is SWI_STRUCT_PPP */
    SWI_NOTIFY_PPP,

    /*! Indicates the modem power state has changed.  This notification is only
        available on 3G or newer CDMA and GSM products.  
        \sa The corresponding user structure is SWI_STRUCT_RadioPower. */
    SWI_NOTIFY_RadioPower,

    /*! Indicates the modem temperature state has changed.  This notification 
        is only available on 3G or newer CDMA and GSM products. 
        \sa The corresponding user structure is SWI_STRUCT_RadioTempState. */
    SWI_NOTIFY_RadioTempState,

    /*! Indicates the current supply voltage of the modem has changed.  This 
        notification is only available on CDMA and GSM MiniCard products. 
        \sa The corresponding user structure is SWI_STRUCT_RadioBattery. */
    SWI_NOTIFY_RadioBattery,

    /*! Indicates there is a change in the band(s) the modem is currently 
        using.  This notification is available on 3G CDMA and GSM products.
        On some CDMA devices, multiple bands can be used at one time.  
        \sa The corresponding user structure is SWI_STRUCT_RadioBandList. */
    SWI_NOTIFY_RadioBand,

    /*! Indicates there is a change in the modem's audio profile.  This 
        notification is available on 3G CDMA and GSM products.  
        \sa The corresponding user structure is SWI_STRUCT_AudioProfile. */
    SWI_NOTIFY_AudioProfile,
    
    /*! Indicates a change in the system time of the device  This notification
        is available on 3G CDMA and GSM products.  This notification is only 
        sent on initial network attach for GSM products, when service is
        acquired/reacquired for CDMA based products, and if and when the time
        zone information changes. 
        \sa The corresponding user structure is SWI_STRUCT_DeviceTime. */
    SWI_NOTIFY_DeviceTime,

    /*! MP firmware download notification.  There is no user structure
        associated with this object. */
    SWI_NOTIFY_MpFwDownload = 300,

    /*! Provides constant and up-to-date GPS information at regular intervals.
        \sa The corresponding user structure is SWI_STRUCT_Gps */
    SWI_NOTIFY_Gps,     

    /*! Provides current logic level of all external digital I/Os.
        \sa The corresponding user structure is SWI_STRUCT_ExtDioNotif */
    SWI_NOTIFY_ExtDio,

    /*! Provides current logic level of all external analog inputs.
        \sa The corresponding user structure is SWI_STRUCT_ExtAiNotif */
    SWI_NOTIFY_ExtAi,   

    /*! Provides the result/response of a GPS command sent to the GPS module
        in the modem.
        \sa The corresponding user structure is SWI_STRUCT_GpsCommand */
    SWI_NOTIFY_GpsCommand,  

    /*! Provides byte counter information.
        \sa The corresponding user structure is SWI_STRUCT_MpByteCounter */
    SWI_NOTIFY_MpByteCount, 

    /*! First informs the host software when the ignition is turned off, then 
        when shutdown time approaches, more notifications are sent to indicate
        that the MP750 will soon be shutdown.
        \sa The corresponding user structure is SWI_STRUCT_IgnSense */
    SWI_NOTIFY_MPIgnSense,

    /*! RF download progress.
        \sa The corresponding user structure is SWI_STRUCT_RfDownload */
    SWI_NOTIFY_RfDownload,  

    /*! Provides SIM sled status.
        \sa The corresponding user structure is SWI_STRUCT_SimSled */
    SWI_NOTIFY_SimSled,     

    /*! Provides low battery alert.
        \sa The corresponding user structure is SWI_STRUCT_BatteryStatus */
    SWI_NOTIFY_BatteryStatus,

    /*! Provides the reason behind a MP shutdown 
        \sa The corresponding user structure is SWI_STRUCT_MpShutdown */
    SWI_NOTIFY_MpShutdown,  

    /*! Provides MP test connection result (Under a Serial connection). 
        Provides MP connection result (Under a USB connection)
        \sa The corresponding user structure is SWI_STRUCT_MpConnect */
    SWI_NOTIFY_MpConnect,

    /*! MP startup diagnostics.
        \sa The corresponding user structure is SWI_STRUCT_MpStartupDiag */
    SWI_NOTIFY_MpStartupDiag,   

    /*! MP disconnect notification.  
        \sa The corresponding user structure is SWI_STRUCT_MpDisconnect */
    SWI_NOTIFY_MpDisconnect,    

    /*! Reports the status of SMS records on the modem whenever a new message
        arrives.  Whenever an SMS message arrives, the modem can generate this
        event notification.  The data reported advises the application of the 
        number of SMS messages stored in the modem.  This SmsStatus 
        notification will trigger on the arrival of any SMS message that can 
        be stored in the modem. If the modem memory fills, and additional
        messages are declined, there will be no further notifications.
        \sa The corresponding user structure is SWI_STRUCT_GSM_SMS_Status for
        GMS devices and SWI_STRUCT_CDMA_SMS_Status for CDMA devices. */
    SWI_NOTIFY_SmsStatus = 500,

    /*! Reports the results of sending a mobile originated SMS message.  This 
        notification passes the acknowledgment from the network to the 
        application for SMS messages sent. 
        \sa The corresponding user structure is SWI_STRUCT_GSM_SMS_SendStatus 
        for GSM devices and SWI_STRUCT_CDMA_SMS_SendStatus for CDMA devices. */
    SWI_NOTIFY_SmsSendStatus,

    /*! Notifies the host application of the result of the position fix. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdData. */
    SWI_NOTIFY_LbsPdData = 603,

    /*! Notifies the host application that a position determination download
        session has completed. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdDone. */ 
    SWI_NOTIFY_LbsPdDone,

    /*! Notifies the host application that a position determination fix session
        had an error. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdEnd. */
    SWI_NOTIFY_LbsPdEnd,

    /*! Notifies the host application that a position determination fix session
        has begun. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdBegin. */
    SWI_NOTIFY_LbsPdBegin,

    /*! Notifies the host application that a position determination session
        has failed. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdUpdateFailure.*/
    SWI_NOTIFY_LbsPdUpdateFailure,

    /*! Notifies the host application that a data download session has begun.
        \sa The corresponding user structure is SWI_STRUCT_LbsPdDloadBegin. */
    SWI_NOTIFY_LbsPdDloadBegin,

    /*! Notifies the host application that ephemeris and almanac data has been
        downloaded.
        \sa The corresponding user structure is SWI_STRUCT_LbsPdDloadData. */
    SWI_NOTIFY_LbsPdDloadData,

    /*! Notifies the host application that a position determination download
        session has completed. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdDloadDone. */
    SWI_NOTIFY_LbsPdDloadDone,

    /*! Notifies the host application that a position determination download
        data session had an error. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdDloadEnd. */
    SWI_NOTIFY_LbsPdDloadEnd,

    /*! This notification contains the default value for the TCP/IP address
        used in LBS sessions. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaIpAddr. */
    SWI_NOTIFY_LbsPaIpAddr,

    /*! This notification contains the default value for the GPS lock used in
        LBS sessions. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaGpsLock. */
    SWI_NOTIFY_LbsPaGpsLock,

    /*! This notification contains the mechanism used to transport LBS 
        messages.
        \sa The corresponding user structure is SWI_STRUCT_LbsPaPtlmMode. */
    SWI_NOTIFY_LbsPaPtlmMode,
    
    /*! This notification contains the default value for the port ID used in
        LBS sessions. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaPortId. */
    SWI_NOTIFY_LbsPaPortId,

    /*! This notification contains the default value for the privacy level
        used for LBS sessions. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaPrivacy. */
    SWI_NOTIFY_LbsPaPrivacy,

    /*! This notification contains the default value for the network access
        permissions used during LBS sessions.
        \sa The corresponding user structure is SWI_STRUCT_LbsPaNetAccess. */
    SWI_NOTIFY_LbsPaNetAccess,

    /*! This notification contains the base station currently being accessed. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaBsInfo. */
    SWI_NOTIFY_LbsPaBsInfo,

    /*! Notifies the registered host application that keep warming processing
        has begun.  
        \sa The corresponding user structure is SWI_STRUCT_LbsPaWarmBegin. */
    SWI_NOTIFY_LbsPaWarmBegin,

    /*! This notification reports the keep warm processing status and is only
        available on GSM devices. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaWarmStatus. */
    SWI_NOTIFY_LbsPaWarmStatus,

    /*! Notifies the host application that keep warm processing is done. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPaWarmDone. */
    SWI_NOTIFY_LbsPaWarmDone,

    /*! This notification lists satellites visible during the last position fix
        session.  It also identifies the satellites used in the latest position
        fix. 
        \sa The corresponding user structure is SWI_STRUCT_LbsSatInfo. */
    SWI_NOTIFY_LbsSatInfo,

    /*! This notification provides the current status of the LBS. 
        \sa The corresponding user structure is SWI_STRUCT_LbsPdGetStatus. */
    SWI_NOTIFY_LbsPdGetStatus,
    
    /*! Notifies the host of a network initiated location fix request. 

        The associated user structure is SWI_STRUCT_LbsNiReq.  The host should 
        display the requester ID and client name if these fields are present.  

        If the notification type requires a user response, the host should 
        present a dialog to the user with the options &quot;Accept&quot; and 
        &quot;Reject&quot; for the user to choose from.  The host will respond 
        using SwiSetLbsNiReq to the modem based on the user&apos;s selection
        (TYPE_LbsNiReqResp).  If the user chooses not to respond (e.g. closed 
        dialog box), the host must respond with a set request where the user 
        response is &quot;Cancel request&quot;.  If the user does not respond 
        to the dialog within the specified timeout period, the host should
        close the dialog and send a set request where the user response is 
        &quot;Cancel request&quot;.  Note that the modem will assume the user 
        response is &quot;Cancel request&quot; after the timeout period has 
        elapsed regardless of what the host continues to send.

        \sa SWI_STRUCT_LbsNiReq 
        \sa SwiSetLbsNiReq */
    SWI_NOTIFY_LbsNiReq,

    /*! Notifies the host of GPS One Xtra subsystem requests for
                almanac injection data.
        \sa The corresponding user structure is SWI_STRUCT_LbsXtraAssist. */
    SWI_NOTIFY_LbsXtraAssistData,

    /*! Notifies the host of GPS One Xtra subsystem requests for
                time injection data.
    \sa The corresponding user structure is SWI_STRUCT_LbsXtraAssist. */
    SWI_NOTIFY_LbsXtraTimeInfo,

    /*! Notifies the host of GPS One Xtra subsystem data download trigger.
    \sa The corresponding user structure is SWI_STRUCT_LbsXtraDloadTrigger. */
    SWI_NOTIFY_LbsXtraDloadTrigger,

    /*! Reports each channel number the radio tunes to.  Using this 
        notification type, the application is advised each time the radio tunes
        to a different channel. This can be very frequent when the radio is 
        scanning for a usable channel.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_ChannelNumber */
    SWI_NOTIFY_ChannelNumber = 1000,

    /*! Reports channel acquisition and scanning.  Using this notification 
        type, the application can keep the user aware of changes in the channel
        lock of the radio. Whenever the radio acquires or loses a channel,
        this notification is issued.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_ChannelState */
    SWI_NOTIFY_ChannelState,

    /*! Reports entering and leaving coverage.  Using this notification type, 
        the application can keep the user aware of changes in the channel lock
        of the radio. Whenever the radio acquires or loses a channel, this 
        notification is issued.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_ServiceIndication */
    SWI_NOTIFY_ServiceIndication,

    /*! Reports the current roaming state of the modem.  Using this 
        notification type, the application can keep the user aware of changes
        in the received signal strength.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_RoamingStatus */
    SWI_NOTIFY_RoamingStatus,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_Ecio,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_AuthenticationStatus,

    /*! Reports data traffic received and transmitted.  Using this notification
        type, the application can keep the user aware of the amount of traffic 
        on the current connection. The notification is issued at 1-second 
        intervals during a data connection.  
    
        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallByteCounter */
    SWI_NOTIFY_CallByteCounter,

    /*! Reports when a call connection is initiated.  This notification 
        advises the application that the process of connecting a call has
        initiated. There is no polling equivalent.  This notification is
        enabled and disabled via SWI_NOTIFY_CallNotificationStatus.
    
        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallConnecting */
    SWI_NOTIFY_CallConnecting,

    /*! Reports the successful connection of a call.  This notification 
        advises the application that a call has been successfully connected. 
        This notification is enabled and disabled via 
        SWI_NOTIFY_CallNotificationStatus.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallConnected */
    SWI_NOTIFY_CallConnected,

    /*! Equivalent to a ring, this advises of the incoming call type as 
        well.  This notification advises the application that there is an 
        incoming call request.  This notification is enabled and disabled via
        SWI_NOTIFY_CallNotificationStatus.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallIncoming */
    SWI_NOTIFY_CallIncoming,

    /*! Reports the standard caller ID of incoming calls (where supported by 
        the network).  This notification type passes the caller ID string to 
        the application for incoming calls (where supported by the network).
        The format of the string is determined by the network. Both the 
        standard and extended caller IDs are sent if enabled. 

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallCallerID */
    SWI_NOTIFY_CallCallerID,

    /*! Reports the extended caller ID of incoming calls (where supported by 
        the network).  This notification type passes an extended caller ID 
        string to the application for incoming calls (where supported by the 
        network).  The format of the string is determined by the network. Both
        the standard and extended caller IDs are sent if enabled.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallExtendedCallerID*/
    SWI_NOTIFY_CallExtendedCallerID,

    /*! Reports when a call is disconnected.  This notification advises the 
        application that a call has been disconnected. This notification is
        enabled and disabled via SWI_NOTIFY_CallNotificationStatus. 

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallDisconnected */
    SWI_NOTIFY_CallDisconnected,

    /*! Reports errors in call management operations.  This notification 
        advises the application that an error or fault has been detected in a
        call.  This can happen anytime during a connection.  This notification
        is enabled and disabled via SWI_NOTIFY_CallNotificationStatus.
    
        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallError */
    SWI_NOTIFY_CallError,

    /*! Reports changes in the protocol revision available from the serving
        base station.  This notification type advises the application when 
        there is a change in the protocol revision supported by the serving 
        base station.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Prev */
    SWI_NOTIFY_Prev,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_ActiveNam,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechSid,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechNid,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechBsInfo,

    /*! Reports changes in the radio band being used (cellular or PCS).  This 
        notification type informs the application of changes in the radio
        band being used: cellular or PCS.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_TechBandClass */
    SWI_NOTIFY_TechBandClass,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechRegZone,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechPacketZone,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechPcsBlock,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_TechCellChannel,

    /*! Only supported on CDMA devices.*/
    SWI_NOTIFY_SystemTime,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_ActivationStatus,

    /*! Only supported on CDMA devices. */
    SWI_NOTIFY_V42BisEnabled,

    /*! Enables and disables a group of notifications related to call 
        management.  This notification type is used to enable and disable
        the following call management notifications as a single block:
        - SWI_NOTIFY_CallConnecting
        - SWI_NOTIFY_CallConnected
        - SWI_NOTIFY_CallIncoming
        - SWI_NOTIFY_CallDisconnected
        - SWI_NOTIFY_CallError
        - SWI_NOTIFY_CallDormant
        - SWI_NOTIFY_OTASPState
        
        There is no actual notification data associated with 
        CallNotificationStatus itself.  

        Only supported on CDMA devices.
        \sa SWI_NOTIFY_CallConnecting
        \sa SWI_NOTIFY_CallConnected
        \sa SWI_NOTIFY_CallIncoming
        \sa SWI_NOTIFY_CallDisconnected
        \sa SWI_NOTIFY_CallError
        \sa SWI_NOTIFY_CallDormant
        \sa SWI_NOTIFY_OTASPState */
    SWI_NOTIFY_CallNotificationStatus,

    /*! Reports when a call enters dormant state.  This notification advises
        the application that a call has been placed in dormant state.  When a
        call leaves dormant state, the SWI_NOTIFY_CallConnected notification 
        is issued.  This notification is enabled and disabled via 
        SWI_NOTIFY_CallNotificationStatus.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallDormant */
    SWI_NOTIFY_CallDormant,

    /*! Reports changes in voice call privacy.  When the Enhanced Privacy 
        Option is enabled (at manufacturing time), the modem issues a request
        to use the Call Privacy feature whenever a voice call (both incoming 
        and outgoing) is established. The base station can accept or reject 
        the request depending on its settings. If voice privacy is successfully
        established, this notification indicates that call privacy is on. In 
        addition, when a handover to a new base station occurs, the voice 
        privacy feature may or may not be available at the new base station;
        again triggering the notificiation if there is a change.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_CallPrivacy */
    SWI_NOTIFY_CallPrivacy,

    /*! Reports entry to, and exit from, E911 mode.  This notification type 
        advised the application when the modem enters or leaves E911 mode.

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_E911Mode */
    SWI_NOTIFY_E911Mode,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_ModemTooHotIndication,

    /*! Reports each stage of the OTASP process.  This notification advises
        the application that a call has been placed in dormant state. This 
        notification is enabled and disabled via 
        SWI_NOTIFY_CallNotificationStatus.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_OTASPState
        \sa SWI_NOTIFY_CallNotificationStatus */
    SWI_NOTIFY_OTASPState,

    /*! Reports entry to power-save mode.  This notification type advised the
        application when the modem enters power-save mode.  
    
        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_PowerSaveEnter */
    SWI_NOTIFY_PowerSaveEnter,

    /*! Reports exit from power-save mode.  This notification type advised the
        application when the modem leaves power-save mode.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_PowerSaveExit */
    SWI_NOTIFY_PowerSaveExit,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_FlashBurn,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_ExcessSPCFailure,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_HdrNotificationStatus,

    /*! Not supported in this version of the SDK.  Use SWI_NOTIFY_Hdr_Srv_State
        instead.
        \sa SWI_NOTIFY_Hdr_Srv_State */
    SWI_NOTIFY_Hdr_Acq,

    /*! Not supported in this version of the SDK.  Use 
        SWI_NOTIFY_Rssi_Ecio instead.
        \sa SWI_NOTIFY_Rssi_Ecio */
    SWI_NOTIFY_Hdr_Rssi,

    /*! Indicates a threshold change in RSSI or EcIo.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Rssi_Ecio */
    SWI_NOTIFY_Rssi_Ecio,

    /*! Indicates a change in HDR Roam status.  
    
        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Hdr_Roam_Status */
    SWI_NOTIFY_Hdr_Roam_Status,

    /*! Indicates a change in HDR Service State.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Hdr_Srv_State */
    SWI_NOTIFY_Hdr_Srv_State,

    /*! Indicates a change in HDR Hybrid Pref.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Hdr_Hybr_Pref */
    SWI_NOTIFY_Hdr_Hybr_Pref,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_Idle_Digital_Mode,

    /*! Indicates a change in DDTM pref.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_DDTM_Pref */
    SWI_NOTIFY_DDTM_Pref,

    /*! Indicates when modem has entered HDR Power save mode.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Hdr_PowerSave_Enter 
        */
    SWI_NOTIFY_Hdr_PowerSave_Enter,

    /*! Indicates when modem has exited HDR power save mode.  Only supported 
        on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Hdr_PowerSave_Exit */
    SWI_NOTIFY_Hdr_PowerSave_Exit,

    /*! Indicates HDR notification status mask. Only supported on CDMA 
        devices. */
    SWI_NOTIFY_TechHdrNotificationStatus,
    
    /*! Indicates HDR Prev. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Prev */
    SWI_NOTIFY_Tech_Hdr_Prev,

    /*! Indicates HDR Sector. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Sector */
    SWI_NOTIFY_Tech_Hdr_Sector,

    /*! Indicates HDR Sector. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_State */
    SWI_NOTIFY_Tech_Hdr_State,

    /*! Indicates HDR channel. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Channel */
    SWI_NOTIFY_Tech_Hdr_Channel,

    /*! Indicates HDR session. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Session */
    SWI_NOTIFY_Tech_Hdr_Session,

    /*! Indicates HDR an authentication. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_TYPE_Tech_Hdr_An_Auth */
    SWI_NOTIFY_Tech_Hdr_An_Auth,

    /*! Indicates HDR drc value. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Drc_Val */
    SWI_NOTIFY_Tech_Hdr_Drc_Val,

    /*! Indicates HDR drc cover. Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Drc_Cover */
    SWI_NOTIFY_Tech_Hdr_Drc_Cover,

    /*! Indicates HDR rri. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Rri */
    SWI_NOTIFY_Tech_Hdr_Rri,

    /*! Indicates HDR statistics. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_Tech_Hdr_Stats */
    SWI_NOTIFY_Tech_Hdr_Stats,

    /*! Indicates debug package data. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_DebugPacket */
    SWI_NOTIFY_DebugRevMsg,

    /*! Indicates HIOTA status. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_HIOTAStatus */
    SWI_NOTIFY_SPCS_HIOTA_STATUS,

    /*! Indicates RUIM status. Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_RuimStatus */
    SWI_NOTIFY_RuimStatus,

    /*! Embedded IOTA  status notification.  Need to enable/disable the mask.
    
        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_IOTA_State */
    SWI_NOTIFY_IOTA_Status, 

    /*! Embedded IOTA  WAP URI notification.  This should be enabled to receive
        the WAP trigger.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is SWI_STRUCT_IOTA_WAPPush */
    SWI_NOTIFY_IOTA_WAPPush,

    /* Rev A */
    /*! Indicates HDR Rev A su forward statistics data.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats */
    SWI_NOTIFY_Tech_Hdr_Su_Fwd_Stats,

    /*! Indicates HDR Rev A su forward count data.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt */
    SWI_NOTIFY_Tech_Hdr_Su_Fwd_Slot_Cnt,

    /*! Indicates HDR Rev A su forward count data.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2 */
    SWI_NOTIFY_Tech_Hdr_Su_Fwd_Slot_Cnt2,

    /*! Indicates HDR Rev A mu forward statistics data.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats */
    SWI_NOTIFY_Tech_Hdr_Mu_Fwd_Stats,
    
    /*! Indicates HDR Rev A personality data.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Personality */
    SWI_NOTIFY_Tech_Hdr_Personality,
    
    /*! Indicates HDR Rev A MAC index data.  

        Only supported on CDMA devices.
        \sa The corresponding user structure is 
        SWI_STRUCT_Tech_Hdr_Mac_Index */
    SWI_NOTIFY_Tech_Hdr_Mac_Index,    

    /*! Notifies the application of the network status with each change
        in service (status, roaming, cell ID, etc).

        Only supports GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_GSM_NetworkStatus */
    SWI_NOTIFY_NetworkStatus = 2000,

    /*! Reports the temperature of the radio whenever it changes. If the 
        temperature gets high, the application may elect to disconnect or 
        throttle back on traffic.       

        Only supports GSM devices. 
        \sa The corresponding user structure is SWI_STRUCT_Temperature */
    SWI_NOTIFY_Temperature,     

    /*! Reports that the SIM Card memory has been filled and any subsequent SMS
        messages will be declined by the modem (and possibly held at the SMS
        Service Center).

        Only supports GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_SMS_SimFull */
    SWI_NOTIFY_SmsSimFull,  

    /*! Reports the status of the frequency band change request initiated by 
        the SwiSetBandInfo function.  The new frequency band will be reported
        by the SWI_NOTIFY_Band notification.        

        Only supports GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_Band */
    SWI_NOTIFY_BandWrite,   

    /*! Notifies the application of the number of missed voice calls.  The 
        missed voice call count can be cleared by the application using the 
        SwiAcknowledgeMissedCall function.  The missed voice call count will 
        not persist through a modem reset.  

        Only supports GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_MissedCall */
    SWI_NOTIFY_MissedCall,      

    /*! Reports the current packet session status.  Used with AirCard 750 or 
        older GSM devices.  When working with newer GSM devices, 
        SWI_NOTIFY_PktSessionCall should be used instead.
        \sa The corresponding user structure is SWI_STRUCT_PktSessionStatus
        \sa SWI_NOTIFY_PktSessionCall */        
    SWI_NOTIFY_PktSessionStatus,    

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_Registration,

    /*! Not supported in this version of the SDK. */        
    SWI_NOTIFY_SmsSendReference, 

    /*! Notifies the application of the SIM's status.  This notification 
        provides a mechanism for the modem to prompt the user to enter the 
        PIN, PIN2, PUK, PUK2 or Special Code (MEP unlocking code).  After 
        every SIM related operation (PIN Change, Enabling of PIN Verification
        or PUK Entry) this notification will inform the host software of the 
        SIM Status and the result of the previous operation.  

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_SIM_StatusExp */
    SWI_NOTIFY_SimStatusExp,    

    /*! Provides the PLMN and/or SPN to be displayed to the user to indicate 
        what network the mobile is registered on.  This information should 
        only be displayed if the mobile is currently registered.  The 
        application will receive a new notification every time the mobile 
        registers on a new network. 

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_Registration */
    SWI_NOTIFY_RegistrationExp, 

    /*! Reports the progress of voice calls.  The host is notified of new 
        incoming voice calls.   

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_CALL_EXP_Progress */
    SWI_NOTIFY_CallProgress,

    /*! Reports the connected calls state.  Connected calls can be active, on 
        hold, in a multi-party call or suspended.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_CALL_Status */
    SWI_NOTIFY_CallStatus,  

    /*! Reports the transmit and receive byte counters on the modem.  The
        byte counters can be cleared by the application using the 
        SwiClearDataCounters function. The byte counters will not persist 
        through a modem reset.  

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_ByteCounter */
    SWI_NOTIFY_ByteCounter,

    /*! Reports the availability of Phonebook service (includes the ability to 
        read and edit the phonebooks).  

        Only supported on GSM devices 
        \sa The corresponding user structure is SWI_STRUCT_PhonebookReady */
    SWI_NOTIFY_PhonebookReady,  

    /*! Reports a phonebook entry.  Currently, only ADN phonebook is read from 
        this notification.  Note, when a new phonebook entry is added, a
        notification is not sent.   

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_PHONEBK_Entry */
    SWI_NOTIFY_PhonebookEntry,  

    /*! Reports the success of the PLMN mode change request initiated by the 
        SwiSetPLMNMode function.    

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_PlmnMode */
    SWI_NOTIFY_PlmnMode,            

    /*! Reports the success of the PLMN scan initiated by the 
        SwiStartPLMNSearch function.  This notification will indicate if any 
        networks were found in the scan.  The SwiGetPLMNSelection function 
        should be used to fetch the members of the list of found networks.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_PlmnAvailable */
    SWI_NOTIFY_PlmnAvailable,   

    /*! Reports the modem's enable/disable status.  
    
        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_ModemDisabled */
    SWI_NOTIFY_ModemDisabled,       
    
    /*! Reports the availability of manual PLMN selection service.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_PlmnReady */
    SWI_NOTIFY_PlmnReady,   

    /*! Notifies the application of a change in voice message waiting flags 
        that reside on the SIM.  These flags identify whether or not voice
        messages and data messages are being stored by the network for 
        retrieval by the mobile.  Since this notification will only be sent to 
        the application if there is a change in the mailbox flags it is 
        necessary for the application to call the SwiGetVoiceMessage function 
        to get the initial state of the mailbox flags at startup.  

        Only supported on GSM devices 
        \sa The corresponding user structure is SWI_STRUCT_VoiceMessage */
    SWI_NOTIFY_VoiceMessage,    

    /*! Allows the application to hide, gray out or display the GPRS icon 
        according to different carrier requirements.  When the notification 
        is enabled, a notification is immediately sent to the application.

        Only supported on GSM devices 
        \sa The corresponding user structure is SWI_STRUCT_IconGprsState */
    SWI_NOTIFY_IconGprsState,       

    /*! Allows the application to hide, gray out or display the ROAMING icon
        according to different carrier requirements.  When the notification is 
        enabled, a notification is immediately sent to the application. 

        Only supported on GSM devices 
        \sa The corresponding user structure is SWI_STRUCT_IconRoamState */
    SWI_NOTIFY_IconRoamState,   

    /*! Provides an indication to the application to display the "Call 
        Forwarded" string to the user.  Some subscriptions on some networks
        notify the user when a call is forwarded or has been forwarded. 

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_CallForwarded */
    SWI_NOTIFY_CallForwarded,   

    /*! Allows the application to gray out or display the Call Forwarding icon
        depending on the users current call forwarding configuration.  When the
        notification is enabled, a notification is immediately sent to the 
        application.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_CFIcon */
    SWI_NOTIFY_CFIcon,      

    /*! Not supported in this version of the SDK. */        
    SWI_NOTIFY_STKCommand,  

    /*! Updates the application with GPRS data rate statistics.  This 
        notification is sent periodically once it is enabled.  If the data 
        rate information has not change since the last SWI_NOTIFY_DataRate 
        notification was sent then no notification will be sent. 

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_DataRate */
    SWI_NOTIFY_DataRate,    

    /*! Provides the application with the service status information needed to
        inform the user of its current GPRS/EDGE service state.     
        \sa The corresponding user structure is SWI_STRUCT_ServiceIcon. */
    SWI_NOTIFY_ServiceIcon,     

    /*! Not supported in this version of the SDK. */    
    SWI_NOTIFY_TTYControl,  

    /*! Packet session call status.  Used with AirCard 875 or newer devices.
    
        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_PktSessionCall */
    SWI_NOTIFY_PktSessionCall,  

    /*! NV backup notification.  Used before a firmware upgrade is
        initiated to instruct the modem to back up information
        stored in non-volatile memory.  

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_NvStore */   
    SWI_NOTIFY_NvStore,     

    /*! HSDPA test channel.  Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_HSDPA_TestChan */
    SWI_NOTIFY_HSDPA_TestChan,

    /*! GSM System Status.  Notifies the application of system status with
        each change in service.
  
        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_GsmSystemStatus */
    SWI_NOTIFY_GsmSystemStatus,


    /*! GSM Received Signal Code Power(RSCP) and Energy per Chip per 
        Power Density (Ec/Io) of the active set's strongest cells.  

        This notifications is to be used for diagnostic purposes.  It 
        will be sent periodically and only if the reported data has changed.

        Information on the strongest active cells will be reported in 
        order of strongest to weakest for a maximum of six cells.
        These measurements are valid only for UMTS, if the current service 
        is GSM, no information shall be reported in this notification.

        The Ec/Io value will represent -dB in half step increments.  For
        example, a reported value of 0x0015 means Ec/Io = -10.5dB.
  
        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_RscpEcIo */
    SWI_NOTIFY_RscpEcIo,


    /*! GSM 2G Cell Information.  

        This notifications is to be used for diagnostic purposes.  It 
        will be sent periodically and only if the reported data has changed.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_GsmCellInfo */
    SWI_NOTIFY_GsmCellInfo,

    /*! GSM Profile Change.

        This notification indicates that a Connection Profile has changed and
        it gives the profile If and info type that the application may wish
        to refresh.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_GsmProfileChange */
    SWI_NOTIFY_GsmProfileChange,

   /*! GSM Default Profile Change.

        This notification indicates that a default Connection Profile has
        changed.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_GsmDefaultChange */
    SWI_NOTIFY_GsmDefaultChange,

    /*! Reports the availability of SMS service (includes the ability to
        read SMS messages from the SIM).  

        Only supported on GSM devices
        \sa The corresponding user structure is SWI_STRUCT_SmsReady */
    SWI_NOTIFY_SmsReady = 2200,

    /*! Indicates that the SMS copy initiated by the calling of the
        SwiStoreSMSMessageExp API is complete and that the application
        is now free to take another mobile-originated action using
        SwiStoreSMSMessageExp or SwiSendSMSMessageExp.  This notification
        will report the success of the last SMS copy request.
    
        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_SmsStoreExp
        \sa The SwiStoreSMSMessageExp API is associated with this 
        notification */
    SWI_NOTIFY_SmsStoreExp,     

    /*! Indicates that the SMS send initiated by the calling of the
        SwiSendSMSMessageExp API is complete and that the application
        is now free to take another mobile-originated action using
        SwiStoreSMSMessageExp or SwiSendSMSMessageExp.  This notification
        will report the success of the last SMS send request.

        Only supported on GSM devices.
        \sa The corresponding user structure is SWI_STRUCT_SmsSendExp
        \sa The SwiSendSMSMessageExp API is associated with this 
        notification */
    SWI_NOTIFY_SmsSendExp, 

    /*! Delivers an incoming SMS message or segment that is not to
        be stored on the SIM to the application. 

        \sa The corresponding user structure is SWI_STRUCT_SMS_Deliver */
    SWI_NOTIFY_SmsDeliver,

    /*! Not supported in this version of the SDK. */        
    SWI_NOTIFY_SmsReceived,

    /*! Not supported in this version of the SDK. */        
    SWI_NOTIFY_SmsSubmit,

    /*! Delivers an incoming SMS status report to the application. 

        Only supported on GSM devices. 
        \sa The corresponding user structure is SWI_STRUCT_SMS_StatusReport */
    SWI_NOTIFY_SmsStatusReport,

    /*! Not supported in this version of the SDK. */        
    SWI_NOTIFY_SmsSimStatus,

    /*! Indicates that the network is request a particular supplementary
        service password.  This notification may be a result of the 
        application initiating the password registration process 
        through the SwiStartSSPassReg API or the network may have
        initiated the process.

        The application is expected to answer this network
        request for password using the SwiSetSSPassword API.  
        
        Only supported on GSM devices.
        \sa SwiStartSSPassReg
        \sa SwiSetSSPassword
        \sa SWI_STRUCT_SS_PasswordStatus */
    SWI_NOTIFY_SSPasswordStatus = 2250,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request that was completed successful and 
        returned no information.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespOK */
    SWI_NOTIFY_SSRespOK,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it gives call forwarding service 
        status.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespFwdInfo */
    SWI_NOTIFY_SSRespFwdInfo,

      /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it gives call barring service 
        status.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespCallBar */
    SWI_NOTIFY_SSRespCallBar,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it gives service status on the  
        requested supplementary service.
    
        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespInfoData */
    SWI_NOTIFY_SSRespInfoData,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it gives the service status on 
        the requested supplementary service.
    
        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespStatus */
    SWI_NOTIFY_SSRespStatus,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it returns a single forwarded 
        number. 

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespFwdNum */
    SWI_NOTIFY_SSRespFwdNum,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it returns a list of basic
        service groups in used for the requested
        supplementary service.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.
    
        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespBSList */
    SWI_NOTIFY_SSRespBSList,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it returns a list of call 
        forwarding information for the requested
        supplementary service.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespListFwdInfo */
    SWI_NOTIFY_SSRespFwdList,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it returns a raw binary data.
    
        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespRaw */
    SWI_NOTIFY_SSRespRaw,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it returns password information.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespPassword */
    SWI_NOTIFY_SSRespPassword,

    /*! Indicates that a supplementary service action
        completed successfully. This notification is 
        a network response to a supplementary service
        request and it returns CLIR service
        and setting information.
    
        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespClir  */
    SWI_NOTIFY_SSRespClir,

    /*! Indicates that a supplementary service action
        completed unsuccessfully. This notification is 
        a network response to a supplementary service
        request and it returns error cause information.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespError  */
    SWI_NOTIFY_SSRespError,

    /*! Indicates that a supplementary service action
        completed unsuccessfully. This notification is 
        a network response to a supplementary service
        request and it returns rejection cause information.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespReject  */
    SWI_NOTIFY_SSRespReject,

    /*! Indicates that a supplementary service action
        completed cancelled. This notification is 
        a network response to a supplementary service
        request and it returns cancel cause.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespCancel  */
    SWI_NOTIFY_SSRespCancel,

    /*! Indicates that an outstanding supplementary 
        service action has been released.  This 
        notification is a network response to a
        supplementary service request.
        completed cancelled. This notification is 
        a network response to a supplementary service
        request and it returns cancel cause.

        The application uses the SwiSetSSRequest
        API to make supplementary service requests.
        The handle returned by this API call is
        used to tie the request to its network
        response.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SS_RespRelease  */
    SWI_NOTIFY_SSRespRelease,

    /*! Indicates that a particular supplemenary
        service is active.  This notification
        is a network indication.

        Only supported on GSM device.
        \sa SwiSetSSRequest
        \sa SWI_STRUCT_SSNotify  */
    SWI_NOTIFY_SSNotify,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_SS_USSD_REQ,

    /* OMA-DM */
    /*! Indicates modem OMA-DM configuration.
        \sa SwiGetOMADMConfig
        \sa SwiSetOMADMConfig
        \sa SWI_STRUCT_OMADM_GET_CONFIG  */
    SWI_NOTIFY_OMADM_Config,

    /*! Indicates modem OMA-DM session state.
        \sa SwiGetOMADMSessionState
        \sa SwiSetOMADMSessionState
        \sa SWI_STRUCT_OMADM_SESSION_STATE  */
    SWI_NOTIFY_OMADM_State,

    /*! Indicates modem OMA-DM NI alert.
        \sa SwiGetOMADMNIAlert
        \sa SWI_STRUCT_OMADM_NI_ALERT  */
    SWI_NOTIFY_OMADM_NI_Alert,
    
    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_OMADM_UI_Alert_Display,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_OMADM_UI_Alert_Confirm,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_OMADM_UI_Alert_Input,

    /*! Not supported in this version of the SDK. */
    SWI_NOTIFY_OMADM_UI_Alert_Choice,

    /* DM-DL */
    /*! Indicates modem DM-DL progress.
        \sa SwiGetDMDLProgress
        \sa SWI_STRUCT_DM_DL_PROGRESS  */
    SWI_NOTIFY_DM_DL_Progress,

    /*! Indicates modem DM-DL alert.
        \sa SWI_STRUCT_DM_DL_ALERT  */
    SWI_NOTIFY_DM_DL_Alert,

    /*! Indicates modem installation status.
        \sa SwiGetDMDLInstallStatus
        \sa SWI_STRUCT_DM_DL_INSTALL_STATUS  */
    SWI_NOTIFY_DM_DL_InstallStatus,

    /*! Indicates modem NVRAM update status.
        \sa SwiNVUpdateFromFile
        \sa SWI_STRUCT_NVUpdateFromFile  */
    SWI_NOTIFY_NVUpdateFromFile,
    
    /*! Indicates modem speaker volume status.
        \sa SwiGetSpeakerVolume
        \sa SwiSetSpeakerVolume  */    
    SWI_NOTIFY_SpkrVol,

    /*! Place holder.  Must be last entry in this list. */
    SWI_NOTIFY_END_NOTIFY  /* place holder - must be last entry in this list */
};


/**
 *
 * This structure is used for enabling multiple notifications simultaneously
 * using the SwiNotify() function call.
 *
 * This object is only supported on GSM devices.
 * 
 * \sa SwiNotify
 * \sa See SWI_TYPE_Notify for list of notifications.
 *
 */
struct SWI_STRUCT_NotifList
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application when calling SwiGetRadioPower
        or SwiSetRadioPower. */
    swi_uint32  sizeStruct;
    
    /*! The number of notifications events to enable.  This value must be 
        greater than 0. */
    swi_uint32  nNumNotif;      
    
    /*! Array of notification event types to be enabled.  The API will enable
        all events in the array starting with index 0 up to index nNumNotif-1.  */          
    enum SWI_TYPE_Notify    arrNotif[MAX_NOTIF_LIST];   
};

/**
 *
 * Notification variant.
 *
 * This structure contains information pertaining to a particular event
 * notification.  Depending on the notification type, the appropriate
 * user structure will be provided.  See SWI_TYPE_Notify for a complete list
 * of event notifications and its corresponding user structure.
 *
 * Event notifications are passed up to the host application
 * through callback function handlers previously registered with the 
 * SwiRegisterCallback command.
 *
 * \sa enum SWI_TYPE_Notify
 * \sa SwiRegisterCallback
 *
 */
struct SwiNotifyVariant
{
    enum SWI_TYPE_Notify  eNotify;

    union
    {
        /*
         * common 
         */
        struct SWI_STRUCT_AirServerChange   _AirServerChange;
/*       SWI_STRUCT_LockAirServer            _LockAirServer; */
/*        SWI_STRUCT_HeartBeat                _HeartBeat; */
/*        SWI_STRUCT_HeadsetIcon              _HeadsetIcon; - Not required until release 2 */
        struct SWI_STRUCT_Rssi              _Rssi;
        struct SWI_STRUCT_FwDownload        _FwDownload;
#if 0
        SWI_STRUCT_Reset                    _Reset;
        SWI_STRUCT_Power                    _Power;
        SWI_HW_STRUCT_Power                 _HWPower;
        SWI_STRUCT_Dhcp                     _Dhcp;
        SWI_STRUCT_PPP                      _PPP;
        SWI_STRUCT_Debug                    _Debug;
#endif
        struct SWI_STRUCT_RadioPower        _RadioPower;
        struct SWI_STRUCT_RadioTempState    _RadioTempState;
        struct SWI_STRUCT_RadioBattery      _RadioBattery;
        struct SWI_STRUCT_RadioBandList     _RadioBand;
#if 0

#endif
        struct SWI_STRUCT_DeviceTime        _DeviceTime;
#if 0

        /* MP notifications */
        SWI_STRUCT_Gps                      _Gps;
        SWI_STRUCT_ExtDioNotif              _ExtDio;
        SWI_STRUCT_ExtAiNotif               _ExtAi;
        SWI_STRUCT_GpsCommand               _GpsCommand;
        SWI_STRUCT_IgnSense                 _MPIgnSense;
        SWI_STRUCT_RfDownload               _RfDownload;
        SWI_STRUCT_SimSled                  _SimSled;
        SWI_STRUCT_BatteryStatus            _BatteryStatus;
        SWI_STRUCT_MpShutdown               _MpShutdown;
        SWI_STRUCT_MpConnect                _MpConnect;
        SWI_STRUCT_MpStartupDiag            _MpStartupDiag;
        SWI_STRUCT_MpStartupConfig          _MpStartupConfig;
        SWI_STRUCT_MpByteCounter            _MpByteCounter;
        SWI_STRUCT_MpDisconnect             _MpDisconnect;
        SWI_STRUCT_MpManagerData            _MpManagerData;

#endif
        /* LBS notifications */
        struct SWI_STRUCT_LbsPdData          _LbsPdData;
        struct SWI_STRUCT_LbsPdDone          _LbsPdDone;
        struct SWI_STRUCT_LbsPdEnd           _LbsPdEnd;
        struct SWI_STRUCT_LbsPdBegin         _LbsPdBegin;
        struct SWI_STRUCT_LbsPdUpdateFailure _LbsPdUpdateFailure;
        struct SWI_STRUCT_LbsPdGetStatus     _LbsPdGetStatus;
        struct SWI_STRUCT_LbsPdDloadBegin    _LbsPdDloadBegin;
        struct SWI_STRUCT_LbsPdDloadData     _LbsPdDloadData;
        struct SWI_STRUCT_LbsPdDloadDone     _LbsPdDloadDone;
        struct SWI_STRUCT_LbsPdDloadEnd      _LbsPdDloadEnd;
        struct SWI_STRUCT_LbsPaIpAddr        _LbsPaIpAddr;
        struct SWI_STRUCT_LbsPaGpsLock       _LbsPaGpsLock;
        struct SWI_STRUCT_LbsPaPtlmMode      _LbsPaPtlmMode;
        struct SWI_STRUCT_LbsPaPortId        _LbsPaPortId;
        struct SWI_STRUCT_LbsPaPrivacy       _LbsPaPrivacy;
        struct SWI_STRUCT_LbsPaNetAccess     _LbsPaNetAccess;
        struct SWI_STRUCT_LbsPaBsInfo        _LbsPaBsInfo;
        struct SWI_STRUCT_LbsPaWarmBegin     _LbsPaWarmBegin;
        struct SWI_STRUCT_LbsPaWarmStatus    _LbsPaWarmStatus;
        struct SWI_STRUCT_LbsPaWarmDone      _LbsPaWarmDone;
        struct SWI_STRUCT_LbsSatInfo         _LbsSatInfo;
        struct SWI_STRUCT_LbsNiReq           _LbsNiReq;
        struct SWI_STRUCT_LbsXtraAssist      _LbsXtraAssist;
        struct SWI_STRUCT_LbsXtraDloadTrigger _LbsXtraDloadTrigger;

        /*
         * CDMA 
         */
        SWI_STRUCT_ChannelNumber            _ChannelNumber;
        SWI_STRUCT_ChannelState             _ChannelState;
        SWI_STRUCT_ServiceIndication        _ServiceIndication;
        SWI_STRUCT_RoamingStatus            _RoamingStatus;
        SWI_STRUCT_Ecio                     _Ecio;
        SWI_STRUCT_AuthenticationStatus     _AuthenticationStatus;
        SWI_STRUCT_CallByteCounter          _CallByteCounter;
        SWI_STRUCT_CallConnecting           _CallConnecting;
        SWI_STRUCT_CallConnected            _CallConnected;
        SWI_STRUCT_CallIncoming             _CallIncoming;
        SWI_STRUCT_CallCallerID             _CallCallerID;
        SWI_STRUCT_CallExtendedCallerID     _CallExtendedCallerID;
        SWI_STRUCT_CallDisconnected         _CallDisconnected;
        SWI_STRUCT_CallError                _CallError;
        SWI_STRUCT_CDMA_SMS_Status          _CdmaSmsStatus;
        SWI_STRUCT_CDMA_SMS_SendStatus      _CdmaSmsSendStatus;
        SWI_STRUCT_Prev                     _Prev;
        /*SWI_STRUCT_HeadsetState             _HeadsetState;*/
        SWI_STRUCT_ActiveNam                _ActiveNam;
        SWI_STRUCT_TechSid                  _TechSid;
        SWI_STRUCT_TechNid                  _TechNid;
        SWI_STRUCT_TechBsInfo               _TechBsInfo;
        SWI_STRUCT_TechBandClass            _TechBandClass;
        SWI_STRUCT_TechRegZone              _TechRegZone;
        SWI_STRUCT_TechPacketZone           _TechPacketZone;
        SWI_STRUCT_TechPcsBlock             _TechPcsBlock;
        SWI_STRUCT_TechCellChannel          _TechCellChannel;
        SWI_STRUCT_SystemTime               _SystemTime;
        SWI_STRUCT_ActivationStatus         _ActivationStatus;
        SWI_STRUCT_V42BisEnabled            _V42BisEnabled;
        SWI_STRUCT_CallNotificationStatus   _CallNotificationStatus;
        SWI_STRUCT_CallDormant              _CallDormant;
        SWI_STRUCT_CallPrivacy              _CallPrivacy;    
        SWI_STRUCT_E911Mode                 _E911Mode;
        SWI_STRUCT_ModemTooHotIndication    _ModemTooHotIndication;
        SWI_STRUCT_OTASPState               _OTASPState;
        SWI_STRUCT_FlashBurn                _FlashBurn;
        SWI_STRUCT_ExcessSPCFailures        _ExcessSPCFailures;
        SWI_STRUCT_PowerSaveEnter           _PowerSaveEnter;
        SWI_STRUCT_PowerSaveExit            _PowerSaveExit;
        SWI_STRUCT_HdrNotificationStatus    _HdrNotificationStatus;
        SWI_STRUCT_Hdr_Acq                  _Hdr_Acq;
        SWI_STRUCT_Hdr_Rssi                 _Hdr_Rssi;
        SWI_STRUCT_Rssi_Ecio                _Rssi_Ecio;
        SWI_STRUCT_Hdr_Roam_Status          _Hdr_Roam_Status;
        SWI_STRUCT_Hdr_Srv_State            _Hdr_Srv_State;
        SWI_STRUCT_Hdr_Hybr_Pref            _Hdr_Hybr_Pref;
        SWI_STRUCT_Idle_Digital_Mode        _Idle_Digital_Mode;
        SWI_STRUCT_DDTM_Pref                _DDTM_Pref;
        SWI_STRUCT_Hdr_PowerSave_Enter      _Hdr_PowerSave_Enter;
        SWI_STRUCT_Hdr_PowerSave_Exit       _Hdr_PowerSave_Exit;
        SWI_STRUCT_TechHdrNotificationStatus _TechHdrNotificationStatus;
        SWI_STRUCT_Tech_Hdr_Prev            _Tech_Hdr_Prev;
        SWI_STRUCT_Tech_Hdr_Sector          _Tech_Hdr_Sector;
        SWI_STRUCT_Tech_Hdr_State            _Tech_Hdr_State;
        SWI_STRUCT_Tech_Hdr_Channel         _Tech_Hdr_Channel;
        SWI_STRUCT_Tech_Hdr_Session         _Tech_Hdr_Session;
        SWI_STRUCT_Tech_Hdr_An_Auth         _Tech_Hdr_An_Auth;
        SWI_STRUCT_Tech_Hdr_Drc_Val         _Tech_Hdr_Drc_Val;
        SWI_STRUCT_Tech_Hdr_Drc_Cover       _Tech_Hdr_Drc_Cover;
        SWI_STRUCT_Tech_Hdr_Rri             _Tech_Hdr_Rri;
        SWI_STRUCT_Tech_Hdr_Stats           _Tech_Hdr_Stats;
        SWI_STRUCT_DebugPacket              _DebugPacket;
        SWI_STRUCT_HIOTAStatus              _HIOTA_Status;
        SWI_STRUCT_RuimStatus               _RuimStatus;
        SWI_STRUCT_IOTA_State               _EMIOTAStauts;
        SWI_STRUCT_IOTA_WAPPush             _EMIOTAWAPPush;

        /* Rev A */
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Stats    _Tech_Hdr_Su_Fwd_Stats;
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt  _Tech_Hdr_Su_Fwd_Slot_Cnt;
        SWI_STRUCT_Tech_Hdr_Su_Fwd_Slot_Cnt2 _Tech_Hdr_Su_Fwd_Slot_Cnt2;
        SWI_STRUCT_Tech_Hdr_Mu_Fwd_Stats     _Tech_Hdr_Mu_Fwd_Stats;
        SWI_STRUCT_Tech_Hdr_Personality      _Tech_Hdr_Personality;
        SWI_STRUCT_Tech_Hdr_Mac_Index       _Tech_Hdr_Mac_Index;

        /*
         * GSM 
         */
        struct SWI_STRUCT_GSM_NetworkStatus   _NetworkStatus;
#if 0
        SWI_STRUCT_Temperature              _Temperature;
        SWI_STRUCT_GSM_SMS_Status           _GsmSmsStatus;
        SWI_STRUCT_SMS_SimFull              _SmsSimFull;
        SWI_STRUCT_Band                     _Band;
        SWI_STRUCT_Band                     _BandWrite;
#endif
        struct SWI_STRUCT_PktSessionStatus  _PktSessionStatus;
        struct SWI_STRUCT_Registration      _Registration;
        struct SWI_STRUCT_PktSessionCall    _PktSessionCall;
#if 0
        SWI_STRUCT_TransmitAlert            _TransmitAlert;
        SWI_STRUCT_SMS_SendReference        _SmsSendReference;
        SWI_STRUCT_GSM_SMS_SendStatus       _GsmSmsSendStatus;
#endif 
        struct SWI_STRUCT_SIM_StatusExp     _SimStatusExp;
        struct SWI_STRUCT_CALL_EXP_Progress _CallProgress;
        struct SWI_STRUCT_CALL_Status       _CallStatus;
        struct SWI_STRUCT_ByteCounter       _ByteCounter;
#if 0
        SWI_STRUCT_SmsReady                 _SmsReady;
#endif
        struct SWI_STRUCT_AudioProfile      _AudioProfile;
        struct SWI_STRUCT_MissedCall        _MissedCall;
        struct SWI_STRUCT_PhonebookReady    _PhonebookReady;
        struct SWI_STRUCT_PlmnAvailable     _PlmnAvailable;
        struct SWI_STRUCT_PlmnMode          _PlmnMode;
        struct SWI_STRUCT_ModemDisabled     _ModemDisabled;
        struct SWI_STRUCT_SS_PasswordStatus _SSPasswordStatus;
        struct SWI_STRUCT_SS_RespOK         _SSRespOK;
        struct SWI_STRUCT_SS_RespFwdInfo    _SSRespFwdInfo;
        struct SWI_STRUCT_SS_RespCallBar    _SSRespCallBar;
        struct SWI_STRUCT_SS_RespInfoData   _SSRespInfoData;
        struct SWI_STRUCT_SS_RespStatus     _SSRespStatus;
        struct SWI_STRUCT_SS_RespFwdNum     _SSRespFwdNum;
        struct SWI_STRUCT_SS_RespBSList     _SSRespBSList;
        struct SWI_STRUCT_SS_RespListFwdInfo _SSRespFwdList;
        struct SWI_STRUCT_SS_RespRaw        _SSRespRaw;
        struct SWI_STRUCT_SS_RespPassword   _SSRespPassword;
        struct SWI_STRUCT_SS_RespClir       _SSRespClir;
        struct SWI_STRUCT_SS_RespError      _SSRespError;
        struct SWI_STRUCT_SS_RespReject     _SSRespReject;
        struct SWI_STRUCT_SS_RespCancel     _SSRespCancel;
        struct SWI_STRUCT_SS_RespRelease    _SSRespRelease;
        struct SWI_STRUCT_SSNotify          _SSNotify;
        struct SWI_STRUCT_PlmnReady         _PlmnReady;
        struct SWI_STRUCT_VoiceMessage      _VoiceMessage;
        struct SWI_STRUCT_CFIcon            _CFIcon;
#if 0
        SWI_STRUCT_IconGprsState            _IconGprsState;
        SWI_STRUCT_IconRoamState            _IconRoamState;
#endif
        struct SWI_STRUCT_CallForwarded     _CallForwarded;
        struct SWI_STRUCT_STK_Notif_Command _Notif_STKCommand;
#if 0
        SWI_STRUCT_DataRate                 _DataRate;
#endif
        struct SWI_STRUCT_PHONEBK_Entry     _PhonebookEntry;
#if 0
        SWI_STRUCT_SmsStoreExp              _SmsStoreExp;
        SWI_STRUCT_SmsSendExp               _SmsSendExp;
#endif
        struct SWI_STRUCT_ServiceIcon       _ServiceIcon;
#if 0
        SWI_STRUCT_TTYControl               _TTYControl;
        SWI_STRUCT_SMS_Received             _SmsReceived;
        SWI_STRUCT_SMS_Deliver              _SmsDeliver;
        SWI_STRUCT_SMS_Submit               _SmsSubmit;
        SWI_STRUCT_SMS_StatusReport         _SmsStatusReport;
        SWI_STRUCT_SMS_SimStatus            _SmsSimStatus;
#endif
        struct SWI_STRUCT_UnstructSupplServiceExp  _USSDExp;  
        struct SWI_STRUCT_NvStore           _NvStore;
        struct SWI_STRUCT_NVUpdateFromFile  _NVUpdateFromFile;
#if 0
        SWI_STRUCT_HSDPA_TestChan           _HSDPA_TestChan;
        SWI_STRUCT_SmsStatusReportCfg       _SmsStsReportCfg;
#endif
        struct SWI_STRUCT_GsmSystemStatus   _GsmSystemStatus;
        struct SWI_STRUCT_RscpEcIo          _RscpEcIo;
        struct SWI_STRUCT_GsmCellInfo       _GsmCellInfo;
        struct SWI_STRUCT_GsmProfileChange  _GsmProfileChange;
        struct SWI_STRUCT_GsmDefaultChange  _GsmDefaultChange;

        /* OMA-DM */
        SWI_STRUCT_OMADM_GET_CONFIG         _OMADM_GetConfig;
        SWI_STRUCT_OMADM_SESSION_STATE      _OMADM_SessionState;    
        SWI_STRUCT_OMADM_NI_ALERT           _OMADM_NIAlert;
        SWI_STRUCT_OMADM_UI_ALERT_DISPLAY   _OMADM_UIAlertDisplay;
        SWI_STRUCT_OMADM_UI_ALERT_CONFIRM   _OMADM_UIAlertConfirm;
        SWI_STRUCT_OMADM_UI_ALERT_INPUT     _OMADM_UIAlertInput;
        SWI_STRUCT_OMADM_UI_ALERT_CHOICE    _OMADM_UIAlertChoice;
        
        /* DM-DL */
        SWI_STRUCT_DM_DL_PROGRESS           _DMDL_Progress;
        SWI_STRUCT_DM_DL_ALERT              _DMDL_Alert;
        SWI_STRUCT_DM_DL_INSTALL_STATUS     _DMDL_InstallStatus;
    };
};

/*************
 *
 * Callback
 *
 ************/
typedef void (*SWI_CALLBACK_EX)(struct SwiNotifyVariant *pNotify);


/* function prototypes */
/* RILSTART */
extern SWI_API SWI_RCODE SwiCreateNotifThread( void (*threadfuncp)(void) );
/* RILSTOP */

extern SWI_API SWI_RCODE SwiWaitNotification( void );

extern SWI_API SWI_RCODE SwiRegisterCallback(SWI_CALLBACK_EX pCallback, 
                                             swi_uint64 nNotifications);

extern SWI_API SWI_RCODE SwiDeRegisterCallback(SWI_CALLBACK_EX pCallback);

extern SWI_API SWI_RCODE SwiNotify(enum SWI_TYPE_Notify eNotify, 
                                   swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiStopNotify(enum SWI_TYPE_Notify eNotify, 
                                       swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWI_API_NOTIFY_H__ */

/* 
 * $Log: SwiApiNotify.h,v $
 */
