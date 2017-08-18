/*
 * \ingroup cbk
 *
 * \file qaCbkDmsSwiEventReportInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkDmsSwiEventReportInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __DMS_SWI_EVENT_REPORT_IND_H__
#define __DMS_SWI_EVENT_REPORT_IND_H__

/*
 * An enumeration of eQMI_DMS_SWI_EVENT_REPORT_IND response TLV IDs
 *
 */
enum eQMI_DMS_SWI_EVENT_REPORT_IND
{
    eTLV_IND_TEMPERATURE = 0x10,
    eTLV_IND_VOLTAGE     = 0x11,
};

/*
 * eQMI_DMS_SWI_EVENT_IND TLVs defined below
 */

/*
 * Name:    TemperatureTlv
 *
 * Purpose: Structure used to store Temperature TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          TempState   - Temperature State
 *
 *          Temperature - Temperature( in degree celsius )
 */
struct TemperatureTlv
{
    BYTE TlvPresent;
    BYTE TempState;
    WORD Temperature;
};

/*
 * Name:    VoltageTlv
 *
 * Purpose: Structure used to store Voltage TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          VoltState  - Voltage State
 *
 *          Voltage    - Voltage (in mV)
 */
struct VoltageTlv
{
    BYTE TlvPresent;
    BYTE VoltState;
    WORD Voltage;
};

/*
 * Name:    QmiCbkDmsSwiEventStatusReportInd
 *
 * Purpose: Structure used to store all DMS SWI Event Report Notification
 *          Parameters.
 *
 * Members: TempTlv - Temperature Tlv
 *          VoltTlv - Voltage Tlv
 *
 * Note:    None
 */
struct QmiCbkDmsSwiEventStatusReportInd{
    struct TemperatureTlv TempTlv;
    struct VoltageTlv     VoltTlv;
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkDmsSwiEventReportInd(
    BYTE                                    *pMdmResp,
    struct QmiCbkDmsSwiEventStatusReportInd *pApiResp );

#endif /* __DMS_SWI_EVENT_REPORT_IND_H__ */

