/**
 *    \if CvsId 
 *    $Id: SwiApiCmBasic.h,v 1.33 2010/08/24 20:31:55 mdejong Exp $
 *    \endif
 *    \ingroup cmbasic
 *    \file SwiApiCmBasic.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiCmBasic.c file
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPI_CM
#define __SWIAPI_CM

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"


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
#define LENGTH_ProductStr                   16
#define MAX_OBJRNGINFOBLOCKS                35
#define LENGTH_PortName                     80

/* enum declarations */

/**
 *
 * An enumeration of supported device types.
 *
 *   Includes both GSM and CDMA PC cards, embedded devices, and MP line
 *   of products.
 *
 *   \sa   SWI_STRUCT_AirServer
 *   \sa See sample code, ConnectApi
 *
 */
enum SWI_TYPE_Device
{
      SWI_DEVICE_None         = 0,  /*!< Unsupported device */
 
      /* PC Cards */
      SWI_DEVICE_AC550,             /*!< AirCard 550 (CDMA) */
      SWI_DEVICE_AC575,             /*!< AirCard 575 (CDMA) */
      SWI_DEVICE_AC580,             /*!< AirCard 580 (CDMA) */
      SWI_DEVICE_AC595,             /*!< AirCard 595 (CDMA) */
      SWI_DEVICE_AC597,             /*!< AirCard 597 (CDMA) */
      SWI_DEVICE_AC710  = 100,      /*!< AirCard 710 (GSM) */
      SWI_DEVICE_AC750,             /*!< AirCard 750 (GSM) */
      SWI_DEVICE_AC775,             /*!< AirCard 775 (GSM) */
      SWI_DEVICE_AC850,             /*!< AirCard 850 (GSM) */
      SWI_DEVICE_AC860,             /*!< AirCard 860 (GSM) */
      SWI_DEVICE_AC875,             /*!< AirCard 875 (GSM) */
      SWI_DEVICE_AC880,             /*!< AirCard 880 (GSM) */
      SWI_DEVICE_AC881,             /*!< AirCard 881 (GSM) */

      /* Embedded Modules */
      SWI_DEVICE_MC5720 = 200,      /*!< MC5720 (CDMA) */
      SWI_DEVICE_MC5725,            /*!< MC5725 (CDMA) */
      SWI_DEVICE_MC5727,            /*!< MC5727 (CDMA) */
      SWI_DEVICE_MC5728,            /*!< MC5728 (CDMA) */
      SWI_DEVICE_SL501X,            /*!< SL5010 and SL5011 (CDMA) */
      SWI_DEVICE_MC8755 = 300,      /*!< MC8775 (GSM) */
      SWI_DEVICE_MC8765,            /*!< MC8765 (GSM) */
      SWI_DEVICE_MC8775,            /*!< MC8775 (GSM) */
      SWI_DEVICE_MC8780,            /*!< MC8780 (GSM) */
      SWI_DEVICE_MC8781,            /*!< MC8781 (GSM) */
      SWI_DEVICE_MC8785,            /*!< MC8785 (GSM) */
      SWI_DEVICE_MC8790,            /*!< MC8790 (GSM) */
      SWI_DEVICE_MC8791,            /*!< MC8791 (GSM) */
      SWI_DEVICE_MC8700,            /*!< MC8700 (GSM) */
      SWI_DEVICE_MC8704,            /*!< MC8704 (GSM) */
      SWI_DEVICE_MC8705,            /*!< MC8705 (GSM) */
      SWI_DEVICE_MC8777,            /*!< MC8777 (GSM) */
      SWI_DEVICE_MC8790V,           /*!< MC8790V (GSM) */
      SWI_DEVICE_MC8801,            /*!< MC8801 (GSM) */
      SWI_DEVICE_SL8080,            /*!< SL8080 (GSM) */
      SWI_DEVICE_SL8081,            /*!< SL8081 (GSM) */
      SWI_DEVICE_SL8082,            /*!< SL8082 (GSM) */
      SWI_DEVICE_SL8083,            /*!< SL8083 (GSM) */
      SWI_DEVICE_SL8084,            /*!< SL8084 (GSM) */
      SWI_DEVICE_SL8085,            /*!< SL8085 (GSM) */
      SWI_DEVICE_SL8090,            /*!< SL8090 (GSM) */ 
      SWI_DEVICE_SL8091,            /*!< SL8091 (GSM) */
      SWI_DEVICE_SL8092,            /*!< SL8092 (GSM) */
      SWI_DEVICE_SL8092RD,          /*!< SL8092RD (GSM) */
      SWI_DEVICE_SL8093,            /*!< SL8093 (GSM) */
      SWI_DEVICE_EM5625 = 400,      /*!< EM5625 (CDMA) */
      SWI_DEVICE_EM5725,            /*!< EM5725 (CDMA) */
      SWI_DEVICE_EM8780 = 500,      /*!< EM8780 (GSM) */
      SWI_DEVICE_EM8781,            /*!< EM8781 (GSM) */
      SWI_DEVICE_M81A,              /*!< M81A (GSM) */
      SWI_DEVICE_M81B,              /*!< M81B (GSM) */
      
      /* MP */
      SWI_DEVICE_MP555  = 600,      /*!< MP555 (CDMA) */
      SWI_DEVICE_MP595,             /*!< MP595 (CDMA) */
      SWI_DEVICE_MP750  = 700,      /*!< MP750 (GSM) */
      SWI_DEVICE_MP775,             /*!< MP775 (GSM) */
      SWI_DEVICE_MP875,             /*!< MP875 (GSM) */
      SWI_DEVICE_MP880,             /*!< MP880 (GSM) */
      SWI_DEVICE_MP881,             /*!< MP881 (GSM) */

      /* ExpressCards */
      SWI_DEVICE_AC597E = 1000,     /*!< AirCard 595E (CDMA) */
      SWI_DEVICE_AC598E,            /*!< AirCard 598E (CDMA) */
      SWI_DEVICE_AC875E = 1100,     /*!< AirCard 875E (GSM) */
      SWI_DEVICE_AC880E,            /*!< AirCard 880E (GSM) */
      SWI_DEVICE_AC881E,            /*!< AirCard 881E (GSM) */
      SWI_DEVICE_AC885E,            /*!< AirCard 885E (GSM) */
      SWI_DEVICE_AC886E,            /*!< AirCard 886E (GSM) */

      /* USB Modems */
      SWI_DEVICE_AC595U = 1200,     /*!< AirCard 595U (CDMA) */
      SWI_DEVICE_T598,              /*!< Tiger 598 (CDMA) */
      SWI_DEVICE_T11,               /*!< Tiger 11 (CDMA) */
      SWI_DEVICE_AC802,             /*!< Owl (CDMA) */
      SWI_DEVICE_AC875U = 1300,     /*!< AirCard 875U (GSM) */
      SWI_DEVICE_AC880U,            /*!< AirCard 880U (GSM) */
      SWI_DEVICE_AC881U,            /*!< AirCard 881U (GSM) */
      SWI_DEVICE_C885,              /*!< Compass 885 (GSM) */
      SWI_DEVICE_C886,              /*!< Compass 886 (GSM) */
      SWI_DEVICE_C888,              /*!< Compass 888 (GSM) */
      SWI_DEVICE_C889,              /*!< Compass 889 (GSM) */
      SWI_DEVICE_K888,              /*!< Karbon 888 (GSM) */
      SWI_DEVICE_K889,              /*!< Karbon 889 (GSM) */
      SWI_DEVICE_C22,               /*!< Compass 22 (GSM) */
      SWI_DEVICE_C23,               /*!< Compass 33 (GSM) */
      SWI_DEVICE_K22,               /*!< Karbon 22 (GSM) */
      SWI_DEVICE_K23,               /*!< Karbon 23 (GSM) */
      SWI_DEVICE_USB303,            /*!< USB 303 (GSM) */
      SWI_DEVICE_USB304,            /*!< USB 304 (GSM) */
      SWI_DEVICE_USB305,            /*!< USB 305 (GSM) */
      SWI_DEVICE_USB306,            /*!< USB 306 (GSM) */
      SWI_DEVICE_USB307,            /*!< USB 307 (GSM) */
      SWI_DEVICE_USB308,            /*!< USB 308 (GSM) */
      SWI_DEVICE_CHspa  = 1399,     /*!< Compass HSPA (GSM) */

