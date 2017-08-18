/**
 * \ingroup loc
 *
 * \file    qaGobiApiLoc.h
 *
 * \brief   Location API function prototypes
 */

/*
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 */
#ifndef __GOBI_API_LOC_H__
#define __GOBI_API_LOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_SENSOR_DATA_LEN       64
#define  MAX_TEMP_DATA_LEN         64

/**
 *  This structure contains the Parameter for RegisterEvents
 *
 *  \param  eventRegister
 *          - Specifies the events that the control point is interested
 *          in receiving.
 *          -Values
 *             - 0x00000001 - to receive position report event
 *                            indications
 *             - 0x00000002 - to receive satellite report event
 *                            indications
 *             - 0x00000004 - to receive NMEA reports for position
 *                            and satellites in view
 *             - 0x00000008 - to receive NI Notify/Verify request
 *                            event indications
 *             - 0x00000010 - to receive time injection request
 *                            event indications.
 *             - 0x00000020 - to receive predicted orbits request
 *                            event indications.
 *             - 0x00000040 - to receive position injection request
 *                            event indications.
 *             - 0x00000080 - to receive fix session status report
 *                            event indications.
 *             - 0x00000200 - to receive Wi-Fi position request
 *                            event indications.
 *             - 0x00000400 - to receive notifications from the
 *                            location engine indicating its readiness
 *                            to accept data from the sensors
 *             - 0x00000800 - to receive time sync requests from
 *                            the GPS engine. Time sync enables the
 *                            GPS engine to synchronize its clock
 *                            with the sensor processor’s clock.
 *             - 0x00001000 - to receive Stationary Position Indicator (SPI)
 *                            streaming report indications.
 *             - 0x00002000 - to receive location server requests.
 *                            These requests are generated when
 *                            the service wishes to establish a
 *                            connection with a location server.
 *             - 0x00004000 - to receive notifications related
 *                            to network-initiated Geofences. These
 *                            events notify the client when a network-initiated
 *                            Geofence is added, deleted, or edited.
 *             - 0x00008000 - to receive Geofence alerts. These alerts
 *                            are generated to inform the client of
 *                            the changes that may affect a Geofence,
 *                            e.g., if GPS is turned off or if the
 *                            network is unavailable.
 *             - 0x00010000 - to receive notifications when a Geofence
 *                            is breached. These events are generated
 *                            when a UE enters or leaves the perimeter
 *                            of a Geofence. This breach report is for
 *                            a single Geofence.
 *             - 0x00020000 - to register for pedometer control requests
 *                            from the location engine. The location
 *                            engine sends this event to control the
 *                            injection of pedometer reports.
 *             - 0x00040000 - to register for motion data control requests
 *                            from the location engine. The location
 *                            engine sends this event to control the
 *                            injection of motion data.
 *             - 0x00080000 - to receive notification when a batch is full.
 *                            The location engine sends this event to
 *                            notify of Batch Full for ongoing batching
 *                            session.
 *             - 0x00100000 - to receive position report indications
 *                            along with an ongoing batching session.
 *                            The location engine sends this event to
 *                            notify the batched position report while
 *                            a batching session is ongoing.
 *             - 0x00200000 - to receive Wi-Fi Access Point (AP) data
 *                            inject request event indications.
 *             - 0x00400000 - to receive notifications when a Geofence
 *                            is breached. These events are generated
 *                            when a UE enters or leaves the perimeter
 *                            of a Geofence. This breach notification
 *                            is for multiple Geofences. Breaches from
 *                            multiple Geofences are all batched and
 *                            sent in the same notification.
 *             - 0x00800000 - to receive notifications from the location
 *                            engine indicating its readiness to accept
 *                            vehicle data (vehicle accelerometer,
 *                            vehicle angular rate, vehicle odometry, etc.).
 *             - 0x01000000 - to receive system clock and satellite
 *                            measurement report events (system clock,
 *                            SV time, Doppler, etc.).
 *             - 0x02000000 - to receive satellite position reports
 *                            as polynomials.
 *
 */

typedef struct
{
	ULONGLONG eventRegister;
} LOCEventRegisterReqResp;

/**
 *  This structure contains the Parameter External Power Source State.
 *
 *  \param  extPowerState
 *          - Specifies the Power state; injected by the control
 *            point.
 *          - Values
 *             - 0 - Device is not connected to an external power
 *                   source
 *             - 1 - Device is connected to an external power source
 *             - 2 - Unknown external power state
 */

typedef struct
{
	ULONG extPowerState;
} LOCExtPowerStateReqResp;

/**
 * This structure contains the Application Information
 *
 * \param appProviderLength
 *        - Length of the Application Provider
 *
 * \param pAppProvider
 *        - Application Provider
 *        - Depends upon the Length of application Provider
 *
 * \param appNameLength
 *        - Length of Application Name
 *
 * \param pAppName
 *        - Application Name
 *        - Depends upon the Length of application Name
 *
 * \param appVersionValid
 *        - Specifies whether the application version string contains a
 *        valid value
 *
 *        - 0x00 (FALSE) – Application version string is invalid
 *        - 0x01 (TRUE) – Application version string is valid
 *
 * \param appVersionLength
 *        - Length of Application Version
 *
 * \param pAppVersion
 *        - Application Version
 *        - Depends upon the Length of application Version
 *
 */

