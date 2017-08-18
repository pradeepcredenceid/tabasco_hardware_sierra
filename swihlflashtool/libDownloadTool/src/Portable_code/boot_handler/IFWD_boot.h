/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/boot_handler/IFWD_boot.h $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:51:33 UTC $
 *   Comment:
 *     SMS01221639
 */

#if defined (TEXT_CLASS_IFWD_ERRORS)
TEXT_DEF(BOOT_ERR_channel_is_outside_range,       "Channel number %d is outside the valid range.")                                  /* 01 */
TEXT_DEF(BOOT_ERR_TX_ack_boot_code,               "Error transmitting boot code")                                                   /* 02 */
TEXT_DEF(BOOT_ERR_wrong_ack_boot_code,            "Boot acknowledge code is invalid.")                                              /* 03 */
TEXT_DEF(BOOT_ERR_RX_boot_ack_timeout,            "Timeout waiting for boot acknowledge code.")                                     /* 04 */
TEXT_DEF(BOOT_ERR_ebl_loader_load_timeout,        "Error loading EBL loader.")                                                      /* 05 */
TEXT_DEF(BOOT_ERR_ebl_loader_checksum_error,      "Ebl loader Checksum error.")                                                     /* 06 */
TEXT_DEF(BOOT_ERR_ebl_load_timeout,               "Error loading EBL.")                                                             /* 07 */
TEXT_DEF(BOOT_ERR_ebl_checksum_error,             "Ebl Checksum error.")                                                            /* 08 */
TEXT_DEF(BOOT_ERR_ebl_version,                    "Timeout waiting for EBL version.")                                               /* 09 */
TEXT_DEF(BOOT_ERR_error_comm_buffer_flush_error,  "Error flushing comport internal buffers.")                                       /* 10 */
TEXT_DEF(BOOT_ERR_error_comm_error_unknown,       "Unknown error in the communication.")                                            /* 11 */
TEXT_DEF(BOOT_ERR_unknown,                        "Unkown error occurred in the boot sequence")                                     /* 12 */
TEXT_DEF(BOOT_ERR_malloc,                         "Could not allocate memory.")                                                     /* 13 */
TEXT_DEF(BOOT_ERR_free,                           "Could not free memory.")                                                         /* 14 */
TEXT_DEF(BOOT_ERR_channel_already_opened,         "Channel %d cannot be opened twice.")                                             /* 15 */
TEXT_DEF(BOOT_ERR_channel_does_not_exist,         "BOOT: Channel does not exist %d.")                                               /* 16 */
TEXT_DEF(BOOT_ERR_channel_close_busy,             "Channel is in use, so it cannot be closed.")                                     /* 17 */
TEXT_DEF(BOOT_ERR_comm_not_open,                  "Communication channel not opened. Can not Boot.")                                /* 18 */
TEXT_DEF(BOOT_ERR_spare_19,                       "")                                                                               /* 19 */
TEXT_DEF(BOOT_ERR_boot_timeout,                   "Timeout booting device.")                                                        /* 20 */
TEXT_DEF(BOOT_ERR_boot_device_error,              "Unexpected error booting device.")                                               /* 21 */
TEXT_DEF(BOOT_ERR_boot_not_started,               "The boot process has not been started.")                                         /* 22 */
TEXT_DEF(BOOT_ERR_platform_unknown,               "Platform not recognized.")                                                       /* 23 */
TEXT_DEF(BOOT_ERR_rx_timeout,                     "Rx timeout while booting")                                                       /* 24 */
TEXT_DEF(BOOT_ERR_gpuk_invalid,                   "A valid GPuK Certificate could not be identified.")                              /* 25 */
TEXT_DEF(BOOT_ERR_gpuk_cust_id,                   "The CUSTOMER_ID bits in the GPuK Certificate do not match the target HW.")       /* 26 */
TEXT_DEF(BOOT_ERR_missing_psi,                    "PSI RAM needs to be injected when using USB")                                    /* 27 */
TEXT_DEF(BOOT_ERR_missing_ebl,                    "EBL needs to be injected when using USB")                                        /* 28 */
TEXT_DEF(BOOT_ERR_target_wdt_timeout,             "Target boot watchdog timeout")                                                   /* 29 */
TEXT_DEF(BOOT_ERR_missing_psi_p,                  "PSI RAM must be injected for this platform")                                     /* 30 */
TEXT_DEF(BOOT_ERR_missing_ebl_p,                  "EBL must be injected for this platform")                                         /* 31 */
TEXT_DEF(BOOT_ERR_rpsi_rsp_crc,                   "RPSI response crc mismatch")                                                     /* 32 */
TEXT_DEF(BOOT_ERR_rpsi_cmd_crc,                   "RPSI command crc mismatch")                                                      /* 32 */
TEXT_DEF(BOOT_ERR_rpsi_cmd_unknown,               "RPSI command unknown")                                                           /* 33 */
TEXT_DEF(BOOT_ERR_rpsi_rsp_unknown,               "RPSI response unknown")                                                          /* 34 */
TEXT_DEF(BOOT_ERR_rpsi_baud_could_not_be_set,     "RPSI baud rate could not be set")                                                /* 35 */
TEXT_DEF(BOOT_ERR_rpsi_rx_fail,                   "RPSI receive response failed")                                                   /* 36 */
TEXT_DEF(BOOT_ERR_CID_mismatch,                   "The CID from the RPSI and the HW does not match")                                /* 37 */
TEXT_DEF(BOOT_ERR_mem_test_fail,                  "RPSI Memory test failed \n\t Test: %s \n\t Addr:       0x%08X \n\t Expected:   0x%08X \n\t Read:       0x%08X") /* 38 */
TEXT_DEF(BOOT_ERR_secure_channel_cb_fail,         "Secure channel call-back failed")                                                /* 39 */
TEXT_DEF(BOOT_ERR_boot_skip_psi,                  "Boot rom failed the process of skipping the PSI.")                               /* 40 */
TEXT_DEF(BOOT_ERR_rpsi_wrong_ack,                 "RPSI did not signal running state correctly.")                                                                               /* 41 */
TEXT_DEF(BOOT_ERR_spare_42,                       "")                                                                               /* 42 */
TEXT_DEF(BOOT_ERR_spare_43,                       "")                                                                               /* 43 */
TEXT_DEF(BOOT_ERR_spare_44,                       "")                                                                               /* 44 */
TEXT_DEF(BOOT_ERR_spare_45,                       "")                                                                               /* 45 */
TEXT_DEF(BOOT_ERR_spare_46,                       "")                                                                               /* 46 */
TEXT_DEF(BOOT_ERR_spare_47,                       "")                                                                               /* 47 */
TEXT_DEF(BOOT_ERR_spare_48,                       "")                                                                               /* 48 */
TEXT_DEF(BOOT_ERR_spare_49,                       "")                                                                               /* 49 */
TEXT_DEF(BOOT_ERR_spare_50,                       "")                                                                               /* 50 */