      /* Mass Storage */
      SWI_DEVICE_MassStorage  = 2000,   /*!< Mass storage device */
      
      /* Other - This value is used for determining if a Product string is
       * provide instead of the usual Device type for newer modems using 
       * unified VID/PID. The API will translate the string to a device type.
       */
      SWI_DEVICE_UNIFIEDPID         /*!< This must be the last item */
};

/* structure declarations */

/**
 *
 * This structure contains logging information. 
 *
 */
struct SWI_STRUCT_Logging
{
    /*! size of this structure */
    swi_uint32     sizeStruct;

    /*! log file name (do not include path) */
    swi_char  szFileName[LENGTH_LogFileName];

    /*! set to true to enable API logging, false otherwise */
    swi_bool           bEnable;
};

/**
 *
 * This structure contains logging information.  The SDK Pathname 
 * includes the executable's filename 
 *
 */
struct SWI_STRUCT_SdkPath
{
    /*!  size of this structure */
    swi_uint32     sizeStruct;

    /*! Pathname of the SDK executable in ASCII */
    swi_char  szSdkPath[LENGTH_SdkPathName];

    /*! Set TRUE if the structure contains a valid pathname
        FALSE if default pathname should be used instead */
    swi_bool           bEnable;
};

/**
 *
 * This structure is used as input for SwiApiStartup and contains
 * information needed to configure the API sub-system.
 *
 * Notes:   None
 *
 */
struct SWI_STRUCT_ApiStartup
{
 
    /*!< size of this structure */
    swi_uint32                    sizeStruct;
 
    /*!< logging parameters */
    struct SWI_STRUCT_Logging     sLogging;

    /*!< Pathname for the SDK process executable */
    struct SWI_STRUCT_SdkPath     sSdkPath;
};


/**
 *
 * This structure contains USB descriptor information. 
 *
 * Only GSM and CDMA USB based PC cards and embedded modules are supported.  
 *
 */
struct SWI_STRUCT_UsbdInfo
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! A NULL terminated string containing the USB descriptor version.
        Maximum length of string is LENGTH_UsbdVersion. */
    swi_char szVersion[LENGTH_UsbdVersion];

    /*! A NULL terminated string containing the USB descriptor build date.
        Maximum length of string is LENGTH_UsbdBuildDate. */
    swi_char szBuildDate[LENGTH_UsbdBuildDate];
};

/**
 *
 * This structure contains modem PRI information.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SwiGetPriInfo
 *
 */
struct SWI_STRUCT_PriInfo
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application when calling SwiGetRadioPower
        or SwiSetRadioPower. */
    swi_uint32  sizeStruct;

    /*! Major PRI version. */
    swi_uint32  nMajorVer;

    /*! Minor version. */
    swi_uint32  nMinorVer;

    /*! Sub-minor version (0xFFFF if unused). */
    swi_uint32 nSubMinorVer;

    /*! Sub-OEM version (0xFFFF if unused). */
    swi_uint32 nSubOemVer;

    /*! SKU ID or part number. */   
    swi_uint32 nSkuId;
};

/**
 *
 * This structure contains modem type information
 * 
 * \sa SwiGetModemType
 *
 */
struct SWI_STRUCT_ModemType
{
    /*! The size, in bytes, of this structure.  This value must always be 
    populated by the calling application. */
    swi_uint32 sizeStruct;          

    /*! Modem type */
    swi_uint16 nModemType;

    /*! A Mobile wireless technology */
    swi_uint32 nWirelessTech;

    /*! A NULL terminated string which contains the Sierra Wireless product
        name.  Length of value is restricted to LENGTH_ProductName. */
    swi_char szProductName[LENGTH_ProductStr];
};

/**
 *
 *  An enumeration of wireless mobile technologies
 *
 *  Wireless technologies including CDMA and GSM
 * 
 *  \sa SWI_STRUCT_AirServerExtended
 *  \sa See sample code, ConnectApi
 *
 */
enum SWI_TYPE_WirelessTech
{
    SWI_WIRELESSTECH_Unknown,
    SWI_WIRELESSTECH_Cdma,
    SWI_WIRELESSTECH_Gsm,
    SWI_WIRELESSTECH_Wifi,
    SWI_WIRELESSTECH_Wimax
};

/**
 *
 *  An enumeration of product classes
 *
 * Product classes are broken into the following categories PC Cards 
 * (PCMCIA, express cards, USB), embedded modules (MC, EM, M81), and MP.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_STRUCT_AirServerExtended
 * \sa See sample code, ConnectApi
 *
 */
enum SWI_TYPE_ProductClass
{
    SWI_PRODUCTCLASS_Unknown,
    SWI_PRODUCTCLASS_PcCard,
    SWI_PRODUCTCLASS_Embedded,
    SWI_PRODUCTCLASS_Mp
};

/**
 *
 * This structure contains basic air server information.
 *
 *  This structure is used by SwiGetAvailAirServers to return the list
 *  of available air servers (devices) on the system.  Similarly,
 *  SwiSelectAirServer uses this structure to bind to an air server.
 *  SWI_NOTIFY_AirServer uses this structure to signal the availability
 *  of an air server.
 *
 *  Supports both GSM and CDMA PC cards, embedded devices, and MP line
 *  of products. 
 *
 *  \sa SWI_TYPE_Device
 *  \sa SWI_NOTIFY_AirServerChange
 *  \sa SwiGetAvailAirServers
 *  \sa SwiSelectAirServer
 *  \sa See sample code, ConnectApi
 *
 */
struct SWI_STRUCT_AirServer
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;     

    /*! MAC address of local adapter through which we access the air server.
        This field mainly applies to MP devices.  When calling 
        SwiSelectAirServer, simply provide the value returned from 
        SwiGetAvailableAirServers or SWI_NOTIFY_AirServer.*/
    swi_uint8 ClientMac[LENGTH_MacAddress];   

    /*! MAC address of the air server.  When calling SwiSelectAirServer,
        simply provide the value returned from SwiGetAvailableAirServers
        or SWI_NOTIFY_AirServer.*/
    swi_uint8  ServerMac[LENGTH_MacAddress];   

    /*! Device of interest. */
    enum SWI_TYPE_Device eDevice;    
};

/**
 *
 *  This structure contains extended air server information
 *
 *  This structure is used by SwiGetAirServerInfo to retrieve additional
 *  information about a particular air server.
 *
 *  Supports both GSM and CDMA PC cards, embedded devices, and MP line
 *  of products.
 *
 * See Also
 *
 *  \sa SWI_TYPE_ProductClass
 *  \sa SWI_TYPE_WirelessTech
 *  \sa SwiGetAirServerInfo
 *  \sa See sample code, ConnectApi
 *
 */
struct SWI_STRUCT_AirServerExtended
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! A NULL terminated string which contains the Sierra Wireless product
        name.  Length of value is restricted to LENGTH_ProductName. */
    swi_char szProductName[LENGTH_ProductName];

    /*! A Product class enumeration.  See SWI_TYPE_ProductClass for list
        of possible values. */
    enum SWI_TYPE_ProductClass   eProductClass;

    /*! A Mobile wireless technology enumeration.  See SWI_TYPE_WirelessTech 
        for list of possible values. */
    enum SWI_TYPE_WirelessTech eWirelessTech;
        
    /*! A NULL terminated string which contains the friendly dial-up modem 
        name.  This is the same value listed under Device Manager->Modems. */   
    swi_char szModemName[LENGTH_ModemName];
    
    /*! A NULL terminated string which contains the hardware ID of the
        network adapter interface. */
    swi_char szHardwareId[LENGTH_HardwareId];
};

/**
 *
 * This structure contains the number of available air servers.  
 * It also lists the available air servers.
 *
 *
 */
struct SWI_STRUCT_AirServerList
{
   /*!< size of this structure */
    swi_uint32                     sizeStruct;

