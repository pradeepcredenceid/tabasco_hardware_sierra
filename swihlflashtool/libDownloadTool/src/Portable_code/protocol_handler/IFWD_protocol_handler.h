 /*
 * Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *      Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Technologies Denmark A/S. Passing on and copying of this document, use and
 * communication of its contents is not permitted without prior written
 * authorisation.
 *
 * Description:
 *   IFWD protocol handler - code
 *   This is the header file for the  protocol handler module which is
 *   responsible for the data transfer between the upper layers and the target.
 *   It is responsible for framing and de-framing the package according to the
 *   protocol used. the The protocol used will be transparent for the upper
 *   layers.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/protocol_handler/IFWD_protocol_handler.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/7 $
 *  $Date:       2014-03-13    15:41:02 UTC $
 *   Responsible: kofod
 *   Comment:
 *     SMS00934737 Solution for certificate flashing
 */

#if defined (TEXT_CLASS_IFWD_ERRORS)
 TEXT_DEF(PROT_ERR_channel_is_outside_range,       "Channel number %d is outside the valid range")                /* 01 */
 TEXT_DEF(PROT_ERR_channel_does_not_exist,         "Cannot delete the non-existing Instance of the channel %d")   /* 02 */
 TEXT_DEF(PROT_ERR_invalid_channel_instance,       "channel instance %d does not exist")                          /* 03 */
 TEXT_DEF(PROT_ERR_malloc,                         "Could not allocate memory")                                   /* 04 */
 TEXT_DEF(PROT_ERR_tx_pack_timeout,                "Timeout occured during packet transmission")                  /* 05 */
 TEXT_DEF(PROT_ERR_rx_pack_timeout,                "Timeout occured during packet reception")                     /* 06 */
 TEXT_DEF(PROT_ERR_tx_pack_error,                  "Error occured during packet transmission")                    /* 07 */
 TEXT_DEF(PROT_ERR_rx_pack_error,                  "Error occured during packet reception")                       /* 08 */
 TEXT_DEF(PROT_ERR_tx_ongoing,                     "Handler currently in transmit state")                         /* 09 */
 TEXT_DEF(PROT_ERR_rx_ongoing,                     "Handler currently in receive state")                          /* 10 */
 TEXT_DEF(PROT_ERR_unknown,                        "Unknown error in the communication")                          /* 11 */
 TEXT_DEF(PROT_ERR_handler,                        "Protocol Handler is in error state")                          /* 12 */
 TEXT_DEF(PROT_ERR_device_error,                   "Device is not booted")                                        /* 13 */
 TEXT_DEF(PROT_ERR_platform_unknown,               "Platform not recognized")                                     /* 14 */
 TEXT_DEF(PROT_ERR_free,                           "Could not free memory")                                       /* 15 */
 TEXT_DEF(PROT_ERROR_target,                       "Unrecoverable error in EBL target device")                                                            /* 16 */
 TEXT_DEF(PROT_ERR_spare_17,                       "")                                                            /* 17 */
 TEXT_DEF(PROT_ERR_spare_18,                       "")                                                            /* 18 */
 TEXT_DEF(PROT_ERR_spare_19,                       "")                                                            /* 19 */
 TEXT_DEF(PROT_ERR_spare_20,                       "")                                                            /* 20 */
 TEXT_DEF(PROT_ERR_spare_21,                       "")                                                            /* 21 */
 TEXT_DEF(PROT_ERR_spare_22,                       "")                                                            /* 22 */
 TEXT_DEF(PROT_ERR_spare_23,                       "")                                                            /* 23 */
 TEXT_DEF(PROT_ERR_spare_24,                       "")                                                            /* 24 */
 TEXT_DEF(PROT_ERR_spare_25,                       "")                                                            /* 25 */
 TEXT_DEF(PROT_ERR_spare_26,                       "")                                                            /* 26 */
 TEXT_DEF(PROT_ERR_spare_27,                       "")                                                            /* 27 */
 TEXT_DEF(PROT_ERR_spare_28,                       "")                                                            /* 28 */
 TEXT_DEF(PROT_ERR_spare_29,                       "")                                                            /* 29 */
 TEXT_DEF(PROT_ERR_spare_30,                       "")                                                            /* 30 */
 TEXT_DEF(PROT_ERR_spare_31,                       "")                                                            /* 31 */
 TEXT_DEF(PROT_ERR_spare_32,                       "")                                                            /* 32 */
 TEXT_DEF(PROT_ERR_spare_33,                       "")                                                            /* 33 */
 TEXT_DEF(PROT_ERR_spare_34,                       "")                                                            /* 34 */
 TEXT_DEF(PROT_ERR_spare_35,                       "")                                                            /* 35 */
 TEXT_DEF(PROT_ERR_spare_36,                       "")                                                            /* 36 */
 TEXT_DEF(PROT_ERR_spare_37,                       "")                                                            /* 37 */
 TEXT_DEF(PROT_ERR_spare_38,                       "")                                                            /* 38 */
 TEXT_DEF(PROT_ERR_spare_39,                       "")                                                            /* 39 */
 TEXT_DEF(PROT_ERR_spare_40,                       "")                                                            /* 40 */
 TEXT_DEF(PROT_ERR_spare_41,                       "")                                                            /* 41 */
 TEXT_DEF(PROT_ERR_spare_42,                       "")                                                            /* 42 */
 TEXT_DEF(PROT_ERR_spare_43,                       "")                                                            /* 43 */
 TEXT_DEF(PROT_ERR_spare_44,                       "")                                                            /* 44 */
 TEXT_DEF(PROT_ERR_spare_45,                       "")                                                            /* 45 */
 TEXT_DEF(PROT_ERR_spare_46,                       "")                                                            /* 46 */
 TEXT_DEF(PROT_ERR_spare_47,                       "")                                                            /* 47 */
 TEXT_DEF(PROT_ERR_spare_48,                       "")                                                            /* 48 */
 TEXT_DEF(PROT_ERR_spare_49,                       "")                                                            /* 49 */
 TEXT_DEF(PROT_ERR_spare_50,                       "")                                                            /* 50 */