struct LocApplicationInfo
{
    BYTE appProviderLength;
    CHAR *pAppProvider;
    BYTE appNameLength;
    CHAR *pAppName;
    BYTE appVersionValid;
    CHAR appVersionLength;
    CHAR *pAppVersion;
};

/**
 * This structure contains the LOC Start Request
 *
 * \param SessionId
 *        - ID of the session as identified by the control point.
 *        - Range: 0 to 255
 *
 * \param pRecurrenceType
 *        - Specifies the type of session in which the control point
 *        is interested.
 *        - Defaults to SINGLE.
 *        -Values
 *           - 1 - Request periodic position fixes
 *           - 2 - Request a single position fix
 *
 * \param pHorizontalAccuracyLvl
 *        - Specifies the horizontal accuracy level required by the
 *        control point.
 *        - Defaults to LOW
 *        - Values
 *           - 1 - Low accuracy
 *           - 2 - Medium accuracy
 *           - 3 - High accuracy
 *
 * \param pIntermediateReportState
 *        - Specifies if the control point is interested
 *        in receiving intermediate reports.
 *        - ON by default.
 *        - Values
 *          - 1 - Intermediate reports are turned on
 *          - 2 - Intermediate reports are turned off
 *
 * \param appVersionValid
 *        - Specifies whether the application version string contains a
 *        valid value
 *
 *        - 0x00 (FALSE) – Application version string is invalid
 *        - 0x01 (TRUE) – Application version string is valid
 *
 * \param LocApplicationInfo
 *        - LOC Application Parameters
 *        - See \ref LocApplicationInfo for more information
 *
 * \param pConfigAltitudeAssumed
 *        - Configuration for Altitude Assumed Info in GNSS SV
 *        Info Event
 *        - Defaults to ENABLED.
 *        - Values
 *          - 1 - Enable Altitude Assumed information in GNSS SV
 *          Info Event
 *          - 2 - Disable Altitude Assumed information in GNSS SV
 *          Info Event
 *
 */

typedef struct
{
     BYTE    SessionId;
     ULONG   *pRecurrenceType;
     ULONG   *pHorizontalAccuracyLvl;
     ULONG   *pIntermediateReportState;
     ULONG   *pMinIntervalTime;
     struct   LocApplicationInfo *pApplicationInfo;
     ULONG   *pConfigAltitudeAssumed;
} LOCStartReq;

/**
 * This structure contains the LOC Stop Request
 *
 * \param sessionId
 *        - ID of the session as identified by the control point.
 *        - Range: 0 to 255
 */
typedef struct
{
     BYTE     sessionId;
}LOCStopReq;



/**
 * This structure contains the Delete SV Info
 *
 * \param id
 *        - SV ID of the satellite whose data is to be
 *          deleted
 *        - Range:
 *           - For GPS: 1 to 32
 *           - For SBAS: 33 to 64
 *           - For GLONASS: 65 to 96
 *
 * \param system
 *        - Indicates to which constellation this SV belongs
 *        - Valid values:
 *           - eQMI_LOC_SV_SYSTEM_GPS (1) - GPS satellite
 *           - eQMI_LOC_SV_SYSTEM_GALILEO (2) - GALILEO satellite
 *           - eQMI_LOC_SV_SYSTEM_SBAS (3) - SBAS satellite
 *           - eQMI_LOC_SV_SYSTEM_COMPASS (4) - COMPASS satellite
 *           - eQMI_LOC_SV_SYSTEM_GLONASS (5) - GLONASS satellite
 *           - eQMI_LOC_SV_SYSTEM_BDS (6) - BDS satellite
 *
 * \param mask
 *        - Indicates if the ephemeris or almanac for a satellite
 *          is to be deleted
 *        - Valid values:
 *           - 0x01 - DELETE_EPHEMERIS
 *           - 0x02 - DELETE_ALMANAC
 *
 */
typedef struct
{
    WORD    id;
    ULONG   system;
    BYTE    mask;
}SV;


/**
 * This structure contains the elements of Delete SV Info
 *
 * \param len
 *        - Number of sets of the following elements in struct SV:
 *           - gnssSvId
 *           - system
 *           - deleteSvInfoMask
 *
 * \param pSV
 *        - Pointer to struct SV. See \ref SV for more information
 *
 */

typedef struct
{
    BYTE    len;
    SV      *pSV;
}SVInfo;