   /*!< number of available air servers */
    swi_uint32                     nNumber;

   /*!< list of available air servers */
    struct SWI_STRUCT_AirServer    AirServers[MAX_AIR_SERVERS];
};

/**
 *
 * This structure is used by the SWI_NOTIFY_AirServerChange 
 * notification
 *
 */
struct SWI_STRUCT_AirServerChange
{
   /*!< size of this structure */
    swi_uint32                  sizeStruct;                    

   /*!< air server */
    struct SWI_STRUCT_AirServer AirServer;

   /*!< true if device inserted, false otherwise */
    swi_bool                    bInsert;
};

/**
 * Enumeration of device information types.
 * 
 * Used by SWI_STRUCT_DeviceInformation_Request and 
 * SWI_STRUCT_DeviceInformation_Response to indicate the type of data being
 * requested in SwiGetDeviceInformation.
 *
 * \sa SWI_STRUCT_DeviceInformation_Request
 * \sa SWI_STRUCT_DeviceInformation_Response
 * \sa SwiGetDeviceInformation
 *
 */
enum SWI_TYPE_DeviceInformationType
{    
    SWI_DEVICEINFORMATION_Technology = 1, /*!< Technology type (CDMA, GPRS) */    
    SWI_DEVICEINFORMATION_Product,        /*!< Product string */
    SWI_DEVICEINFORMATION_FrmDwlProtocol, /*!< Firmware download protocol */
    SWI_DEVICEINFORMATION_DeviceReEnum,    /*!< Device re-enumeration */
    SWI_DEVICEINFORMATION_CNSsupported    /*!< CNS supported by FW application */
};

/**
 * Enumeration indicates return status of request.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * */
enum SWI_TYPE_RequestStatus
{    
    SWI_REQUESTSTATUS_SUCCESS,
    SWI_REQUESTSTATUS_UnknownType,
    SWI_REQUESTSTATUS_GenFailure
};

/**
 * Enumeration of device CNS object technology range.
 * 
 * Returned by SwiGetDeviceInformation in conjunction with 
 * SWI_DEVICEINFORMATION_Technology information request.
 *
 * \sa SWI_TYPE_DeviceInformationType
 * \sa SwiGetDeviceInformation
 *
 */
enum SWI_TYPE_Technology
{    
    SWI_TECHNOLOGY_Unknown,
    SWI_TECHNOLOGY_CDMA,
    SWI_TECHNOLOGY_GSM
};

/**
 * Enumeration of protocols to be used for firmware download.
 * 
 * Returned by SwiGetDeviceInformation in conjunction with 
 * SWI_DEVICEINFORMATION_FrmDwlProtocol information request.
 *
 * \sa SWI_TYPE_DeviceInformationType
 * \sa SwiGetDeviceInformation
 *
 */
enum SWI_TYPE_FrmDlwProtocol
{
    /*! 0x00  - Host must use firmware download protocol 1 to download images 
        to this modem */    
    SWI_FRMDLWPROTOCOL_One,

    /*! 0x01  - Host must use firmware download protocol 2 to download images 
        to this modem */    
    SWI_FRMDLWPROTOCOL_Two  
};

/**
 *
 * Enumeration of protocols to be used for firmware download.
 * 
 * Returned by SwiGetDeviceInformation in conjunction with 
 * SWI_DEVICEINFORMATION_DeviceReEnum information request.
 *
 * \sa SWI_TYPE_DeviceInformationType
 * \sa SwiGetDeviceInformation
 *
 */
enum SWI_TYPE_DeviceReEnum
{
    /*! 0x00  - Modem reset causes PnP device removal/arrival or media 
        disconnect/reconnect notifications on the host platform */
    SWI_DEVICEREENUM_Trigger_Remove,

    /*! 0x01  - Modem reset does not trigger PnP device removal/arrival or 
        media disconnect/reconnect notifications on the host platform */    
    SWI_DEVICEREENUM_Trigger_NoRemove
};
/**
 *
 * CNS supported by FW application flag.
 * 
 * Returned by SwiGetDeviceInformation in conjunction with 
 * SWI_DEVICEINFORMATION_CNSsupported information request.
 *
 * \sa SWI_TYPE_DeviceInformationType
 * \sa SwiGetDeviceInformation
 *
 */
enum SWI_TYPE_CNSsupported
{
    /*! 0x00  - Modem reset causes PnP device removal/arrival or media 
        disconnect/reconnect notifications on the host platform */
    SWI_CNSSUPPORTED_Yes,

    /*! 0x01  - Modem reset does not trigger PnP device removal/arrival or 
        media disconnect/reconnect notifications on the host platform */    
    SWI_CNSSUPPORTED_No
};
/**
 *
 *  This structure sets the context for a device information request.
 *
 *  This structure is used by SwiGetDeviceInformation to retrieve
 *  fundamental information about the stack's physical device.
 *
 * See Also
 *
 *  \sa SwiGetDeviceInformation
 *
 */
struct SWI_STRUCT_DeviceInformation_Request
{   
    /*! The size, in bytes, of this structure.  This value must always be 
     *  populated by the calling application. 
     */    
    swi_uint32                          sizeStruct;
    /*! Payload version. */
    swi_uint32                          Version;
    /*! Device information request type. */
    enum SWI_TYPE_DeviceInformationType eDeviceInformationType; 
};

/**
 *
 * This structure contains the response for a device information request.
 *
 * This structure is used by SwiGetDeviceInformation to retrieve
 * fundamental information about the stack's physical device.
 *
 * See Also
 *
 * \sa SwiGetDeviceInformation
 *
 */
struct SWI_STRUCT_DeviceInformation_Response
{   
    /*! The size, in bytes, of this structure. */
    swi_uint32                          sizeStruct;
    /*! Payload version. */    
    swi_uint32                          Version;

    /*! Request status. */
    enum SWI_TYPE_RequestStatus         eRequestStatus;

    /*! Device information request type. */
    enum SWI_TYPE_DeviceInformationType eDeviceInformationType;
    /*! Length of the value in bytes. */
    swi_uint32                          nLength;

    /*! Valid if eDeviceInformationType = SWI_DEVICEINFORMATION_Technology. */
    enum SWI_TYPE_Technology            eTechnology;

    /*! Valid if eDeviceInformationType = SWI_DEVICEINFORMATION_Product. */
    swi_char                            szProduct[LENGTH_ProductStr];

    /*! Valid if eDeviceInformationType = SWI_DEVICEINFORMATION_FrmDwlProtocol.*/
    enum SWI_TYPE_FrmDlwProtocol        eFrmDlwProtocol;
    /*! Valid if eDeviceInformationType = SWI_DEVICEINFORMATION_DeviceReEnum. */
    enum SWI_TYPE_DeviceReEnum          eDeviceReEnum;

/*! Valid if eDeviceInformationType = SWI_DEVICEINFORMATION_CNSsupported. */
    enum SWI_TYPE_CNSsupported          eCNSsupported;
};

/**
 *
 * This structure is used by the SWI_NOTIFY_HeartBeat notification. 
 *
 */
struct SWI_STRUCT_HeartBeat
{
   /*!< size of this structure */
    swi_uint32    sizeStruct;            
   
};

/**
 *
 * Enumeration of months of a year
 *
 * \sa SWI_STRUCT_DeviceTime
 *
 */
enum SWI_TYPE_Month
{
    SWI_MONTH_Jan   = 0x01, /*!< January */
    SWI_MONTH_Feb,          /*!< February */
    SWI_MONTH_Mar,          /*!< March */
    SWI_MONTH_Apr,          /*!< April */
    SWI_MONTH_May,          /*!< May */
    SWI_MONTH_Jun,          /*!< June */
    SWI_MONTH_Jul,          /*!< July */
    SWI_MONTH_Aug,          /*!< August */
    SWI_MONTH_Sep,          /*!< September */
    SWI_MONTH_Oct,          /*!< October */
    SWI_MONTH_Nov,          /*!< November */
    SWI_MONTH_Dec           /*!< December */
};

/**
 *
 * Enumeration of days of the week.
 *
 * \sa SWI_STRUCT_DeviceTime
 *
 */