#elif defined (TEXT_CLASS_IFWD_INFO)
TEXT_DEF(BOOT_INFO_target_sync,                   "Device synchronized.")                /* 01 */
TEXT_DEF(BOOT_INFO_inject_ebl_ldr,                "Injecting RPSI")                      /* 02 */
TEXT_DEF(BOOT_INFO_inject_ebl,                    "Injecting EBL")                       /* 03 */
TEXT_DEF(BOOT_INFO_sync,                          "Sync. process started.")              /* 04 */
TEXT_DEF(BOOT_INFO_PSI_version,                   "  - RPSI version %s %d.%d")           /* 05 */
TEXT_DEF(BOOT_INFO_Bootmode,                      "    - bootmode %08X")                 /* 06 */
TEXT_DEF(BOOT_INFO_SNUM,                          "    - SNUM[%d] %04X")                 /* 07 */
TEXT_DEF(BOOT_INFO_HASH,                          "    - HASH[%d] %08X")                 /* 08 */
TEXT_DEF(BOOT_INFO_FUSING,                        "    - Fusing verified as: %s")        /* 09 */
TEXT_DEF(BOOT_INFO_TARGET,                        "  - Target is: %s")                   /* 10 */
TEXT_DEF(BOOT_INFO_SEC_X,                         "    - Sec X: %d")                     /* 11 */
TEXT_DEF(BOOT_INFO_SEC_Y,                         "    - Sec Y: %d")                     /* 12 */
TEXT_DEF(BOOT_INFO_SEC_LEVEL,                     "    - Sec Level: %d")                 /* 13 */
TEXT_DEF(BOOT_INFO_CUST_ID,                       "    - Cust ID: %d")                   /* 14 */
TEXT_DEF(BOOT_INFO_ROW_BIT,                       "    - Row bit: %d")                   /* 15 */
TEXT_DEF(BOOT_INFO_RPSI_CMD_MODE,                 "  - RPSI command mode.")              /* 16 */
TEXT_DEF(BOOT_INFO_RPSI_BAUD,                     "    - Set baudrate to %d")            /* 17 */
TEXT_DEF(BOOT_INFO_RPSI_LD_EBL,                   "    - Load/Execute EBL")              /* 18 */
TEXT_DEF(BOOT_INFO_HW_CID,                        "  -   HW CID=%s")                     /* 19 */
TEXT_DEF(BOOT_INFO_RPSI_CID,                      "  - RPSI CID=%s")                     /* 20 */
TEXT_DEF(BOOT_INFO_USB_OUT_TEST_msg,              "  - USB OUT avg bandwidth = %d Kbps")                                    /* 21 */
TEXT_DEF(BOOT_INFO_USB_OUT_TEST_bytes_msg,        "  - USB OUT bytes transferred in Kbytes = %d")                                    /* 21 */
TEXT_DEF(BOOT_INFO_USB_IN_TEST_msg,              "  - USB IN avg bandwidth = %d Kbps")                                    /* 21 */
TEXT_DEF(BOOT_INFO_USB_IN_TEST_bytes_msg,         "  - USB IN bytes transferred in Kbytes = %d")                                    /* 21 */
TEXT_DEF(BOOT_INFO_USB_TIME_msg,                  "  - time elapsed in sec = %d")                                    /* 22 */
TEXT_DEF(BOOT_INFO_USB_CERTIFICATION_TEST,        "  \n\n - USB certification test.\n\n Close FlashTool_E2 application and launch USB verfication test application")    /* 24 */
TEXT_DEF(BOOT_INFO_USB_OUT_TEST_START,            "  \n - USB Out test started")                                    /* 25 */
TEXT_DEF(BOOT_INFO_USB_IN_TEST_START,             "  \n - USB IN test started")                                    /* 25 */
TEXT_DEF(BOOT_INFO_spare_29,                      "")                                    /* 29 */
TEXT_DEF(BOOT_INFO_spare_30,                      "")                                    /* 30 */
TEXT_DEF(BOOT_INFO_spare_31,                      "")                                    /* 31 */
TEXT_DEF(BOOT_INFO_spare_32,                      "")                                    /* 32 */
TEXT_DEF(BOOT_INFO_spare_33,                      "")                                    /* 33 */
TEXT_DEF(BOOT_INFO_spare_34,                      "")                                    /* 34 */
TEXT_DEF(BOOT_INFO_spare_35,                      "")                                    /* 35 */
TEXT_DEF(BOOT_INFO_spare_36,                      "")                                    /* 36 */
TEXT_DEF(BOOT_INFO_spare_37,                      "")                                    /* 37 */
TEXT_DEF(BOOT_INFO_spare_38,                      "")                                    /* 38 */
TEXT_DEF(BOOT_INFO_spare_39,                      "")                                    /* 39 */
TEXT_DEF(BOOT_INFO_spare_40,                      "")                                    /* 40 */
TEXT_DEF(BOOT_INFO_spare_41,                      "")                                    /* 41 */
TEXT_DEF(BOOT_INFO_spare_42,                      "")                                    /* 42 */
TEXT_DEF(BOOT_INFO_spare_43,                      "")                                    /* 43 */
TEXT_DEF(BOOT_INFO_spare_44,                      "")                                    /* 44 */
TEXT_DEF(BOOT_INFO_spare_45,                      "")                                    /* 45 */
TEXT_DEF(BOOT_INFO_spare_46,                      "")                                    /* 46 */
TEXT_DEF(BOOT_INFO_spare_47,                      "")                                    /* 47 */
TEXT_DEF(BOOT_INFO_spare_48,                      "")                                    /* 48 */
TEXT_DEF(BOOT_INFO_spare_49,                      "")                                    /* 49 */
TEXT_DEF(BOOT_INFO_spare_50,                      "")                                    /* 50 */