/**
 * This structure contains the GNSS data
 *
 * \param mask
 *        - Mask for the GNSS data that is to be deleted
 *        - Valid values:
 *           - QMI_LOC_MASK_DELETE_GPS_SVDIR (0x00000001) - Mask to
 *             delete GPS SVDIR
 *           - QMI_LOC_MASK_DELETE_GPS_SVSTEER (0x00000002) - Mask to
 *             delete GPS SVSTEER 
 *           - QMI_LOC_MASK_DELETE_GPS_TIME (0x00000004) - Mask to
 *             delete GPS time
 *           - QMI_LOC_MASK_DELETE_GPS_ALM_CORR (0x00000008) - Mask to
 *             delete almanac correlation
 *           - QMI_LOC_MASK_DELETE_GLO_SVDIR (0x00000010) - Mask to
 *             delete GLONASS SVDIR
 *           - QMI_LOC_MASK_DELETE_GLO_SVSTEER (0x00000020) - Mask to
 *             delete GLONASS SVSTEER
 *           - QMI_LOC_MASK_DELETE_GLO_TIME (0x00000040) - Mask to
 *             delete GLONASS time
 *           - QMI_LOC_MASK_DELETE_GLO_ALM_CORR (0x00000080) - Mask to
 *             delete GLONASS almanac correlation
 *           - QMI_LOC_MASK_DELETE_SBAS_SVDIR (0x00000100) - Mask to
 *             delete SBAS SVDIR
 *           - QMI_LOC_MASK_DELETE_SBAS_SVSTEER (0x00000200) - Mask to
 *             delete SBAS SVSTEER
 *           - QMI_LOC_MASK_DELETE_POSITION (0x00000400) - Mask to
 *             delete position estimate
 *           - QMI_LOC_MASK_DELETE_TIME (0x00000800) - Mask to
 *             delete time estimate
 *           - QMI_LOC_MASK_DELETE_IONO (0x00001000) - Mask to
 *             delete IONO
 *           - QMI_LOC_MASK_DELETE_UTC (0x00002000) - Mask to
 *             delete UTC estimate
 *           - QMI_LOC_MASK_DELETE_HEALTH (0x00004000) - Mask to
 *             delete SV health record
 *           - QMI_LOC_MASK_DELETE_SADATA (0x00008000) - Mask to
 *             delete SADATA
 *           - QMI_LOC_MASK_DELETE_RTI (0x00010000) - Mask to
 *             delete RTI
 *           - QMI_LOC_MASK_DELETE_SV_NO_EXIST (0x00020000) - Mask to
 *             delete SV_NO_EXIST
 *           - QMI_LOC_MASK_DELETE_FREQ_BIAS_EST (0x00040000) - Mask to
 *             delete frequency bias estimate
 *           - QMI_LOC_MASK_DELETE_BDS_SVDIR (0x00080000) - Mask to
 *             delete BDS SVDIR
 *           - QMI_LOC_MASK_DELETE_BDS_SVSTEER (0x00100000) - Mask to
 *             delete BDS SVSTEER
 *           - QMI_LOC_MASK_DELETE_BDS_TIME (0x00200000) - Mask to
 *             delete BDS time
 *           - QMI_LOC_MASK_DELETE_BDS_ALM_CORR (0x00400000) - Mask to
 *             delete BDS almanac correlation
 *           - QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GPS (0x00800000) -
 *             Mask to delete GNSS SV blacklist GPS
 *           - QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_GLO (0x01000000) -
 *             Mask to delete GNSS SV blacklist GLO
 *           - QMI_LOC_MASK_DELETE_GNSS_SV_BLACKLIST_BDS (0x02000000) -
 *             Mask to delete GNSS SV blacklist BDS
 *
 */
typedef struct
{
    ULONGLONG   mask;
}GnssData;

/**
 * This structure contains the cell database
 *
 * \param mask
 *        - Mask for the cell database assistance data that is to be deleted
 *        - Valid values:
 *           - 0x00000001 - DELETE_CELLDB_POS
 *           - 0x00000002 - DELETE_CELLDB_LATEST_GPS_POS
 *           - 0x00000004 - DELETE_CELLDB_OTA_POS
 *           - 0x00000008 - DELETE_CELLDB_EXT_REF_POS
 *           - 0x00000010 - DELETE_CELLDB_TIMETAG
 *           - 0x00000020 - DELETE_CELLDB_CELLID
 *           - 0x00000040 - DELETE_CELLDB_CACHED_CELLID
 *           - 0x00000080 - DELETE_CELLDB_LAST_SRV_CELL
 *           - 0x00000100 - DELETE_CELLDB_CUR_SRV_CELL
 *           - 0x00000200 - DELETE_CELLDB_NEIGHBOR_INFO
 *
 */
typedef struct
{
    ULONG   mask;
}CellDb;

/**
 * This structure contains the clock Info
 *
 * \param mask
 *        - Mask for the clock information assistance data that is to be deleted
 *        - Valid bitmasks:
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST (0x00000001) - Mask to
 *             delete time estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST (0x00000002) - Mask to
 *             delete frequency estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER (0x00000004) - Mask to
 *             delete week number from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME (0x00000008) - Mask to
 *             delete RTC time from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER (0x00000010) - Mask to
 *             delete time transfer from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST (0x00000020) - Mask to
 *             delete GPS time estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST (0x00000040) - Mask to
 *             delete GLONASS time estimate from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER (0x00000080) - Mask to
 *             delete GLONASS day number from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER (0x00000100) - Mask to
 *             delete GLONASS four year number from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY (0x00000200) - Mask to
 *             delete GLONASS RF GRP delay from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT (0x00000400) - Mask to
 *             delete disable TT from clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_LEAPSEC (0x00000800) - Mask to
 *             delete a BDS time estimate from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GG_GGTB (0x00001000) - Mask to
 *             delete a BDS time estimate from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSTIME_EST (0x00002000) - Mask to
 *             delete a BDS time estimate from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_GB_GBTB (0x00004000) - Mask to delete
 *             Glonass-to-BDS time bias-related information from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BG_BGTB (0x00008000) - Mask to delete
 *             BDS-to-GLONASS time bias-related information from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDSWEEK_NUMBER (0x00010000) - Mask to
 *             delete the BDS week number from the clock information
 *           - QMI_LOC_MASK_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY (0x00020000) - Mask to
 *             delete the BDS RF GRP delay from the clock information
 *
 */