#elif defined (TEXT_CLASS_IFWD_INFO)
 TEXT_DEF(PROT_INFO_init,                    "Protocol handler Initialized")                                      /* 01 */
 TEXT_DEF(PROT_INFO_target_error_file_line,  "Target file name: %s.\nFunction: %s.\nLine:   %d")                  /* 02 */
 TEXT_DEF(PROT_INFO_target_DQ_error_data_1,  "First last DQ status: 0x%04X")                                      /* 03 */
 TEXT_DEF(PROT_INFO_target_DQ_error_data_2,  "Second last DQ status: 0x%04X")                                     /* 04 */
 TEXT_DEF(PROT_INFO_target_DQ_error_data_3,  "Third  last DQ status: 0x%04X")                                     /* 05 */
 TEXT_DEF(PROT_INFO_target_last_data,        "Last data: 0x%04X")                                                 /* 06 */
 TEXT_DEF(PROT_INFO_target_last_addr_1,      "First Last address: 0x%08X\n")                                      /* 07 */
 TEXT_DEF(PROT_INFO_target_last_addr_2,      "Second Last address: 0x%08X")                                       /* 08 */
 TEXT_DEF(PROT_INFO_target_last_addr_3,      "Third Last address: 0x%08X")                                        /* 09 */
 TEXT_DEF(PROT_INFO_target_error,            "\nError in EBL target")                                             /* 10 */
 TEXT_DEF(PROT_INFO_flash_root_addr,         "\nFlash root address: 0x%08X")                                      /* 11 */
 TEXT_DEF(PROT_INFO_flash_start_addr,        "Flash start address: 0x%08X")                                       /* 12 */
 TEXT_DEF(PROT_INFO_flash_address,           "Flash address: 0x%08X")                                             /* 13 */
 TEXT_DEF(PROT_INFO_flash_corr_addr,         "Flash correction address: 0x%08X")                                  /* 14 */
 TEXT_DEF(PROT_INFO_erase_next_addr,         "Flash erase next address: 0x%08X")                                  /* 15 */
 TEXT_DEF(PROT_INFO_erase_end_addr,          "Flash erase next address: 0x%08X")                                  /* 16 */
 TEXT_DEF(PROT_INFO_erase_sec_addr,          "Flash erase sec address: 0x%08X")                                   /* 17 */
 TEXT_DEF(PROT_INFO_state,                   "state: %s\n")                                                 /* 18 */
 TEXT_DEF(PROT_INFO_erase_addr,              "Flash erase address: 0x%08X")                                       /* 19 */
 TEXT_DEF(PROT_INFO_spare_20,                "")                                                                  /* 20 */
 TEXT_DEF(PROT_INFO_spare_21,                "")                                                                  /* 21 */
 TEXT_DEF(PROT_INFO_spare_22,                "")                                                                  /* 22 */
 TEXT_DEF(PROT_INFO_spare_23,                "")                                                                  /* 23 */
 TEXT_DEF(PROT_INFO_spare_24,                "")                                                                  /* 24 */
 TEXT_DEF(PROT_INFO_spare_25,                "")                                                                  /* 25 */
 TEXT_DEF(PROT_INFO_spare_26,                "")                                                                  /* 26 */
 TEXT_DEF(PROT_INFO_spare_27,                "")                                                                  /* 27 */
 TEXT_DEF(PROT_INFO_spare_28,                "")                                                                  /* 28 */
 TEXT_DEF(PROT_INFO_spare_29,                "")                                                                  /* 29 */
 TEXT_DEF(PROT_INFO_spare_30,                "")                                                                  /* 30 */
 TEXT_DEF(PROT_INFO_spare_31,                "")                                                                  /* 31 */
 TEXT_DEF(PROT_INFO_spare_32,                "")                                                                  /* 32 */
 TEXT_DEF(PROT_INFO_spare_33,                "")                                                                  /* 33 */
 TEXT_DEF(PROT_INFO_spare_34,                "")                                                                  /* 34 */
 TEXT_DEF(PROT_INFO_spare_35,                "")                                                                  /* 35 */
 TEXT_DEF(PROT_INFO_spare_36,                "")                                                                  /* 36 */
 TEXT_DEF(PROT_INFO_spare_37,                "")                                                                  /* 37 */
 TEXT_DEF(PROT_INFO_spare_38,                "")                                                                  /* 38 */
 TEXT_DEF(PROT_INFO_spare_39,                "")                                                                  /* 39 */
 TEXT_DEF(PROT_INFO_spare_40,                "")                                                                  /* 40 */
 TEXT_DEF(PROT_INFO_spare_41,                "")                                                                  /* 41 */
 TEXT_DEF(PROT_INFO_spare_42,                "")                                                                  /* 42 */
 TEXT_DEF(PROT_INFO_spare_43,                "")                                                                  /* 43 */
 TEXT_DEF(PROT_INFO_spare_44,                "")                                                                  /* 44 */
 TEXT_DEF(PROT_INFO_spare_45,                "")                                                                  /* 45 */
 TEXT_DEF(PROT_INFO_spare_46,                "")                                                                  /* 46 */
 TEXT_DEF(PROT_INFO_spare_47,                "")                                                                  /* 47 */
 TEXT_DEF(PROT_INFO_spare_48,                "")                                                                  /* 48 */
 TEXT_DEF(PROT_INFO_spare_49,                "")                                                                  /* 49 */
 TEXT_DEF(PROT_INFO_spare_50,                "")                                                                  /* 50 */