#else

#ifndef _BOOT_H_
#define _BOOT_H_

#include "devices.h"
#include "nand_spare_area_internal.h"
#include "psi_std_errors.h"
#include "memtest.h"                //< memtest_error_t struct


/*****************************************************************************
 *                         CONSTANT DEFINITIONS                              *
 *****************************************************************************/
#define BOOT_VERSION_NAME_SIZE  32


#define BOOT_NO_SEC_MODE   0xBB
#define BOOT_SEC_MODE      0xCC
#define BOOT_USB_MODE      0xDD

/* Used in callback to SFH */
#define EBL_NOT_RUNNING                 0
#define EBL_RUNNING                     1
#define BOOT_SYNC                       2
#define BOOT_START                      3
#define PRE_BOOT_OK                     4
#define SYS_SW_RUNNING                  5

/*****************************************************************************
 *                       ENUMERATION DEFINITIONS                             *
 *****************************************************************************/

typedef enum boot_status_s
{
  boot_status_ok,
  boot_status_comm_channel_not_running,
  boot_status_channel_added,
  boot_status_channel_not_added,
  boot_status_not_resources_to_add_channel,
  boot_status_channel_deleted,
  boot_status_comm_error,
  boot_status_platform_not_supported,
  boot_status_error,
  boot_status_booting_device,
  boot_status_boot_timeout,
  boot_status_booting_device_error,
  boot_status_ebl_running,
  boot_status_pre_boot_ok,
  boot_status_comm_error_unknown,
  boot_status_not_valid_status = 0xFFFF
} boot_status_t;