typedef struct
{
    ULONG   mask;
}ClkInfo;

/**
 * This structure contains the BDS SV Info
 *
 * \param id
 *        - SV ID of the satellite whose data is to be deleted.
 *           - Range for BDS: 201 to 237
 *
 * \param mask
 *        - Indicates if the ephemeris or almanac for a satellite is to be deleted
 *        - Valid values:
 *           - QMI_LOC_MASK_DELETE_EPHEMERIS (0x01) - Delete ephemeris for the satellite
 *           - QMI_LOC_MASK_DELETE_ALMANAC (0x02) - Delete almanac for the satellite
 *
*/
typedef struct
{
    WORD    id;
    BYTE    mask;
}BdsSV;

/**
 * This structure contains the number of sets of the BDS SVN Info
 *
 * \param len
 *        - Number of sets of the following elements:
 *           - gnssSvId
 *           - deleteSvInfoMask
 *
 * \param pSV
 *        - Pointer to struct BdsSV. See \ref BdsSV for more information
 *
*/
typedef struct
{
    BYTE    len;
    BdsSV   *pSV;
}BdsSVInfo;



/**
 * This structure contains LOC delete assist data request
 *
 * \param pSVInfo
 *        - Pointer to struct SVInfo. See \ref SVInfo for more information
 *
 * \param pGnssData
 *        - Pointer to struct GnssData. See \ref GnssData for more information
 *
 * \param pCellDb
 *        - Pointer to struct CellDb. See \ref CellDb for more information
 *
 * \param pClkInfo
 *        - Pointer to struct ClkInfo. See \ref ClkInfo for more information
 *
 * \param pBdsSVInfo
 *        - Pointer to struct BdsSVInfo. See \ref BdsSVInfo for more information
 *
 */

typedef struct
{
    SVInfo *pSVInfo;
    GnssData *pGnssData;
    CellDb *pCellDb;
    ClkInfo *pClkInfo;
    BdsSVInfo *pBdsSVInfo;
}LocDelAssDataReq;


/**
 * This structure contains the LOC Get Server Request
 *
 * \param serverType
 *        - Type of server, as defined in qmiLocServerTypeEnumT.
 *          Valid values:
 *          -0x00000001 – CDMA_PDE
 *          -0x00000002 – CDMA_MPC
 *          -0x00000003 – UMTS_SLP
 *          -0x00000004 – CUSTOM_PDE
 *
 * \param pAddressType
 *        - Type of address the client wants. If unspecified, the indication will contain
 *           all the types of addresses it has for the specified server type.
 *           Valid bitmasks:
 *           - 0x01 – IPV4
 *           - 0x02 – IPV6
 *           - 0x04 – URL
 */

typedef struct
{
     ULONG    serverType;
     BYTE    *pAddressType;
} LOCGetServerReq;


/**
 * This structure contains IPV4 address and port.
 *
 * \param address
 *        - IPV4 address
 *
 * \param port
 *        - IPV4 port
 *
*/
typedef struct
{
    ULONG    ipv4Addr;
    WORD     port;
}IPv4Address;

/**
 * This structure contains IPV6 address and port.
 *
 * \param address
 *        - IPV6 address
 *
 * \param port
 *        - IPV6 port
 *
*/
typedef struct
{
    USHORT  ipv6Addr[8];
    ULONG    port;
}IPv6Address;

/**
 * This structure contains URL address.
 *
 * \param url
 *        - URL address
*/
typedef struct
{
    CHAR  url[256];
}UrlAddress;

/**
 * This structure contains the LOC Set Server Request
 *
 * \param serverType
 *        - Type of server, as defined in qmiLocServerTypeEnumT.
 *          Valid values:
 *          -0x00000001 – CDMA_PDE
 *          -0x00000002 – CDMA_MPC
 *          -0x00000003 – UMTS_SLP
 *          -0x00000004 – CUSTOM_PDE
 *
 * \param pIPv4Address
 *        - Pointer to struct IPv4Address. See \ref IPv4Address for more information
 *
 * \param pIPv6Address
 *        - Pointer to struct pIPv6Address. See \ref pIPv6Address for more information
 *
 * \param pURLAddress
 *        - URL address. NULL-terminated string and maximum string length (including
 *          NULL terminator): 256
 *
 */

typedef struct
{
    ULONG           serverType;
    IPv4Address     *pIPv4Address;
    IPv6Address     *pIPv6Address;
    UrlAddress      *pURLAddress;
} LOCSetServerReq;

/**
 *  Used by the control point to register for events from the location subsystem.
 *
 *  \param  pLOCEventRegisterReqResp[IN]
 *          - See \ref LOCEventRegisterReqResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */

ULONG SLQSLOCEventRegister (
        LOCEventRegisterReqResp *pLOCEventRegisterReqResp );
/**
 *  Used by the control point to set the current external power configuration.
 *
 *  \param  pLOCExtPowerStateReqResp[IN]
 *          - See \ref LOCExtPowerStateReqResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */

ULONG SLQSLOCSetExtPowerState(
		LOCExtPowerStateReqResp *pLOCExtPowerStateReqResp);

/**
 *  Used by the control point to initiate a GPS session.
 *
 *  \param  pLOCStartReqReq[IN]
 *          - See \ref LOCStartReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */

ULONG SLQSLOCStart(LOCStartReq *pLOCStartReq);
/**
 *  Used by the control point to stop a GPS session.
 *
 *  \param  pLOCStopReqResp[IN]
 *          - See \ref LOCStopReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */

ULONG SLQSLOCStop(LOCStopReq *pLOCStopReq);

/**
 *  Used by the control point to tells the engine to use the specified operation mode
 *  while making the position fixes
 *
 *  \param  mode[IN]
 *          - Valid values:
 *             - eQMI_LOC_OPER_MODE_DEFAULT (1) - Use the default engine mode
 *             - eQMI_LOC_OPER_MODE_MSB (2) - Use the MS-based mode
 *             - eQMI_LOC_OPER_MODE_MSA (3) - Use the MS-assisted mode
 *             - eQMI_LOC_OPER_MODE_STANDALONE (4) - Use Standalone mode
 *             - eQMI_LOC_OPER_MODE_CELL_ID (5) - Use cell ID; this mode is
 *               only valid for GSM/UMTS networks
 *             - eQMI_LOC_OPER_MODE_WWAN (6) - Use WWAN measurements to calculate
 *               the position; if this mode is set, AFLT will be used for 1X
 *               networks and OTDOA will be used for LTE networks
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCSetOpMode(ULONG mode);

/**
 *  Used by the control point to inject UTC time 
 *
 *  \param  request[IN]
 *          - Input a NULL pointer to delete all assistance data. Otherwise, specify optional fields
 *            to be deleted. See \ref LocDelAssDataReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */

ULONG SLQSLOCDelAssData(LocDelAssDataReq request);

/**
 *  Used by the control point to delete the location engine assistance data
 *
 *  \param  timeMsec[IN]
 *          - The UTC time since Jan. 1, 1970
 *
 *  \param  timeUncMsec[IN]
 *           - The time Uncertainty
 *           
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCInjectUTCTime(ULONGLONG timeMsec, ULONG timeUncMsec);

/**
 *  Used by the control point to get the location server from the location engine
 *
 *  \param  request[IN]
 *          - See \ref LOCGetServerReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCGetServer(LOCGetServerReq request);

/**
 *  Used by the control point to specify the A-GPS server type and address
 *
 *  \param  request[IN]
 *          - See \ref LOCSetServerReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCSetServer(LOCSetServerReq request);

/**
 *  Used by the control point to get current operation mode
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCGetOpMode();

/**
 *  Used by the control point to get fix criteria
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCGetFixCriteria();

/**
 * This structure contains SWI LOC Get Auto Start setting
 *
 * \param function
 *        - Setting to indicate when modem should start an automatic GNSS fix
 *          - 0 - disabled
 *          - 1 - At bootup
 *          - 2 - When NMEA port is opened
 *
 * \param function_reported
 *        - 0 - not reported by modem
 *        - 1 - reported by modem
 *
 * \param fix_type
 *        - Type of GNSS fix:
 *          - 1 - Default Engine mode
 *          - 2 - MS-Based
 *          - 3 - MS-Assisted
 *          - 4 - Standalone
 *
 * \param fix_type_reported
 *        - 0 - not reported by modem
 *        - 1 - reported by modem
 *
 * \param max_time
 *        - Maximum time allowed for the receiver to get a fix in seconds
 *        - Valid range: 1-255
 *
 * \param max_time_reported
 *        - 0 - not reported by modem
 *        - 1 - reported by modem
 *
 * \param max_dist
 *        - Maximum uncertainty of a fix measured by distance in meters
 *        - Valid range: 1 - 4294967280
 *
 * \param max_dist_reported
 *        - 0 - not reported by modem
 *        - 1 - reported by modem
 *
 * \param fix_rate
 *        - Time between fixes in seconds
 *        - Valid range: 1--65535
 *
 * \param fix_rate_reported
 *        - 0 - not reported by modem
 *        - 1 - reported by modem
 */
typedef struct
{
    BYTE function;
    BOOL function_reported;
    BYTE fix_type;
    BOOL fix_type_reported;
    BYTE max_time;
    BOOL max_time_reported;
    ULONG max_dist;
    BOOL max_dist_reported;
    ULONG fix_rate;
    BOOL fix_rate_reported;
} SwiLocGetAutoStartResp;

/**
 *  Used by the control point to Get Loc Auto Start settings
 *
 *  \param  resp[OUT]
 *          - See \ref SwiLocGetAutoStartResp for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 */
ULONG SwiLocGetAutoStart(SwiLocGetAutoStartResp *resp);