enum SWI_TYPE_WeekDay
{
    SWI_WEEKDAY_Mon,    /*!< Monday */
    SWI_WEEKDAY_Tue,    /*!< Tuesday */
    SWI_WEEKDAY_Wed,    /*!< Wednesday */
    SWI_WEEKDAY_Thu,    /*!< Thursday */
    SWI_WEEKDAY_Fri,    /*!< Friday */
    SWI_WEEKDAY_Sat,    /*!< Saturday */
    SWI_WEEKDAY_Sun     /*!< Sunday */
};

/**
 *
 * Enumeration of daylight savings.
 *
 * \sa SWI_STRUCT_DeviceTime   
 *
 */
enum SWI_TYPE_DaylightSavings
{
    SWI_DAYLIGHTSAVINGS_None,           /*!< No daylight savings */
    SWI_DAYLIGHTSAVINGS_PlusOneHour,    /*!< Plus one hour */
    SWI_DAYLIGHTSAVINGS_PlusTwoHour     /*!< Plus two hours */
};

/**
 *
 * This structure contains device time information.
 *
 * \sa SwiGetDeviceTime    
 *
 */
struct SWI_STRUCT_DeviceTime
{
    /*!< The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*!< Numerical value representing the year. */
    swi_uint32 nYear;
    
    /*!< Month enumeration.  */
    enum SWI_TYPE_Month eMonth;
    
    /*!< Numerical value representing the day of the month.  Valid values
        range between 1-31. */
    swi_uint32 nDay;
    
    /*!< Day of the week enumeration. */
    enum SWI_TYPE_WeekDay eWeekDay;

    /*!< Numerical value representing the hour.  Valid values range between
        0-23 in UTC. */
    swi_uint32 nHour;

    /*!< Numerical value representing minutes.  Valid values range between
        0-59 in UTC. */
    swi_uint32 nMinute;

    /*!< Numerical value representing seconds.  Valid values range between
        0-59 in UTC. */
    swi_uint32 nSecond;

    /*!< Numerical value representing the time zone.  Values range from -48
        to 48 where 1 equals 15 minutes.  For example:
            - -4 is UTC minus 60 minutes
            - -3 is UTC minus 45 minutes
            - -2 is UTC minus 30 minutes
            - -1 is UTC minus 15 minutes
            - 0 is UTC time
            - 1 is UTC plus 15 minutes
            - 2 is UTC plus 30 minutes */
    swi_int32 nTimeZone;

    /*!< Daylight saving enumeration.  See SWI_TYPE_DaylightSavings for more
        details. */
    enum SWI_TYPE_DaylightSavings eDaylightSavings;
};

#define LENGTH_ImgInfoVer       85  /*!< Maximum verion info length */
#define LENGTH_ImgInfoRelDate   9   /*!< Maximum release date info length */
#define LENGTH_ImgInfoProdType  5   /*!< Maximum product type info length */
#define LENGTH_ImgInfoImgType   5   /*!< Maximum image type infor length */ 

/**
 *
 * An enumeration of flash image result codes.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_STRUCT_FlashImgInfo
 *
 */
typedef enum 
{
    FLASHIMGINFORESULT_Success,             /*!< Action succeeded */
    FLASHIMGINFORESULT_UnsupportedImgType,  /*!< Unsupported image type */
    FLASHIMGINFORESULT_ImgNotFound         /*!< Image not found */
} TYPE_FlashImgInfo_Result;

/**
 *
 * This structure contains flash image information.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa TYPE_FlashImgInfo_Result
 * \sa SwiGetFlashImgInfo
 *
 */
struct SWI_STRUCT_FlashImgInfo
{
    /*!< The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_int32   sizeStruct;
    
    /*!< This value is case sensitive and must be supplied by the calling
        application.  Possible values include:
        - 'QCOM' - Boot images including NAND image partition table, PBL 
          (Primary Boot Loader), SBL (Secondary Boot Loader), OEMSBL (OEM
          Secondary Boot Loader) and the configuration data used by PBL)
        - 'BOOT' - OEMSBL (SWI Boot Loader)
        - 'APPL' - Application code 
        - 'USBD' - USB descriptor table
        - 'SWOC' - Software on card image */
    swi_char szImageType[LENGTH_ImgInfoImgType];
    
    /*!< An enumeration of type TYPE_FlashImgInfo_Result returned from the
        device which specifies the result of the request. */
    TYPE_FlashImgInfo_Result eResult;
    
    /*!< A NULL terminated string returned from the device which specifies the 
        version of the flash image of interest.  The format of this string
        varies between technology types and image types:
        - CDMA QCOM, BOOT, APPL: 
            - <product>_<build_type>[_<pid>].<major>.<minor>[.<point>]
                - <product> - ex. EM5625, SWI6800, SWI6800V2
                - <build_type> - Either FP for normal release or PP for point 
                    release.
                - <pid> - USB product ID (note: <pid> only appears in 
                    dedicated builds like EM5625 or MC5720 and does not appear
                    in single builds used for multiple products (ex. SWI6800)).
                - <major>, <minor> and <point> are zero padded two digit
                    decimal version numbers.
        - CDMA USBD: 
            - <product>_<customer>.<major>.<minor>
                - <product> - ex. SWI6800 or SWI6800V2
                - <customer> - ex. GENERIC, AC595U
                - <major> and <minor> are zero padded two digit decimal version
                    numbers.
        - UMTS QCOM, BOOT, APPL: 
            - AW_X_Y_Zxx
                - A - Device family type (U-6275, H-6280, D-7200R2.x, 
                    F-7200R3.x, J-7200A)
                - W - Major release
                - X - Minor release
                - Y - Sub-release identifier (increments with a stack change)
                - Z - Sub-release identifier (increments with a Sierra update)
                - xx - Platform and load type
                    - 'ap' - Application
                    - 'bt' - Bootloader
        - UMTS USBD: 
            - AW_Xud
                - A - Device family type (U-6275, H-6280, D-7200R2.x, 
                    F-7200R3.x, J-7200A)
                - W - Major release
                - X - Minor release 
        - SWOC: 
            - CDPC_<version>_<major>.<minor>.<point>
                - <version> - zero padded 5-digit decimal CDPC (CD Product 
                    Code).
                - <major>, <minor>, and <point> are zero padded two digit 
                    decimal numbers for CD version. */
    swi_char szVersion[LENGTH_ImgInfoVer];
    
    /*!< A NULL terminated string returned from the device which specifies the 
        release date of the flash image of interest.  The format of the date
        is MM/DD/YY. */
    swi_char szRelDate[LENGTH_ImgInfoRelDate];
    
    /*!< A NULL terminated string returned from the device which specifies the 
        product type of the flash image of interest.  Exact values are image
        specific; examples are '6800' or '5625' for FW_DWN_APPL and 
        FW_DWN_BOOT images.  FW_DWN_SWOC image does not use this field. */
    swi_char szProdType[LENGTH_ImgInfoProdType];
    
    /*!< The size, in bytes, of the image data only and does not include the 
        size of the CWE header. */
    swi_int32   nImgSize;
};

/**
 *
 * Enumeration of object ID range specifiers used in 
 * SWI_STRUCT_OBJRNGREV_InfoBlock
 *
 * \sa SWI_STRUCT_OBJRNGREV_InfoBlock
 *
 */