#else

#ifndef _INC_PROT
#define _INC_PROT

#include "IFWD_transport_handler.h"

/*****************************************************************************
 *                     CONSTANT DEFINITIONS                                  *
 *****************************************************************************/
#define MAX_PACK_DATA_SIZE  0x80000      /* Max buffer size is 512K            */
#define DEFAULT_PACK_SIZE   0x0800       /* Set default pack size to 2048 byte */

#define PROT_1_16  0x00000110UL
#define PROT_1_32  0x00000120UL
#define PROT_2_32  0x00000220UL
#define PROT_3_32  0x00000320UL

/*****************************************************************************
 *                    ENUMERATION DEFINITIONS                                *
 *****************************************************************************/

/* This enumeration gives the possible status of the protocol handler */

typedef enum prot_status_s
{
   prot_status_ok = 0,                 /* requested operation is successful */
   prot_status_ready,                  /* handler is currently in ready state */

   prot_status_invalid_channel_id,         /* channel id provided is invalid */
   prot_status_channel_instance_not_exist,  /* instance is not avaialble  */

   prot_status_target_not_booted,      /* Boot process has not been started*/

   prot_status_rx_pack_ongoing,        /* Protocol is processing a new
                                                    received pack from target */
   prot_status_rx_pack_ready,          /* protocol received a new and valid
                                                            pack from target */
   prot_status_rx_pack_timeout,        /* The protocol has timeout waiting
                                               for a receive pack from target */
   prot_status_tx_pack_ongoing,        /* transmitting a pack to target */
   prot_status_tx_pack_done,           /* transmitted a pack succesfully */
   prot_status_tx_pack_timeout,        /* timeout while transmitting a pack */

   prot_status_channel_already_added,  /* channel is already been added */
   prot_status_channel_already_deleted,/* the instance for the channel does not
                                                   exists or already removed  */
   prot_status_no_mem,                 /* No memory for creating instance */
   prot_status_error,                  /* requested operation failed */
   prot_status_unknown_error =0xFFFF   /* An unexpected error has occurred */
} prot_status_t;


