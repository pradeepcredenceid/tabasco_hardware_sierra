/*   ----------------------------------------------------------------------
     Copyright (C) 2014 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     Copyright (C) 2009 - 2010 Infineon Technologies Denmark A/S (IFWD)
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/inc/psi_std_errors.h $
       $$CC-Version: .../oint_tools_flashtoole2_01/1 $
       $$Date:       2014-03-13    15:39:14 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */
#ifndef __PSI_STD_ERROR_H__
#define __PSI_STD_ERROR_H__

#include "IMC_bootcore_strings.h"

//#define CHECKSUM_SIZE_BYTES    (uint8_t)4
#define PSI_ACK               (uint16_t)0xAAAA
#define DUT_TEST_MODE         (uint16_t)0x1111
#define PSI_ACK_EXT           (uint16_t)0xAA00
#define PSI_NACK              (uint16_t)0xEE00
#define PSI_ACK_ENHANCED      (uint16_t)0xDD01
#define PSI_LENGTH_ACK        (uint16_t)0xCCCC
#define PSI_CHECKSUM_OK        (uint8_t)0xA5
#define PSI_CHECKSUM_NOTOK     (uint8_t)0x5A

// Extracts the packet type (uint16_t)(first two data bytes)
#define PSI_PACKET_TYPE( packet ) (packet.buffer[0] | packet.buffer[1]<<8)

#endif // __PSI_STD_ERROR_H__