enum SWI_TYPE_ObjIdRngSpecifier
{
    OBJIDRNGSPECIFIER_Cwe,      /*!< CWE (0x0000 - 0x0EFF) */
    OBJIDRNGSPECIFIER_CweLbs,   /*!< CWE LBS (0x0F00 - 0x0F7F) */
    OBJIDRNGSPECIFIER_Gprs,     /*!< GPRS (0x1000 - 0x1FFF) */
    OBJIDRNGSPECIFIER_Cdma,     /*!< CDMA (0x1000 - 0x5FFF) */
    OBJIDRNGSPECIFIER_Mp,       /*!< MP (0x6000 - 0x6FFF) */
    OBJIDRNGSPECIFIER_Umts,     /*!< UMTS (0x7000 - 0x70FF) */
    OBJIDRNGSPECIFIER_CweDm     /*!< CWE DM (0x0F80 - 0x0FFF) */
};
/**
 *
 * Contains object revision information for a specific CnS type.
 * 
 * Firmware keeps a reverse revision number and a forward revision number for
 * each range.  The reverse revision number indicates to the host the oldest
 * set of objects supported by the firmware.  If firmware stops supporting
 * an object, the reverse revision number will be incremented past the
 * revision number of the unsupported object.
 * 
 * The forward revision number indicates the newest set of objects supported
 * by the firmware.  Every time an object is modified or added, this number
 * will be incremented.
 * 
 * The API keeps a single revision number for every range.  If the API
 * object revision number falls between the reverse and forward revision
 * numbers reported by the firmware for a particular range of object IDs
 * then the host software and modem firmware are compatible in that range.
 *
 * \sa SWI_STRUCT_ObjRngRev
 *
 */
struct SWI_STRUCT_OBJRNGREV_InfoBlock
{
    /*!< Size of this structure */
    swi_int32 sizeStuct;
    
    /*!< Identifies the CnS object type */
    enum SWI_TYPE_ObjIdRngSpecifier eRngSpecifier;
    
    /*!< Modem reverse object revision number */
    swi_int32 nModemReverseObjRevNum;
    
    /*!< Modem forward object revision number */
    swi_int32 nModemForwardObjRevNum;
    
    /*!< API object revision number */
    swi_int32 nApiObjRevNum;
};

/**
 *
 * Contains list of object range revisions supported by the modem and API.
 *
 * \sa SwiGetObjRngRev
 * \sa SWI_STRUCT_OBJRNGREV_InfoBlock
 *
 */
struct SWI_STRUCT_ObjRngRev
{
    /*!< Size of this structure */
    swi_int32 sizeStruct;     
    
    /*!< Number of object revision information blocks */
    swi_int32 NumOfEntries;
    
    /*!< Object revision information block array */
    struct SWI_STRUCT_OBJRNGREV_InfoBlock arInfoBlock[MAX_OBJRNGINFOBLOCKS];
};

/**
 *
 * Enumeration of radio power modes.
 *
 * This enumeration is used by SWI_STRUCT_RadioPower. 
 *
 * \sa SWI_STRUCT_RadioPower
 * \sa SWI_NOTIFY_RadioPower 
 *
 */
enum SWI_TYPE_RadioPowerMode
{
    SWI_RADIOPOWERMODE_Low,     /*!< Radio is in low power mode */
    SWI_RADIOPOWERMODE_Online  /*!< Radio is online (full power mode) */
};

/**
 *
 * Low power modem reason bit mask values.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_STRUCT_RadioPower
 * \sa SWI_NOTIFY_RadioPower  
 *
 */
enum SWI_TYPE_RadioPowerReason
{
    /*!< W_DISABLED is asserted */
    SWI_RADIOPOWERREASON_WDISABLE = 0x0001,

    /*!< User request */
    SWI_RADIOPOWERREASON_UserRequest = 0x0002,

    /*!< Out of operational temperature range */
    SWI_RADIOPOWERREASON_Temperature = 0x0004,

    /*!< Out of operational supply voltage range */  
    SWI_RADIOPOWERREASON_Battery = 0x0008
};

/**
 *
 * Enumeration of power down request reasons.
 *
 * This enumeration is used by SWI_STRUCT_RadioPower
 *
 * \sa SWI_STRUCT_RadioPower
 * \sa SWI_NOTIFY_RadioPower
 *
 */
enum SWI_TYPE_ModuleReqReason
{
    /*!< No reason given (e.g. when no requests are being made */
    SWI_MODULEREQREASON_NoReason,

    /*!< Temperature exceeds limit */ 
    SWI_MODULEREQREASON_Temperature,

    /*!< Voltage exceeds limit */
    SWI_MODULEREQREASON_Battery,

    /*!< Network related reason */
    SWI_MODULEREQREASON_Network,

    /*!< All other reasons */
    SWI_MODULEREQREASON_Other
};

/**
 *
 * This structure contains the radio power information.
 *
 * Certain host platforms may be driving the power rail of the module
 * and thus if the module tries to power itself down, it will just be
 * powered back up.  Therefore, when bModuleReqPowerDown is returned
 * as true, the host must remove power from the power rail to the module.
 *
 * This structure is valid for MiniCards only.  Both GSM and CDMA
 * MiniCards are supported.
 *
 * \sa SWI_NOTIFY_RadioPower
 * \sa SwiGetRadioPower
 * \sa SwiSetRadioPower  
 *
 */
struct SWI_STRUCT_RadioPower
{
    /*!< The size, in bytes, of this structure.  This value must always be
        populated by the calling application when calling SwiGetRadioPower
        or SwiSetRadioPower. */
    swi_uint32 sizeStruct;

    /*!< Radio power mode enumeration.  See SWI_TYPE_RadioPowerMode 
        more details. */
    enum SWI_TYPE_RadioPowerMode eMode;

    /*!< Bit mask indicating reason for entering the radio power mode.  
        See SWI_TYPE_RadioPowerReason for mask definitions.  This value is
        ignored when setting the radio power state. */
    swi_uint32 nLowPowerReason;

    /*!< Set to true if the module has requested to be powered down.  Only 
        valid when returned in a notification.  This field only applies to 
        CDMA MiniCards. */
    swi_bool bModuleReqPowerDown;
    
    /*!< Reason enumeration stating why the modem has requested to be
        powered down.  Only valid when returned in a notification. */
    enum SWI_TYPE_ModuleReqReason eRequestReason;
};

/**
 *
 * Enumeration of radio temperature states
 *
 * This enumeration is used by the SWI_STRUCT_RadioTempState structure.
 *
 * \sa SWI_STRUCT_RadioTempState
 * \sa SWI_NOTIFY_RadioTempState
 * \sa SwiGetRadioTempState
 *
 */
enum SWI_TYPE_RadioTempState
{
    SWI_RADIOTEMPSTATE_Normal,      /*!< Normal temperature state */
    SWI_RADIOTEMPSTATE_HighWarning, /*!< High temperature warning state */
    SWI_RADIOTEMPSTATE_HighCrit,    /*!< High temperature critical state */
    SWI_RADIOTEMPSTATE_LowCrit     /*!< Low temperature critical state */
};

/**
 *
 * This structure contains the radio temperature state.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_TYPE_RadioTempState
 * \sa SWI_NOTIFY_RadioTempState
 * \sa SwiGetRadioTempState
 *
 */
struct SWI_STRUCT_RadioTempState
{
    /*!< The size, in bytes, of this structure.  This value must always be
        populated by the calling application when calling 
        SwiGetRadioTempState.  */
    swi_uint32 sizeStruct;
    
    /*!< Temperature state enumeration.  See SWI_TYPE_RadioTempState for
        more details. */
    enum SWI_TYPE_RadioTempState eState;
    
    /*!< A numerical representation of the current temperature in degrees
        Celsius. */
    swi_uint32 nCurrent;
};

/**
 *
 * Enumeration of radio battery states
 * 
 * This enumeration is used by SWI_STRUCT_RadioBattery.
 *
 * \sa SWI_STRUCT_RadioBattery
 * \sa SWI_NOTIFY_RadioBattery
 *
 */
enum SWI_TYPE_RadioBatteryState
{
    SWI_RADIOBATTERYSTATE_Normal,     /*!< Normal state */
    SWI_RADIOBATTERYSTATE_LowWarning, /*!< Low battery warning state */
    SWI_RADIOBATTERYSTATE_LowCrit,    /*!< Low battery critical state */
    SWI_RADIOBATTERYSTATE_HighCrit   /*!< High battery critical state */
};

/**
 *
 * This structure contains the radio battery state.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_TYPE_RadioBatteryState
 * \sa SWI_NOTIFY_RadioBattery
 * \sa SwiGetRadioBattery
 *
 */