/* This enumeration gives the possible errors reached in the protocol module */

typedef enum prot_error_s
{
   prot_error_target_boot_error,

   prot_error_invalid_channel_id,
   prot_error_channel_instance_not_exist,

   prot_error_buffers_no_config,

   prot_error_rx_pack_error,
   prot_error_rx_pack_timeout,

   prot_error_rx_ongoing,

   prot_error_tx_pack_error,
   prot_error_tx_pack_timeout,

   prot_error_tx_ongoing,

   prot_error_no_more_mem_for_pack,

   prot_error_handler_in_error_state,
   prot_error_no_error,
   prot_error_unknown = 0xFFFF
} prot_error_t;


/* This Enumeration gives the Possible types of Package exchanged between
                                                               PC and target */


typedef unsigned short int package_type_t;
enum package_type_s
{
  Errors = 0,
  RspChecksumError,
  RspRxTimeoutError,

  ErrorHandling = 0x40,
  IndErrorMsg,
  IndWarningMsg,
  IndInfoMsg,
  IndDebugPackage,


  MainStuff = 0x0080,
  ReqRetransmission,
  CnfBaudChange,
  ErrWrongBaudrate,
  ReqCfiInfo_1,
  ReqCfiInfo_2,
  ReqSetProtConf,
  ReqProtocolChange,
  ErrWrongProtocol,

  TestAndDebug = 0x0100,
  RspRxLoopback,
  ReqSetLed,

  LowLevelService = 0x0200,
  ReqSetRamWorkAddress,
  ReqWriteToRam,
  ReqExecuteFromRam,
  ReqSecStart,
  ReqSecEnd,
  ReqCloseHandle,
  ReqBootloaderVer,
  ReqForceHwReset,
  ReqSecChNonceAndVersion,  // request is empty, reply contains tlv packaged sec-channel request
  ReqSecChValidateVerdict, // request contains tlv packaged sec-channel reply, reply contains  verdict and validation result { uint16_t verdict || uint16_t valid }


  FlashFunctions = 0x0800,
  ReqFlashId,
  ReqFlashSetAddress,
  ReqFlashReadBlock,
  ReqFlashWriteBlock,
  ReqFlashEraseStart,
  ReqFlashEraseCheck,
  ReqFlashReadChecksum,
  ReqCompressedPackage,
  ReqSetNandCtrlBitField,
  ReqSetNvmOptions,
  ReqFlashSetReadAddress,
  ReqOutOfSessionControl,
  ReqOutOfSessionDataWrite,
  ReqOutOfSessionDataRead,
  ReqFlashWriteImageRaw,

  packagedummy = 0xFFFF
};


/*****************************************************************************
 *                    STRUCTURE  TYPE DEFINITIONS                            *
 *****************************************************************************/

typedef struct prot_header_s
{
  uint16_t type;
  uint16_t checksum;
  uint32_t len;
} prot_header_t;

