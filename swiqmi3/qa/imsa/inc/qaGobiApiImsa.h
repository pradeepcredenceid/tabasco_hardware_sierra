/**
 * \ingroup ims
 *
 * \file    qaGobiApiImsa.h
 *
 * \brief   IMSA Service API function prototypes
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_IMSA_H__
#define __GOBI_API_IMSA_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This structure contains parameters of IMSA Config Indication Register
 *
 *  \param  pRegStatusConfig(optional)
 *          - Register Indication For Registration status.
 *          - When this registration is enabled, the device learns of Registration
 *            status via the QMI_IMSA_REGISTRATION_STATUS_IND
 *            indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  pServiceStatusConfig(optional)
 *          - Register Indication For Service status
 *            Events.
 *          - When this registration is enabled, the device learns of Service 
 *            status via the QMI_IMSA_SERVICE_STATUS_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \param  pRatHandoverStatusConfig(optional)
 *          - Registration Indication For RAT handover status.
 *          - When this registration is enabled, the device learns of RAT handover
 *            status via the QMI_IMSA_RAT_HANDOVER_STATUS_IND indication.
 *              - 0x00 - Disable
 *              - 0x01 - Enable
 *
 *  \note   One of the optional parameter is mandatory to be present in the
 *          request.
 */
typedef struct
{
    BYTE *pRegStatusConfig;
    BYTE *pServiceStatusConfig;
    BYTE *pRatHandoverStatusConfig;
} IMSAIndRegisterInfo;

/**
 *  Sets the registration state for different QMI_IMSA indications for the
 *  requesting control point
 *
 *  \param  pImsaIndRegisterInfo[IN]
 *          - Structure containing Indication Register Information.
 *              - See \ref IMSAIndRegisterInfo for more informtaion.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Timeout: 5 Secs
 *
 *          This API is used by a device to register/deregister for different
 *          QMI IMSA indications.
 *          The device's registration state variables that control registration
 *          for indications will be modified to reflect the settings indicated in the
 *          request message.
 *          At least one optional parameter must be present in the request.
 *
 */
ULONG SLQSRegisterIMSAIndication (
    IMSAIndRegisterInfo *pImsaIndRegisterInfo );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_IMSA_H__ */