struct SWI_STRUCT_RadioBattery
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application when calling 
        SwiGetRadioBattery. */
    swi_uint32 sizeStruct;

    /*! Current battery state enumeration. See SWI_TYPE_RadioBatteryState
        for more details. */
    enum SWI_TYPE_RadioBatteryState eState;
    
    /*! Current supply voltage in mV */
    swi_uint32 nCurrent;
};
/**
 *
 * Enumeration of radio technologies.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 *
 */
enum SWI_TYPE_RadioType
{
    SWI_RADIOTYPE_1xrtt,     /*!< 1xRTT */
    SWI_RADIOTYPE_Evdo,      /*!< EVDO */
    SWI_RADIOTYPE_GsmWcdma  /*!< GSM/WCDMA */
};

/**
 *
 * Enumeration of radio bands.
 *
 * This enumeration is used by SWI_STRUCT_RadioBandInfo and
 * SWI_STRUCT_GmsCellMeasure.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SWI_STRUCT_GsmCellMeasure
 *
 */
enum SWI_TYPE_RadioBand
{
    SWI_RADIOBAND_CdmaClass0 = 0x00,    /*!< CDMA Band Class 0 */
    SWI_RADIOBAND_CdmaClass1,           /*!< CDMA Band Class 1 */
    SWI_RADIOBAND_CdmaClass2,           /*!< CDMA Band Class 2 */
    SWI_RADIOBAND_CdmaClass3,           /*!< CDMA Band Class 3 */
    SWI_RADIOBAND_CdmaClass4,           /*!< CDMA Band Class 4 */
    SWI_RADIOBAND_CdmaClass5,           /*!< CDMA Band Class 5 */
    SWI_RADIOBAND_CdmaClass6,           /*!< CDMA Band Class 6 */
    SWI_RADIOBAND_CdmaClass7,           /*!< CDMA Band Class 7 */
    SWI_RADIOBAND_CdmaClass8,           /*!< CDMA Band Class 8 */
    SWI_RADIOBAND_CdmaClass9,           /*!< CDMA Band Class 9 */
    SWI_RADIOBAND_CdmaClass10,          /*!< CDMA Band Class 10 */
    SWI_RADIOBAND_CdmaClass11,          /*!< CDMA Band Class 11 */
    SWI_RADIOBAND_CdmaClass12,          /*!< CDMA Band Class 12 */
    SWI_RADIOBAND_Gsm450     = 0x28,    /*!< GSM 450 Band */
    SWI_RADIOBAND_Gsm480,               /*!< GSM 480 Band */
    SWI_RADIOBAND_Gsm750,               /*!< GSM 750 Band */
    SWI_RADIOBAND_Gsm850,               /*!< GSM 850 Band */
    SWI_RADIOBAND_GsmEGsm900,           /*!< GSM E-GSM 900 Band */
    SWI_RADIOBAND_GsmPGsm900,           /*!< GSM P-GSM 900 Band */
    SWI_RADIOBAND_GsmRGsm900,           /*!< GSM R-GSM 900 Band */
    SWI_RADIOBAND_GsmDcs1800,           /*!< GSM DCS 1800 Band */
    SWI_RADIOBAND_GsmPcs1900,           /*!< GSM DCS 1900 Band */
    SWI_RADIOBAND_WcdmaIImt2000 = 0x50, /*!< WCDMA I IMT 2000 */
    SWI_RADIOBAND_WcdmaIIPcs1900,       /*!< WCDMA II PCS 1900 */
    SWI_RADIOBAND_WcdmaIIIDcs1800,      /*!< WCDMA III DCS 1800 */
    SWI_RADIOBAND_WcdmaIV1700,          /*!< WCDMA IV 1700 */
    SWI_RADIOBAND_WcdmaV850,            /*!< WCDMA V 850 */
    SWI_RADIOBAND_WcdmaVI800,           /*!< WCDMA VI 800 */
    SWI_RADIOBAND_WcdmaVII2600,         /*!< WCDMA VII 2600 */
    SWI_RADIOBAND_WcdmaVIII900,         /*!< WCDMA VIII 900 */
    SWI_RADIOBAND_WcdmaIX1700,          /*!< WCDMA IX 1700 */
    SWI_RADIOBAND_Invalid      = 0xFF   /*!< Invalid band */
};

/**
 *
 * This structure contains radio band information
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_TYPE_RadioType
 * \sa SWI_TYPE_RadioBand
 * \sa SWI_STRUCT_RadioBandList
 *
 */
struct SWI_STRUCT_RadioBandInfo
{
    /*! Radio type enumeration.  See SWI_TYPE_RadioType for more details. */
    enum SWI_TYPE_RadioType eType;

    /*! Radio band enumeration.  See SWI_TYPE_RadioBand for more details. */
    enum SWI_TYPE_RadioBand eBand;
};

/**
 *
 * This structure contains list of radio bands.
 *
 * This structure replaces SWI_STRUCT_Band and should be used
 * with SwiGetCurrentRadioBand.  Both SwiGetBandInfo and SWI_STRUCT_Band
 * is obsolete and should no longer be used with devices that support
 * 3G networks.
 *
 * Supports PC cards, embedded devices, and MP line of products. 
 *
 * \sa SWI_TYPE_RadioType
 * \sa SWI_TYPE_RadioBand
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
struct SWI_STRUCT_RadioBandList
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! Number of radio bands in list.  Ranging from 0 to MAX_BAND_LIST. */
    swi_uint32 nNumBands;

    /*! List of radio bands.  Only valid if nNumBands is greater than 0.
        The array of SWI_STRUCT_RadioBandList structures is populated
        beginning with index 0 up to index nNumBands-1 sequentially. */
    struct SWI_STRUCT_RadioBandInfo sList[MAX_BAND_LIST];   
};

/**
 *
 * Band Class 0 A-system only (Cellular)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass0A    0x0000000000000001ull

/**
 *
 * Band Class 0 B-system only (Cellular)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass0B    0x0000000000000002ull

/**
 *
 * Band Class 0 A & B (Cellular)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass0ABo  0x0000000000000003ull

/**
 *
 * Band Class 1 (US PCS)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass1     0x0000000000000004ull

/**
 *
 * Band Class 2 (TACS)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass2     0x0000000000000008ull

/**
 *
 * Band Class 3 (JTACS)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass3     0x0000000000000010ull

/**
 *
 * Band Class 4 (Korean PCS)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass4     0x0000000000000020ull

/**
 *
 * Band Class 5 (450 MHz)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass5     0x0000000000000040ull

/**
 *
 * Band Class 6 (2 GHz)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass6     0x0000000000000080ull

/**
 *
 * Band Class 7 (700 MHz)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass7     0x0000000000000100ull

/**
 *
 * Band Class 8 (1800 MHz)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass8     0x0000000000000200ull

/**
 *
 * Band Class 9 (900 MHz)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass9     0x0000000000000400ull

/**
 *
 * Band Class 10 (Secondary 800 MHz)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass10    0x0000000000000800ull

/**
 *
 * Band Class 11 (400 MHz European PAMR)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass11    0x0000000000001000ull

/**
 *
 * Band Class 12 (800 MHz PAMR)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaClass12    0x0000000000002000ull

/**
 *
 * PCS/Cellular
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_CdmaPcsCellular   0x0000000000000007ull

/**
 *
 * GSM 450
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_Gsm450         0x0000000000010000ull

/**
 *
 * GSM 480
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_Gsm480         0x0000000000020000ull

/** 
 *
 * GSM 750
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_Gsm750         0x0000000000040000ull

/**
 *
 * GSM 850
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_Gsm850         0x0000000000080000ull

/**
 *
 * GSM RGSM 900
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmRGsm900     0x0000000000100000ull

/**
 *
 * GSM PCS 1900
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmPcs1900     0x0000000000200000ull

/**
 *
 * GSM DCS 1800
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmDcs1800     0x0000000000400000ull

/** 
 *
 * GSM EGSM 900
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmEGsm900     0x0000000000800000ull

/**
 *
 * GSM PGSM 900
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmPGsm900     0x0000000001000000ull

/**
 *
 * GSM 850/PCS
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 * 
 */
#define SWI_RADIOBANDGROUP_GsmPcs850      0x0000000000280000ull