/**
 * This structure contains SWI LOC Get Auto Start setting
 *
 * \param function
 *        - Setting to indicate when modem should start an automatic GNSS fix
 *          - 0 - disabled
 *          - 1 - At bootup
 *          - 2 - When NMEA port is opened
 *
 * \param set_function
 *        - 0 - do not set to modem
 *        - 1 - set to modem
 *
 * \param fix_type
 *        - Type of GNSS fix:
 *          - 1 - Default Engine mode
 *          - 2 - MS-Based
 *          - 3 - MS-Assisted
 *          - 4 - Standalone
 *
 * \param set_fix_type
 *        - 0 - do not set to modem
 *        - 1 - set to modem
 *
 * \param max_time
 *        - Maximum time allowed for the receiver to get a fix in seconds
 *        - Valid range: 1-255
 *
 * \param set_max_time
 *        - 0 - do not set to modem
 *        - 1 - set to modem
 *
 * \param max_dist
 *        - Maximum uncertainty of a fix measured by distance in meters
 *        - Valid range: 1 - 4294967280
 *
 * \param set_max_dist
 *        - 0 - do not set to modem
 *        - 1 - set to modem
 *
 * \param fix_rate
 *        - Time between fixes in seconds
 *        - Valid range: 1--65535
 *
 * \param set_fix_rate
 *        - 0 - do not set to modem
 *        - 1 - set to modem
 */
typedef struct
{
    BYTE function;
    BOOL set_function;
    BYTE fix_type;
    BOOL set_fix_type;
    BYTE max_time;
    BOOL set_max_time;
    ULONG max_dist;
    BOOL set_max_dist;
    ULONG fix_rate;
    BOOL set_fix_rate;
} SwiLocSetAutoStartReq;

/**
 *  Used by the control point to Set Loc Auto Start settings
 *
 *  \param  req[IN]
 *          - See \ref SwiLocSetAutoStartReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 */
ULONG SwiLocSetAutoStart(SwiLocSetAutoStartReq *req);

/**
 * This structure specifies information regarding the altitude source
 *
 * \param source
 *        - Specifies the source of the altitude
 *        - Valid values
 *           - 0 - Source is unknown
 *           - 1 - GPS is the source
 *           - 2 - Cell ID provided the source
 *           - 3 - Source is enhanced cell ID
 *           - 4 - Wi-Fi is the source
 *           - 5 - Terrestrial source
 *           - 6 - Hybrid terrestrial source
 *           - 7 - Altitude database is the source
 *           - 8 - Barometric altimeter is the source
 *           - 9 - Other sources
 *
 * \param linkage
 *        - Specifies the dependency between the horizontal
 *          and altitude position components
 *        - Valid values
 *           - 0 - Not specified
 *           - 1 - Fully interdependent
 *           - 2 - Depends on latitude and longitude
 *           - 3 - Fully independent
 *
 * \param coverage
 *        - Specifies the region of uncertainty.
 *        - Valid values
 *           - 0 - Not specified
 *           - 1 - Altitude uncertainty is valid at the injected
 *                 horizontal position coordinates only
 *           - 2 - Altitude uncertainty applies to the position
 *                 of the device regardless of horizontal position
 *
*/
typedef struct
{
    ULONG    source;
    ULONG    linkage;
    ULONG    coverage;
}altitudeSrcInfo;