typedef struct prot_payload_s
{
  uint8_t reserved[128];
  uint8_t data[MAX_PACK_DATA_SIZE + TRA_COMPRESSION_HEADER_BYTES];  /* PACK_DATA_SIZE is 2 Kbytes + 4 byte compression header inserted by TRA */
} prot_payload_t;

/*****************************************************************************
 *                       PROTOTYPES DEFINITIONS                              *
 *****************************************************************************/

 typedef void (*prot_fx_tx_t) (uint8_t channel,uint8_t success);
 typedef void (*prot_fx_rx_t) (uint8_t channel,uint8_t success);

 /************************** Public Functions ********************************/

/*******************************************************************************
 * Function: void PROT_init(void);
 *------------------------------------------------------------------------------
 * Purpose:   This function is responsible for the initialization of the
 *            structures used to manage the creation and disposal of protcol
 *            and to trigger the state machine instances.
 *
 * Parameters:
 * IN         None
 *
 * IN/OUT     None
 *
 * Returns:   Nothing
 ******************************************************************************/
 extern void PROT_init(void);
/*******************************************************************************
 * Function:  void PROT_close(void);
 *------------------------------------------------------------------------------
 * Purpose:   This function is responsible for cleaning up the protocol
 *            management state machine and instances, if any.
 *
 * Parameters:
 * IN         None
 *
 * IN/OUT     None
 *
 * Returns:   Nothing
 ******************************************************************************/
 extern void PROT_close(void);
/*******************************************************************************
 * Function:  void PROT_clock(void);
 *------------------------------------------------------------------------------
 * Purpose:   This function is responsible to trigger the internal state machine
 *            for each instance currently running.
 *
 * Parameters:
 * IN         None
 *
 * IN/OUT     None
 *
 * Returns:   Nothing
 ******************************************************************************/
 extern void PROT_clock(void);
/*******************************************************************************
 * Function: prot_status_t PROT_add_channel(uint8_t channel);
 *------------------------------------------------------------------------------
 * Purpose:  This function is responsible for the creation of an instance and
 *           the protocol state machine.
 *
 * Parameters:
 * IN        channel - instance number
 *
 * IN/OUT    None
 *
 * Returns:  prot_status_t
 *              The enum value indicating the current status of the handler
 ******************************************************************************/
 extern prot_status_t PROT_add_channel(uint8_t channel);
 /******************************************************************************
 * Function:  prot_status_t PROT_delete_channel(uint8_t channel);
 *------------------------------------------------------------------------------
 * Purpose:   This function is responsible for disposing a running instance
 *            corresponding to the channel id provided.
 * Parameters:
 * IN         channel  - Channel number identifying the instance
 *
 * IN/OUT     None
 *
 * Returns:   prot_status_t
 *               The enum value indicating the current status of the handler
 *
 ******************************************************************************/
 extern prot_status_t PROT_delete_channel(uint8_t channel);
 /*******************************************************************************
* Function: prot_status_t PROT_set_protocol(uint8_t channel, uint32_t prot_type);
*------------------------------------------------------------------------------
* Purpose:  Change the protocol 1.16, 2.32 ....
*
* Parameters:
* IN        channel   - Instance number
*           prot_type - PROT_1_16, PROT_2_32
*
* IN/OUT    None
*
* Returns:  prot_status_t
*              The enum value indicating the current status of the handler
******************************************************************************/
extern prot_status_t PROT_set_protocol(uint8_t channel, uint32_t prot_type);

/*******************************************************************************
 * Function: prot_status_t PROT_tx_pack(uint8_t channel,
 *                                      prot_header_t *p_hdr,
 *                                      prot_payload_t *p_data,
 *                                      prot_fx_tx_t fx_tx);
 *------------------------------------------------------------------------------
 * Purpose:  Transmit a protocol pack if the target is booted and instance is
 *           in ready state
 *
 * Parameters:
 * IN        channel   - Instance number
 *           p_hdr     - Pointer to protocol header
 *           p_data    - Pointer to protocol payload
 *           fx_tx     - Callback function called when transmition is complete
 *
 * IN/OUT    None
 *
 * Returns:  prot_status_t
 *              The enum value indicating the current status of the handler
 ******************************************************************************/
 extern prot_status_t PROT_tx_pack( uint8_t channel,
                                    prot_header_t *p_hdr,
                                    prot_payload_t *p_data,
                                    prot_fx_tx_t fx_tx);