/**
 *
 * GSM EGSM/DCS
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmEGsmDcs     0x0000000001C00000ull

/**
 *
 * GSM All
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_GsmAll         0x0000000001E80000ull

/**
 *
 * WCDMA I IMT 2000 (EU)
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaIImt2000  0x0000000100000000ull

/**
 *
 * WCDMA II PCS 1900
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaIIPcs1900   0x0000000200000000ull

/**
 *
 * WCDMA III 1700
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaIII1700   0x0000000400000000ull

/**
 *
 * WCDMA IV 1700
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaIV1700    0x0000000800000000ull

/**
 *
 * WCDMA V 850
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaV850      0x0000001000000000ull

/**
 *
 * WCDMA VI 800
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaVI800     0x0000002000000000ull

/**
 *
 * WCDMA VII 2600
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaVII2600   0x0000004000000000ull

/**
 *
 * WCDMA VIII 900
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaVIII900   0x0000008000000000ull

/**
 *
 * WCDMA IX 1700
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaIX1700    0x0000010000000000ull

/**
 *
 * WCDMA NA
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaNa        0x0000001200000000ull

/**
 *
 * WCDMA/GSM EU
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaGsmEu     0x0000000101C00000ull

/**
 *
 * WCDMA/GSM NA
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaGsmNa     0x0000001200280000ull

/**
 *
 * WCDMA Australia
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaAustralia   0x0000001100000000ull

/**
 *
 * WCDMA Australia/GSM EU
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaAustrailiaGsmEu    0x0000001101C00000ull

/**
 *
 * WCDMA Japan
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaJapan     0x0000002100000000ull

/**
 *
 * WCDMA All
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_WcdmaAll    0x0000001300000000ull

/**
 *
 * Auto band
 * 
 * This radio band group is to be used with SWI_STRUCT_RadioBandConfig.
 *
 * \sa SWI_STRUCT_RadioBandInfo
 * \sa SwiGetCurrentRadioBand
 *
 */
#define SWI_RADIOBANDGROUP_AutoBand       0xFFFFFFFFFFFFFFFFull

/**
 *
 * This structure contains radio band configurations.
 *
 * Only supported on 3G devices and newer.
 *
 * \sa Radio Band Group Definitions
 * \sa SwiGetRadioBandCfg
 *
 */
struct SWI_STRUCT_RadioBandConfig
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! A bit mask indicating the last selected band grouping setting.  
        See Radio Band Group Definitions for more details. */
    swi_uint64 nLastSelected;

    /*! Number of band settings in list.  Ranges from 0 to MAX_BAND_GROUPS. */
    swi_uint32 nNumInGroup;

    /*! An array of band settings.  Only valid if nNumInGroup is greater
        than 0.  The array of Radio Band Group Definitions is populated
        beginning with index 0 up to index nNumInGroup-1 sequentially. */
    swi_uint64 arBandSetting[MAX_BAND_GROUPS];
};

/**
 *
 * This structure contains basic usb port name information.
 *
 *  This structure is used by SwiGetUsbPortName to return the device name for
 *  the available endpoints .
 *
 *  Supports both GSM and CDMA PC cards, embedded devices, and MP line
 *  of products. 
 *
 *  \sa SwiGetUsbPortName
 *  \sa See sample code, ConnectApi
 *
 */
struct SWI_STRUCT_UsbPortName
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;     

    /*! HIP port */
    swi_uint8  HipPort[LENGTH_PortName];
     
    /*! Diag port */
    swi_uint8  DiagPort[LENGTH_PortName];
     
    /*! NMEA port */
    swi_uint8  NmeaPort[LENGTH_PortName];
     
    /*! AT command port */
    swi_uint8  AtCmdPort[LENGTH_PortName];
     
    /*! DATA1 port */
    swi_uint8  Data1Port[LENGTH_PortName];
     
    /*! DATA2 port */
    swi_uint8  Data2Port[LENGTH_PortName];
     
    /*! DATA3 port */
    swi_uint8  Data3Port[LENGTH_PortName];
    
    /*! Direct IP port */
    swi_uint8  DirectIPPort[LENGTH_PortName];
    
    /*! QMI port */
    swi_uint8  QMIPort[LENGTH_PortName];
    
    /*! Mass Storage port */
    swi_uint8  MassStoragePort[LENGTH_PortName];
};

/*! Max length of file name in device file system */
#define SWI_FILENAME_LEN                    8
#if defined(CNS_FILENAME_LEN) && SWI_FILENAME_LEN != CNS_FILENAME_LEN
#error CNS and SWI definition mismatch
#endif

/*! Max length of API unlock password */
#define SWI_API_UNLOCK_PWD_LEN              10
#if defined(CNS_API_UNLOCK_PWD_LEN) && \
    SWI_API_UNLOCK_PWD_LEN != CNS_API_UNLOCK_PWD_LEN
#error CNS and SWI definition mismatch
#endif

/**
 * Enumeration of NV update from file status
 */
typedef enum eUpdateFromFileStatus
{
    FILE_UPDATE_SUCCESS,    /*!<  Update success */
    FILE_NOT_FOUND,         /*!<  File not found */
    FILE_INCORRECT_SIZE,    /*!<  Incorrect size */
    FILE_INCORRECT_HEADER,  /*!<  Incorrect header */
    FILE_INVALID_REVISION  /*!<  Invalid revision */
}UpdateFromFileStatus;

/**
 *
 * This structure contains NV update from file configurations and results.
 *
 */
typedef struct SWI_STRUCT_NVUpdateFromFile
{
    swi_uint32 sizeStruct;        /*!< size of this structure */

    /*! File name in device to be used */
    swi_uint8 szFileName[SWI_FILENAME_LEN];
    
    UpdateFromFileStatus nStatus;       /*!< Operation return status */
    swi_uint32 nItemsUpdated;           /*!< Number of items updated */
    swi_uint32 nItemsSipped;            /*!< Number of items not updated */
}SWI_STRUCT_NVUpdateFromFile;

/**
 *
 * Enumeration of unlock level.
 *
 */
typedef enum eApiUnlockLevel
{
    API_UNLOCK_OEM,     /*!< OEM unlock */
    API_UNLOCK_SWI     /*!< SWI unlock */
}ApiUnlockLevel;

/**
 *
 * Enumeration of unlock result.
 *
 */
typedef enum eApiUnlockResult
{
    API_UNLOCK_SUCCESS,     /*!< Action succeeded */
    API_UNLOCK_PWD_INVALID /*!< Invalid password key */
}ApiUnlockResult;

/**
 *
 * This structure contains API unlock configurations.
 *
 * \sa ApiUnlockLevel
 * \sa ApiUnlockResult
 *
 */
typedef struct SWI_STRUCT_ApiUnlock
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    ApiUnlockLevel nLevel;      /*!< Lock level. See ApiUnlockLevel */
    swi_uint8 nPasswordLen;     /*!< Length of OEM password */
    swi_uint8 szPassword[SWI_API_UNLOCK_PWD_LEN];   /*!< OEM password */
    ApiUnlockResult nResult;    /*!< Operation result. See ApiUnlockResult */
}SWI_STRUCT_ApiUnlock;

/**
 *
 * Enumeration of Remote Diagnostics Agent enableing status.
 *
 */
typedef enum SWI_TYPE_RDA_SUPPORT_STATUS
{
    RDA_STATUS_NOT_SUPPORTED  = 0,  /*!< Not supported */
    RDA_STATUS_DISABLED       = 1,  /*!< Disabled (not running) */
    RDA_STATUS_ENABLED        = 2   /*!< Enabled */
}SWI_TYPE_RDA_SUPPORT_STATUS;

#define SWI_RDA_NAME_MAX        69  /*!< Max RDA name length */
#define SWI_RDA_VERSION_MAX     32  /*!< Max RDA version length */

/**
 *
 * This structure contains Remote Diagnostics Agent Support configurations.
 *
 * \sa SWI_TYPE_RDA_SUPPORT_STATUS
 * \sa ApiUnlockResult
 *
 */