typedef enum boot_error_s
{
  boot_error_no_error,
  boot_error_channel_not_added,
  boot_error_boot_not_started,
  boot_error_TX_boot_code,
  boot_error_wrong_ack_boot_code,
  boot_error_RX_boot_ack_timeout,
  boot_error_ebl_loader_load_timeout,
  boot_error_ebl_loader_checksum_error,
  boot_error_ebl_load_timeout,
  boot_error_ebl_checksum_error,
  boot_error_rx_ebl_version,
  boot_error_comm_buffer_flush_error,
  boot_error_rx_timeout,
  boot_error_gpuk_fail,

  boot_error_comm_error_unknown,
  boot_error_unknown = 0xFFFF
} boot_error_t;

/*****************************************************************************
 *                        PROTOTYPES DEFINITIONS                             *
 *****************************************************************************/
/******** Defines to be used with the capability struct below.*************/

#define CAP_NOT_ENHANCED 0

/*  protocol_version */
#define CAP_PROT_VER_ENH            1
#define CAP_PROT_VER_ENH_SYNC_32BIT 2
#define CAP_PROT_VER_ENH_32BIT      3

/* package_size */
#define CAP_PACK_SIZE_DEFAULT_2048  0
#define CAP_PACK_SIZE_4096            4
#define CAP_PACK_SIZE_8192            8
#define CAP_PACK_SIZE_16384          16
#define CAP_PACK_SIZE_32768          32
#define CAP_PACK_SIZE_65536          64
#define CAP_PACK_SIZE_131072        128
#define CAP_PACK_SIZE_262144        256
#define CAP_PACK_SIZE_524288        512


/* crc_method */
#define CAP_CRC_RETURNED_BY_SEC_END 1

/* crc_type */
#define CAP_CRC_NOT_USED            1

/* wr pack skip cnf */
#define CAP_SKIP_WR_PACK_CNF 1
/* compression */
#define CAP_SKIP_CONST_FF_PACKS 1

/* Debug capabilities  */
#define ERASE_SECTOR_VERIFY     1

/* Flash DEBUG handling */
#define FLASH_DEBUG_ON      1

/* Protocol CRC method */
#define CAP_PROT_NO_CRC  1

#define CAP_FLAG_USE_EXT_CAP 0x01

#define EXT_CAP_OOS_CFG1_GET_HW_ID    0x01
#define EXT_CAP_OOS_CFG1_GET_1ST_16   0x02
#define EXT_CAP_OOS_CFG1_GET_FLASH_ID 0x04
#define EXT_CAP_OOS_CFG1_ERASE_ALL    0x08
#define EXT_CAP_OOS_CFG1_WRITE_XML    0x10

#define EXT_CAP_USB_RAW_WR_MODE       0x01
#define EXT_CAP_MIPI_RAW_1_16         0x01
#define EXT_CAP_MIPI_RAW_3_32         0x04

#define EXT_CAP_COMM_FORMAT_USIF      0x01
#define EXT_CAP_COMM_FORMAT_USB       0x02
#define EXT_CAP_COMM_FORMAT_MIPI      0x04
#define EXT_CAP_COMM_FORMAT_SPI       0x08
#define EXT_CAP_COMM_FORMAT_HSIC      0x10

#define EXT_CAP_VERIFY_OPT_NO_PRG     0x01
#define EXT_CAP_SEC_OPEN_READ         0x01

/*****************************************************************************
 *                        STRUCTURES DEFINITIONS                             *
 *****************************************************************************/