/*******************************************************************************
 * Function: prot_status_t PROT_rx_pack(uint8_t channel,
 *                                      prot_header_t *p_hdr,
 *                                      prot_payload_t *p_data,
 *                                      prot_fx_rx_t fx_rx);
 *------------------------------------------------------------------------------
 * Purpose: upper layers can invoke this function to get the data buffer
 *          received through the communication channel
 *
 * Parameters:
 * IN       channel- Instance number
 *          fx_rx  - Callback function called when the data reception is
 *                   complete
 *
 * IN/OUT   p_hdr     - Pointer to protocol header
 *          p_data    - Pointer to protocol payload
 *                       written
 *
 * Returns: prot_status_t
 *              The enum value indicating the current status of the handler
 *
 ******************************************************************************/
 extern prot_status_t PROT_rx_pack( uint8_t channel,
                                    prot_header_t *p_hdr,
                                    prot_payload_t *p_data,
                                    prot_fx_rx_t fx_rx);

/*******************************************************************************
 * Function: prot_status_t PROT_get_status(uint8_t channel, char *status)
 *------------------------------------------------------------------------------
 * Purpose:  This function returns the last status registred in the protocol
 *           instance of the given channel.
 *
 * Parameters:
 * IN        channel - Instance number.
 *
 * IN/OUT    status - status information  in the form of a string. The calling
 *                    function is responsible for the memory allocation
 *
 * Returns:  prot_status_t
 *              The enum value indicating the current status of the handler
 *
 ******************************************************************************/
 extern prot_status_t PROT_get_status(uint8_t channel, char *status);

/*******************************************************************************
 * Function: prot_status_t PROT_set_status(uint8_t channel)
 *------------------------------------------------------------------------------
 * Purpose: sets the last status registred in the instance to prot_status_ok
 *
 * Parameters:
 * IN       channel - Instance number.
 *
 * IN/OUT   None
 *
 * Returns: prot_status_t
 *              The enum value indicating the current status of the handler
 ******************************************************************************/
 extern prot_status_t PROT_set_status(uint8_t channel);

/*******************************************************************************
 * Function: prot_status_t PROT_abort_process(uint8_t channel);
 *------------------------------------------------------------------------------
 * Purpose:  This function will terminate all oustanding transaction and close
 *           the communication handler identified in the parameter channel. The
 *           resources allocated by the instances are disposed
 *
 * Parameters:
 * IN        channel - Instance number
 *
 * IN/OUT    None
 *
 * Returns:  prot_status_t
 *              The enum value indicating the current status of the handler
 ******************************************************************************/
 extern prot_status_t PROT_abort_process(uint8_t channel);

/*******************************************************************************
 * Function: prot_error_t PROT_get_last_error(uint8_t channel, char *error);
 *------------------------------------------------------------------------------
 * Purpose:  This function returns the last error registred in the protocol
 *           instance of the given channel.
 *
 * Parameters:
 * IN        channel - Instance number.
 *
 * IN/OUT
 *           error - error value in the string format. The calling function is
 *                   responsible for the memory allocation
 *
 * Returns:  prot_error_t
 *              The enum value indicating the current error in the handler
 *
 ******************************************************************************/
 extern prot_error_t PROT_get_last_error(uint8_t channel, char *error);

/*******************************************************************************
 * Function:  prot_status_t PROT_set_last_error(uint8_t channel);
 *------------------------------------------------------------------------------
 * Purpose:   This function initialize the error registred in the instance
 *            to prot_error_no_error;
 *
 * Parameters:
 * IN         channel:   Instance number.
 *
 * IN/OUT     None
 *
 * Returns:  prot_status_t
 *              The enum value indicating the current status of the handler
 *
 ******************************************************************************/
 extern prot_status_t PROT_set_last_error(uint8_t channel);


#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
  void PROT_Err(uint8_t channel, uint32_t err);
#endif

#if defined (USE_TEXT_CLASS_IFWD_INFO)
  void PROT_info(uint8_t channel, uint32_t err);
#endif
#endif  /* From #ifndef _PROT_H_ */

#endif