typedef struct SWI_STRUCT_RDA_SUPPORT
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    swi_uint8  nRDASupport;         /*!< see SWI_TYPE_RDA_SUPPORT_STATUS */
    swi_uint8  nNameLength;                     /*!< Vendor name length */
    swi_uint8  Name[SWI_RDA_NAME_MAX];          /*!< Vendor name (ASCII) */
    swi_uint8  nVersionLength;                  /*!< Agent version length */
    swi_uint8  Version[SWI_RDA_VERSION_MAX];    /*!< Agent version (ASCII) */
}SWI_STRUCT_RDA_SUPPORT;

/**
 *
 * Enumeration Removable Media support.
 *
 */
typedef enum SWI_TYPE_REMOVABLE_MEDIA
{
    REMOVABLE_MEDIA_MINI_SD   = 0x0001,     /*!< Mini-SD supported  */
    REMOVABLE_MEDIA_MICRO_SD  = 0x0002     /*!< Micro-SD supported */
}SWI_TYPE_REMOVABLE_MEDIA;

/**
 *
 * This structure contains Removable Media configurations.
 *
 * \sa SWI_TYPE_REMOVABLE_MEDIA
 * \sa SWI_TYPE_REMOVABLE_MEDIA
 *
 */
typedef struct SWI_STRUCT_RM_CONFIG
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! contains RM capability support. 
    See SWI_TYPE_REMOVABLE_MEDIA for bit definitions */
    swi_uint16 nFeatureSupportMask;

    /*! contains RM enable status.
    See SWI_TYPE_REMOVABLE_MEDIA for bit definitions */
    swi_uint16 nFeatureEnableMask;
}SWI_STRUCT_RM_CONFIG;

/**
 *
 * This structure contains Removable Media configurations.
 *
 * \sa SWI_STRUCT_RM_CONFIG
 * \sa SWI_TYPE_REMOVABLE_MEDIA
 *
 */
typedef struct SWI_STRUCT_RM_CONFIG_SET
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! SWI_TYPE_REMOVABLE_MEDIA: 
    Note-- can only disable/enable 1 media at a time */
    swi_uint16 nRMFeature;

    /*! contains RM enable status.
    See SWI_TYPE_REMOVABLE_MEDIA for bit definitions */
    swi_uint8  bEnable;
}SWI_STRUCT_RM_CONFIG_SET;

/**
 *
 * This structure contains Software on Card (SWoC) configuration items.
 *
 * \sa SwiGetSwocCfg
 *
 */
struct SWI_STRUCT_GetSwocCfg
{
    /*! The size, in bytes, of this structure.  This value must always be
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! Bitmask indicating supported features:
            bit 0: general SWoC operation supported
            bit 1: SWoC access in modem modem supported
            bit 2: bypass SWoC on soft boot supported */
    swi_uint16 FeatureSupport;

    /*! Bitmask indicating feature enable/disable state:
            bit 0: general SWoC operation enabled
            bit 1: SWoC access in modem mode enabled
            bit 2: bypass SWoC on soft boot enabled */
    swi_uint16 FeatureState;

    /*! Bitmask indicating feature enable/disable host capability:
            bit 0: host can enable/disable general SWoC
            bit 1: host can enable/disable SWoC access in modem mode
            bit 2: host can enable/disable bypass SWoC on soft boot */
    swi_uint16 FeatureCap;

    /*! Bitmask indicating host change requires OEM level unlock:
            bit 0: general SWoC change requires unlock
            bit 1: SWoC access in modem mode change requires unlock
            bit 2: bypass SWoC on soft boot change requires unlock */
    swi_uint16 LockReq;
};

/* Function prototypes */
extern SWI_API SWI_RCODE SwiApiStartup(
    struct SWI_STRUCT_ApiStartup *pOpenParams);
/* RILSTART */
extern SWI_API SWI_RCODE SwiTerminateSDK( void );
/* RILSTOP */

extern SWI_API SWI_RCODE SwiGetFirmwareVersion(
    swi_charp szFWVersion, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetFirmwareBuildDate(
    swi_charp szBuildDate, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetHardwareVersion(
    swi_charp szHWVersion, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetBootVersion(
    swi_charp szBVVersion, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetBootloaderBuildDate(
    swi_charp szBLBldDate, 
    swi_uint32 *nLength, 
    swi_uint32 Timeout);

extern SWI_API SWI_RCODE SwiGetRadioPower(
    struct SWI_STRUCT_RadioPower *pRadioPower, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetRadioPower(
    struct SWI_STRUCT_RadioPower* pRadioPower, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetRadioTempState(
    struct SWI_STRUCT_RadioTempState *pRadioTemp,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetRadioBattery(
    struct SWI_STRUCT_RadioBattery *pRadioBattery,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetCurrentRadioBand(
    struct SWI_STRUCT_RadioBandList *pRadioBand, 
    swi_int32  timeout);

extern SWI_API SWI_RCODE SwiGetRadioBandCfg(
    struct SWI_STRUCT_RadioBandConfig *pRadioBand, 
    swi_int32  timeout);

extern SWI_API SWI_RCODE SwiSetRadioBandCfg(
    swi_uint64 nRadioBandGroup, 
    swi_int32  timeout);

extern SWI_API SWI_RCODE SwiGetUsbdInfo(
    struct SWI_STRUCT_UsbdInfo* pUsbdInfo, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetPriInfo(
    struct SWI_STRUCT_PriInfo* pPriInfo, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetModemType(
    struct SWI_STRUCT_ModemType* pSwiModemType,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetDeviceTime(
    struct SWI_STRUCT_DeviceTime* pDeviceTime,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetDeviceID(
    swi_charp pszDeviceID,
    swi_uint32 sizeBuffer,
    swi_uint32  timeout);

extern SWI_API SWI_RCODE SwiGetFlashImgInfo(
    struct SWI_STRUCT_FlashImgInfo* pFlashImgInfo, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetObjRngRev(
    struct SWI_STRUCT_ObjRngRev *ObjRngRevp, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetAvailAirServers(
    struct SWI_STRUCT_AirServerList *sList);

extern SWI_API SWI_RCODE SwiGetBootAndHoldMode(
    swi_bool *pbootnhold, swi_uint32 timeout );
    
extern SWI_API void SwiGetLastError(
    swi_char *szError, 
    swi_uint32 sizeBuffer);

extern SWI_API SWI_RCODE SwiGetUsbPortName(
    struct SWI_STRUCT_UsbPortName *sPortNamep);

extern SWI_API SWI_RCODE SwiNVUpdateFromFile(
            SWI_STRUCT_NVUpdateFromFile *pNVUpdateFromFile, 
            swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiApiUnlock(SWI_STRUCT_ApiUnlock *pApiUnlock, 
                               swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetRemoteDiagAgentSupport(
        SWI_STRUCT_RDA_SUPPORT *psRDA, 
        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetRemoteDiagAgentEnable(swi_uint8 bEnable, 
                                              swi_uint8 *pStatus, 
                                              swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetRemovableMediaSupport(
        SWI_STRUCT_RM_CONFIG *psRM, 
        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetRemovableMediaSupport(
        const SWI_STRUCT_RM_CONFIG_SET *psRM, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSoftModemReset(void);

extern SWI_API SWI_RCODE SwiResetToBootHold( swi_uint32 timeout );

extern SWI_API SWI_RCODE SwiGetSdkVersion(swi_charp *sdkversionpp);

extern SWI_API SWI_RCODE SwiGetDeviceInformation(
    struct SWI_STRUCT_DeviceInformation_Request *sDeviceInformation_Requestp, 
    struct SWI_STRUCT_DeviceInformation_Response *sDeviceInformation_Responsep,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetSwocCfg(
    struct SWI_STRUCT_GetSwocCfg* pSwocCfg,
    swi_uint32 timeout);

extern SWI_RCODE SwiIntSendnWaitUsbPortNameRequest(
    struct SWI_STRUCT_UsbPortName *sPortNamep );

extern void SwiUpkDeviceInformationResp(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_DeviceInformation_Response *pDeviceInfo );

extern void SwiUpkSwocCfg (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GetSwocCfg *pSwocCfg );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_CM */

/* 
 * $Log: SwiApiCmBasic.h,v $
 */