typedef struct capabilities_s
{
  uint8_t protocol_version;     /* 0 = standard; 0x01 enhanced (this struct is used)                            */
  uint8_t package_size;         /* 0 = 2048 bytes, else N*1024 bytes                                            */
  uint8_t crc_method;           /* 0 = Returned by WriteData, 1 = used returned by SecEndPack, 2 = CRC not used */
  uint8_t crc_type;             /* 0 = 16 bit XOR, 1 = CRC not used.                                            */
  uint8_t skip_wr_pack_cnf;     /* 0 = Expect confirm on ReqFlashWriteBlock, 1= skip the confirm pack.          */
  uint8_t compression;          /* 0 = no compression, 1 = skip whole 0xFF packages                             */
  uint8_t flags;                /* Bit0 = 1 Use extended capabilities (Windows)                                 */
  uint8_t erase_sector_verify;  /* 0 = no verification. 0x01 flash sector verified after sector erase.          */
  uint8_t flash_debug;          /* 1 = Debug information enabled                                                */
  uint8_t protocol_crc;         /* 0 = 16 bit ADD, 1= No CRC                                                    */
  uint8_t skip_erase;           /* 0 = normal erase, 1 = conditional skip erase                                 */
  uint8_t skip_wr_pack_crc;     /* 0 = calculate CRC on all packages, 1 = skip CRC for write packages.          */
}capabilities_t;

typedef struct
{
  uint8_t oos_config;            // bit0 get_hw_id, bit1 get_first_16, bit2 get_flash_id, bit3 erase_all, bit4 write_xml
  uint8_t usb_handling_flags;    // bit0 support_raw_wr_mode.
  uint8_t comm_formats_1;        // bit0=USIF, 1=USB, 2=MIPI, 3=SPI, 4=HSIC
  uint8_t verify_options;        // bit0="Verify without programming."
  uint8_t mipi_handling_flags;   // bit0 support_raw_wr_mode.
  uint8_t sec_handling_flags;    // bit0 support open read in secure mode
  uint8_t spare_07;
  uint8_t spare_08;
  uint8_t spare_09;
  uint8_t spare_10;
  uint8_t spare_11;
  uint8_t spare_12;
} ext_capabilities_t;

typedef struct boot_ebl_version_s
{
  uint32_t  boot_mode;
  uint32_t  major;
  uint32_t  minor;
  uint8_t   name[BOOT_VERSION_NAME_SIZE];
  capabilities_t capabilities;
  uint8_t  boot_cfg_value;
  uint8_t  ext_capabilities_are_handled;   // Set to signal that the currently unused bits in this structure is cleared
                                      // This has so far not been the case - causing backward compatability issues
                                      // when taking new bits into use.
  uint16_t expected_memory_class;
  uint32_t reserved_long;                  //< Windows
  ext_capabilities_t ext_capabilities;//< Windows
} boot_ebl_version_t;

typedef void (*boot_fx_t)(uint8_t id, uint8_t ebl_run, boot_ebl_version_t *p_version);

typedef struct boot_dev_cfg_s
{
   uint32_t              platform;
   uint32_t              technology;
   char                 *boot_filename;   //< Windows
   uint8_t              *psi_offset;
   uint32_t              psi_size;
   uint8_t              *ebl_offset;
   uint32_t              ebl_size;
 } boot_dev_cfg_t;

/*****************************************************************************
 *                               PUBLIC FUNCTIONS                            *
 *****************************************************************************/

 extern void BOOT_init(void);

 extern void BOOT_close(void);

 extern void BOOT_clock(void);

 extern boot_status_t BOOT_add_channel(uint8_t channel);

 extern boot_status_t BOOT_delete_channel(uint8_t channel);

 extern boot_status_t BOOT_abort_process(uint8_t channel);

 extern boot_status_t BOOT_start_boot(uint8_t channel, boot_dev_cfg_t *p_dev_cfg,
                                      boot_ebl_version_t *p_ebl_version,
                                      boot_fx_t fx);

 extern boot_status_t BOOT_get_chip_response(uint8_t channel, uint8_t *response, uint32_t alloc_size);

 extern boot_status_t BOOT_get_status(uint8_t channel, char *status_text);

 extern boot_status_t BOOT_clear_status(uint8_t channel);

 extern boot_error_t  BOOT_get_last_error(uint8_t channel, char *error_text);

 extern boot_status_t BOOT_clear_last_error(uint8_t channel);

 extern void BOOT_set_EBL_load_speed(uint8_t channel, uint32_t baudrate);


extern capabilities_t dll_capabilities;
extern capabilities_t target_capabilities[33]; /* 32 channels but index is 1..32  */

extern ext_capabilities_t ext_dll_capabilities;
extern ext_capabilities_t ext_target_capabilities[33]; /* 32 channels but index is 1..32  */

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
  void BOOT_Err(uint8_t channel, uint32_t err);
#endif

#if defined (USE_TEXT_CLASS_IFWD_INFO)
  void BOOT_info(uint8_t channel, uint32_t err);
  void BOOT_show(uint8_t channel, uint32_t err);
#endif
#endif  /* From #ifndef _BOOT_H_ */

#endif