/**
 *  This structure contains LOC Inject Position parameters
 *
 *  \param  pLatitude
 *             - Optional parameter
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -90.0 to 90.0
 *             - Positive values indicate northern latitude
 *             - Negative values indicate southern latitude
 *
 *             - Note - This field must be specified together with
 *                        pLongitude and pHorUncCircular.
 *
 *  \param  pLongitude
 *             - Optional parameter
 *             - Type - Floating point
 *             - Units - Degrees
 *             - Range - -180.0 to 180.0
 *             - Positive values indicate eastern latitude
 *             - Negative values indicate western latitude
 *
 *             - Note - This field must be specified together with
 *                        pLatitude and pHorUncCircular.
 *
 *   \param  pHorUncCircular
 *             - Optional parameter
 *             - Horizontal position uncertainty.
 *             - Units - Meters
 *
 *             - Note - This field must be specified together with
 *                        pLatitude and pLongitude.
 *
 *   \param  pHorConfidence
 *             - Optional parameter
 *             - Horizontal confidence.
 *             - Units - Percent
 *             - Values
 *               - Valid Values - 1 to 99
 *               - Invalid Values - 0, 101 to 255
 *               - If 100 is received, reinterpret to 99
 *
 *             - Note - This field must be specified together with
 *                      horizontal uncertainty. If not specified
 *                      when pHorUncCircular is set, the default
 *                      value is 50.
 *
 *   \param  pHorReliability
 *             - Optional parameter
 *          - Values
 *            - 0 - Location reliability is not set.
 *            - 1 - Location reliability is very low; use it at
 *                  your own risk
 *            - 2 - Location reliability is low; little or no
 *                  cross-checking is possible.
 *            - 3 - Location reliability is medium; limited cross-check passed
 *            - 4 - Location reliability is high; strong cross-check passed
 *
 *   \param  pAltitudeWrtEllipsoid
 *             - Optional parameter
 *             - Altitude With Respect to Ellipsoid.
 *             - Units - Meters
 *             - Values
 *               - Positive - height
 *               - Negative = depth
 *
 *   \param  pAltitudeWrtMeanSeaLevel
 *             - Optional parameter
 *             - Altitude With Respect to Sea Level.
 *             - Units - Meters
 *
 *   \param  pVertUnc
 *             - Optional parameter
 *             - Vertical uncertainty.
 *             - Units - Meters
 *
 *             - Note - This is mandatory if either pAltitudeWrtEllipsoid
 *                      or pAltitudeWrtMeanSeaLevel is specified.
 *
 *   \param  pVertConfidence
 *             - Optional parameter
 *             - Vertical confidence.
 *             - Units - Percentage
 *             - Values
 *               - Valid Values - 0 to 99
 *               - Invalid Values - 0, 100-256
 *               - If 100 is received, reinterpret to 99
 *
 *             - Note - This field must be specified together with
 *                      the vertical uncertainty. If not specified,
 *                      the default value will be 50.
 *
 *   \param  pVertReliability
 *             - Optional parameter
 *          - Values
 *             - 0 - Location reliability is not set.
 *             - 1 - Location reliability is very low;
 *                   use it at your own risk.
 *             - 2 - Location reliability is low; little or no
 *                   cross-checking is possible
 *             - 3 - Location reliability is medium; limited
 *                   cross-check passed
 *             - 4 - Location reliability is high; strong
 *                   cross-check passed
 *
 *   \param  pAltitudeSrcInfo
 *             - Optional parameter
 *          - Pointer to struct altitudeSrcInfo. See \ref altitudeSrcInfo for more information
 *
 *   \param  pTimestampUtc
 *             - Optional parameter
 *             - UTC timestamp
 *             - Units - Milliseconds since Jan. 1, 1970
 *
 *   \param  pTimestampAge
 *             - Optional parameter
 *             - Position age, which is an estimate of how long ago
 *               this fix was made.
 *             - Units - Milliseconds
 *
 *   \param  pPositionSrc
 *             - Optional parameter
 *             - Source from which this position was obtained
 *             - Valid values
 *               - 0 - Position source is GNSS
 *               - 1 - Position source is Cell ID
 *               - 2 - Position source is Enhanced Cell ID
 *               - 3 - Position source is Wi-Fi
 *               - 4 - Position source is Terrestrial
 *               - 5 - Position source is GNSS Terrestrial Hybrid
 *               - 6 - Other sources
 *
 *             - Note - If altitude is specified and the altitude
 *                      source is not specified, the engine assumes
 *                      that the altitude was obtained using the
 *                      specified position source.
 *                    - If both altitude and altitude source are
 *                      specified, the engine assumes that only
 *                      latitude and longitude were obtained using
 *                      the specified position source.
 *
 *   \param  pRawHorUncCircular
 *             - Optional parameter
 *             - Horizontal position uncertainty (circular) without
 *               any optimization.
 *             - Units - Meters
 *
 *   \param  pRawHorConfidence
 *             - Optional parameter
 *             - Horizontal confidence associated with raw horizontal
 *               uncertainty
 *             - Units: Percent
 *             - Values
 *               - Valid values - 1 to 99
 *               - Invalid values - 0, 101 to 255
 *               - If 100 is received, reinterpret to 99
 *
 *             - Note - This field must be specified together with
 *                      raw horizontal uncertainty. If not specified
 *                      when rawHorUncCircular is set, the default value is 50.
 */
typedef struct
{
    ULONGLONG               *pLatitude;
    ULONGLONG               *pLongitude;
    ULONG                   *pHorUncCircular;
    BYTE                    *pHorConfidence;
    ULONG                   *pHorReliability;
    ULONG                   *pAltitudeWrtEllipsoid;
    ULONG                   *pAltitudeWrtMeanSeaLevel;
    ULONG                   *pVertUnc;
    BYTE                    *pVertConfidence;
    ULONG                   *pVertReliability;
    altitudeSrcInfo         *pAltitudeSrcInfo;
    ULONGLONG               *pTimestampUtc;
    ULONG                   *pTimestampAge;
    ULONG                   *pPositionSrc;
    ULONG                   *pRawHorUncCircular;
    BYTE                    *pRawHorConfidence;
} LocInjectPositionReq;

/**
 *  Injects a position to the location engine.
 *
 *  \param  pLocInjectPositionReq[IN]
 *          - See \ref LocInjectPositionReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCInjectPosition(LocInjectPositionReq *pLocInjectPositionReq);

/**
 *  This structure contains parameters to set current cradle mount configuration
 *
 *  \param  state
 *             - Cradle Mount State
 *             - Valid values:
 *               - 0 - Device is mounted on the cradle
 *               - 1 - Device is not mounted on the cradle
 *               - 2 - Unknown cradle mount state
 *
 *  \param  pConfidence
 *             - Cradle Mount Confidence (Optional)
 *             - Confidence in the Cradle Mount state expressed as a
 *               percentage.
 *             - Range - 0 to 100
 */
typedef struct
{
    ULONG               state;
    BYTE               *pConfidence;
} LocSetCradleMountReq;

/**
 *  Control point to set the current cradle mount configuration.
 *
 *  \param  pLocSetCradleMountReq[IN]
 *          - See \ref LocSetCradleMountReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCSetCradleMountConfig(LocSetCradleMountReq *pLocSetCradleMountReq);

/**
 * This structure specifies information regarding the 3-Axis Sensor Data.
 *
 * \param timeOfFirstSample
 *        - Denotes a full 32-bit time stamp of the first (oldest) sample in this message.
 *        - The time stamp is in the time reference scale that is used by the sensor time source.
 *        - Units - Milliseconds
 *
 * \param flags
 *        - Flags to indicate any deviation from the default measurement assumptions.
 *        - All unused bits in this field must be set to 0.
 *        - Valid bitmasks
 *           - 0x01 - Bitmask to specify that a sign reversal is required
 *                    while interpreting the sensor data; only applies to the
 *                    accelerometer samples
 *           - 0x02 - Bitmask to specify that the sensor time stamp is the same
 *                    as the modem time stamp
 *
 * \param sensorDataLen
 *        - Number of sets of the following elements
 *          - timeOffset
 *          - xAxis
 *          - yAxis
 *          - zAxis
 *
 * \param timeOffset
 *        - Sample time offset
 *        - Units - Milliseconds
 *
 * \param xAxis
 *        - Sensor x-axis sample.
 *        - Units Accelerometer - Meters/seconds square
 *        - Units Gyroscope - Radians/second
 *
 * \param yAxis
 *        - Sensor Y-axis sample.
 *        - Units Accelerometer - Meters/seconds square
 *        - Units Gyroscope - Radians/second
 *
 * \param xAxis
 *        - Sensor Z-axis sample.
 *        - Units Accelerometer - Meters/seconds square
 *        - Units Gyroscope - Radians/second
 */
typedef struct
{
    ULONG    timeOfFirstSample ;
    BYTE     flags;
    BYTE     sensorDataLen;
    WORD     timeOffset[MAX_SENSOR_DATA_LEN];
    ULONG    xAxis[MAX_SENSOR_DATA_LEN];
    ULONG    yAxis[MAX_SENSOR_DATA_LEN];
    ULONG    zAxis[MAX_SENSOR_DATA_LEN];
}sensorData;

/**
 * This structure specifies information regarding the Temperature Data.
 *
 * \param timeSource
 *        - Time source of the sensor data
 *        - Valid values
 *          - 0 - Sensor time source is unspecified
 *          - 1 - Time source is common between the sensors and
 *                the location engine
 *
 * \param timeOfFirstSample
 *        - Denotes a full 32-bit time stamp of the first (oldest) sample in this message.
 *        - The time stamp is in the time reference scale that is used by the sensor time source.
 *        - Units - Milliseconds
 *
 * \param temperatureDataLen
 *        - Number of sets of the following elements
 *          - timeOffset
 *          - temperature
 *
 * \param timeOffset
 *        - Sample time offset
 *        - Units - Milliseconds
 *
 * \param temperature
 *        - Sensor temperature.
 *        - Type - Floating point
 *        - Units - Degrees Celsius
 *        - Range -50 to +100.00
 */
typedef struct
{
    ULONG    timeSource;
    ULONG    timeOfFirstSample;
    BYTE     temperatureDataLen;
    WORD     timeOffset[MAX_TEMP_DATA_LEN];
    ULONG    temperature[MAX_TEMP_DATA_LEN];
}tempratureData;

/**
 *  This structure contains parameters to inject sensor data into the GNSS location engine
 *
 *  \param  pOpaqueIdentifier
 *             - Opaque Identifier (Optional parameter)
 *             - An opaque identifier that is sent in by the client that
 *               will be echoed in the indication so the client can relate
 *               the indication to the request.
 *
 *   \param  pAcceleroData
 *             - Optional parameter
 *             - Pointer to struct sensorData. See \ref sensorData for more information
 *
 *   \param  pGyroData
 *             - Optional parameter
 *             - Pointer to struct sensorData. See \ref sensorData for more information
 *
 *  \param  pAcceleroTimeSrc
 *             - 3-Axis Accelerometer Data Time Source (Optional parameter)
 *             - The location service uses this field to identify the time reference
 *               used in the accelerometer data time stamps.
 *             - If not specified, the location service assumes that the time source
 *               for the accelerometer data is unknown.
 *             - Valid values
 *                - 0 - Sensor time source is unspecified
 *                - 1 - Time source is common between the sensors and
 *                      the location engine
 *
 *  \param  pGyroTimeSrc
 *             - 3-Axis Gyroscope Data Time Source (Optional)
 *             - The location service uses this field to identify the time reference
 *               used in the gyroscope data time stamps.
 *             - If not specified, the location service assumes that the time source
 *               for the gyroscope data is unknown.
 *             - Valid values
 *                - 0 - Sensor time source is unspecified
 *                - 1 - Time source is common between the sensors and
 *                      the location engine
 *
 *   \param  pAcceleroTempData
 *             - Optional parameter
 *             - Pointer to struct tempratureData. See \ref tempratureData for more information
 *
 *   \param  pGyroTempData
 *             - Optional parameter
 *             - Pointer to struct tempratureData. See \ref tempratureData for more information
 *
 */
typedef struct
{
    ULONG                *pOpaqueIdentifier;
    sensorData           *pAcceleroData;
    sensorData           *pGyroData;
    ULONG                *pAcceleroTimeSrc;
    ULONG                *pGyroTimeSrc;
    tempratureData       *pAcceleroTempData;
    tempratureData       *pGyroTempData;
} LocInjectSensorDataReq;

/**
 *  Control point to to inject sensor data into the GNSS location engine.
 *
 *  \param  pLocInjectSensorDataReq[IN]
 *          - See \ref LocInjectSensorDataReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 seconds
 *
 */
ULONG SLQSLOCInjectSensorData(LocInjectSensorDataReq *pLocInjectSensorDataReq);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_LOC_H__ */
