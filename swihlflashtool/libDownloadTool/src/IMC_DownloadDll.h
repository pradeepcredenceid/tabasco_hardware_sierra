/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   Main inferface file for the IFWD download DLL
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/IMC_DownloadDll.h $
 *  $CC-Version: .../dhkristx_140314_v0/3 $
 *  $Date:       2014-05-02    13:10:22 UTC $
 *   Responsible: kofod, Hilmar
 *   Comment:
 *     SMS01221639 updating enum to avoid future misalignment.
 */

#ifndef _INC_IFDW_DL_DLL
#define _INC_IFDW_DL_DLL

/*! @addtogroup DLLinterface DLL Interface
@{
    Detailed Interface Specification of the Download DLL.
*/
#include "OS.h"
#include "FileIO.h"
#include "EBL2_nvm.h"

/*---------------------------------------------*/
/* Enumeration                                 */
/*---------------------------------------------*/

/**
 * @brief Number defining the module - Used for Trace & Debugging
 */
typedef enum
{
  MODULE_NONE = 0,
  MODULE_MAIN,
  MODULE_MFH,
  MODULE_SFH,
  MODULE_TRA,
  MODULE_PROT,
  MODULE_BOOT,
  MODULE_COMM,
  MODULE_USB,
  MODULE_SEC,
  MODULE_CLIENT,
  MODULE_ALL
} MODULE_enum;

/**
 * @brief Return values from IFWD_DownloadDll.DLL functions, and status given to callback functions.
 */
typedef enum
{
  IFWD_DL_OK = 0,                   /**< Operation was okay (if callbacks are used, then wait for it to see final result in the callback) */
  IFWD_DL_Error,                    /**< Error occurred (if callbacks are used, then no callback will be called if this is returned on a function call) */
  IFWD_DL_ProgressUpdated,          /**< This is a percentage progress update in ASCII ("0", "1"..."100"), showing the current progress in the current action, e.g. FLS download. It will only update when the procentage changes.*/
  IFWD_DL_ProcessOutlineUpdated,    /**< This is a general text message from the DLL */
  IFWD_DL_ProcessDetailUpdated,     /**< This is a detailed text message from the DLL (verbose output that can be filtered out unless details are needed) */
  IFWD_DL_PhoneOn,                  /**< Not used */
  IFWD_DL_AT_Command,               /**< AT-Command output has been received from target. This is ASCII text. */
  IFWD_DL_status_force_int_enum = 0xfffff /**< Used to ensure 32-bit enum size. */
} IFWD_DL_status_enum;

/**
 * @brief Definitions of text clases.
 */
typedef enum
{
  IFWD_DL_text_class_ifx_error_code = 0, /**< Text class for error codes */
  IFWD_DL_text_class_os_api_error_code,  /**< Text class for error codes originating from the PC Operating System */
  IFWD_DL_text_class_process_info,       /**< Text class for process informations - not error conditions */
  IFWD_PSI_error_code                    /**< Text class for PSI errors included from the bootloader inc */
} IFWD_DL_text_class_enum;

/**
 * @brief Used to set modem control outputs. Used in #IFWD_DL_modem_control_signals_type.
 */
typedef enum
{
  IFWD_DL_mco_no_change = 0, /**< Leave modem output as it is (no change) */
  IFWD_DL_mco_set_to_0,      /**< Set modem output low */
  IFWD_DL_mco_set_to_1,      /**< Set modem output high */
  IFWD_DL_mco_pulse_0,       /**< Set signal low before boot, and set high when PC is synchronized */
  IFWD_DL_mco_pulse_1        /**< Set signal high before boot, and set low when PC is synchronized */
} IFWD_DL_modem_control_output_enum;

/**
 * @brief Return values when reading modem control inputs. Used in #IFWD_DL_modem_control_signals_type.
 */
typedef enum
{
  IFWD_DL_mci_is_0 = 0,  /**< The modem signal is low. */
  IFWD_DL_mci_is_1       /**< The modem signal is high. */
} IFWD_DL_modem_control_input_enum;


/**
 * @brief Parameters that can be set in the DLL by the #IFWD_DL_set_dll_parameter function.
 */
typedef enum
{
  IFWD_DL_dll_parameter_stay_in_function = 0,        /**< Determines if DLL functions are blocking. If this is set to 1, DLL functions do not return until the action is complete (multiple channels is not possible in that case). */
  IFWD_DL_dll_parameter_boot_process_timeout,        /**< Sets the allowed boot timeout period in ms. This is the maximum time to wait from the PC tries to boot target until target replies. Typical value is 30000 (30 seconds). */
  IFWD_DL_dll_parameter_comm_timeout,                /**< Sets the allowed communication timeout period in ms. This is the maximum time to wait from the PC sends a request to target tries to the time target answers. Typical value is 30000 (30 seconds). */
  IFWD_DL_dll_parameter_use_pre_erase,               /**< Erase FLASH before doing any download. Normally the areas that are written to are erased in the background on target in parallel with programming. Set this to 1 to do all the erasing before any programming is carried out. */
  IFWD_DL_dll_parameter_return_unformatted_text,     /**< Language Codes. Set this to 1 if you want the DLL to send language codes instead of plain English text, e.g. "2|24", or "0|38|1|c:\prj\TheFile.fls". This can be used to translations. */
  IFWD_DL_dll_parameter_skip_empty_blocks,           /**< Empty blocks are not sent to target thus making download faster. Set this to 1 unless you are experiencing trouble during download. All blocks are erased so there is no need to send empty blocks. If the progress bar makes sudden jumps during download this means that empty blocks have been skipped - this is normal. It will not be enabled if the connected phone or DLL does not support it. */
  IFWD_DL_dll_parameter_faster_crc_method,           /**< Makes downloads slightly faster if supported. Set this to 1 unless you are experiencing trouble during download. It will not be enabled if the connected phone or DLL does not support it. Try disabling this option if you get checksum errors. */
  IFWD_DL_dll_parameter_lower_multichannel_CPU_load, /**< Reduce the CPU load when downloading to multiple channels. It may increase download times slightly, but it will make download more reliable when there are many channels in use. A setting of 1 is recommended. */
  IFWD_DL_dll_parameter_erase_mode,                  /**< Determines how FLASH is erased - see #IFWD_DL_dll_parameter_erase_mode_enum. */
  IFWD_DL_dll_parameter_force_area_erase,            /**< Whole Area Erase. Erases the whole area that is being written to is erased regardless of how much data is written, e.g. if it is an FLS file, then all of the area reserved to FLS is erased, regardless of how much data is written. Note, this increases the download time! */
  IFWD_DL_dll_parameter_cond_dyn_eep_erase,          /**< Only erases the dynamic EEPROM area if the target sofware already present is different from what we are downloading. Dynamic EEPROM is normally always erased, but this makes it possible to avoid when the versions match (when set to 1). */
  IFWD_DL_dll_parameter_set_trace_filename,          /**< The "uint32_t parameter_value" in #IFWD_DL_set_dll_parameter is typecast to a "char *" and used to set the filename of the internal IO/function trace. If no filename is specified trace will be made to "IFWD_DownloadDll.log" in current path. */
  IFWD_DL_dll_parameter_set_trace,                   /**< Enables/disables tracing (Argument is verbosity level!) (all external function calls with arguments and return values, and all IO activity are logged. Set #IFWD_DL_dll_parameter_set_trace to 0 to turn off trace before using tracefile. */
  IFWD_DL_dll_parameter_set_nand_bitfield,           /**<  The "uint32_t parameter_value" in #IFWD_DL_set_dll_parameter is typecast to a #IFWD_DL_nand_control_bitfield_type * and is copied into the DLL's internal stores. See #IFWD_DL_nand_control_bitfield_type for details.*/
  IFWD_DL_dll_parameter_force_plain_upload,          /**< If set to 1 it means that no MBN header information is included in the uploaded data. Only used when uploading data with the function #IFWD_DL_upload_bin_image from a target using a NAND FLASH. */
  IFWD_DL_dll_parameter_upload_mbn_data,             /**< Set to 1 if uploading for preflashing, 0 otherwise. Only used when uploading data with the function #IFWD_DL_upload_bin_image from a target using a NAND FLASH. */
  IFWD_DL_dll_parameter_set_usb_auto_mode,           /**< Set to 1 if using a USB port and the USB port is available at the time #IFWD_DL_open_comm_port() is called. If set to 1, the DLL should fail at once if it cannot open the USB port, and not try to open it later by polling. This setting was needed by the MMI when doing downloads to many targets simultaniously using an auto mode (a method for detecting that the target is connected). Ordinarly the USB port is not available before target is connected, so the DLL will return OK and try to open the USB port later. This option disables that behaviour. */
  IFWD_DL_dll_parameter_skip_wr_prak_crc,            /**< If set to 1, then the targets checksum calculation is skipped for packages transferring data to the target. This speeds up the download. Note, it does not disable checksum on the written data - only the write package set to target. */
  IFWD_DL_dll_parameter_check_sign_hw_cfg_value,     /**< If set to 1, then the HW configuration field in the software signature is checked against the actual configuration as reported from the target (the EFUSED values SEC ver X and SEC ver Y). Only valid for EGoldVoice in secure mode. */
  IFWD_DL_dll_parameter_do_not_allow_EEP_mode_write, /**< If set to 1, then test/normal mode is not writte to EEP files(the EEP file is untouched no matter what test/normal setting is set). */
  IFWD_DL_dll_parameter_prolific_gp0,                /**< Sets the PL2303 prolific usb-serial chip's GP0 as an output, and sets its value to the parameter given (the channel is written in the 'status' argument of #IFWD_DL_set_dll_parameter) */
  IFWD_DL_dll_parameter_emulation_tx,                /**< Sets the callback function to emulate Tx instead of using the COM port (used during testing/simulation) */
  IFWD_DL_dll_parameter_emulation_rx,                /**< Sets the callback function to emulate Rx instead of using the COM port (used during testing/simulation) */
  IFWD_DL_dll_parameter_emulation_rx_avail,          /**< Sets the callback function that indicates how many bytes are available in the emulated rx buffer (used during testing/simulation) */
  IFWD_DL_dll_parameter_com_port_emulation,          /**< Enables the com port emulation (requires all channels to be closed and if set TRUE, the Rx/Tx/RxAvail callbacks must be set prior to enabling this.*/
  IFWD_DL_dll_parameter_use_sec_pack_erase_flag,     /**< Use the erase flags in the secure structure. If set the flags for forcing erasure of the whole load-region have effect. Otherwise the selection of erasure of used-length or total-length is controlled by #IFWD_DL_dll_parameter_force_area_erase  */
  IFWD_DL_dll_parameter_allow_hw_channel_switch,     /**< Use to allow switch to SPI or other HS HW channels */
  IFWD_DL_dll_parameter_use_alt_boot_speed,          /**< Used to force boot speed to 460800 */
  IFWD_DL_dll_parameter_USB_certification_test,      /**< Used to perform a USB certification test */
  IFWD_DL_dll_parameter_USB_IN_token_test,           /**< Used to perform a USB data transfer IN token (from target to PC) test */
  IFWD_DL_dll_parameter_USB_OUT_token_test,          /**< Used to perform a USB data transfer OUT token (from PC to target) test */
  IFWD_DL_dll_parameter_USB_loopback_token_test,     /**< Used to perform a USB data transfer loopback (from target to PC) test */
  IFWD_DL_dll_parameter_set_precheck_mode,           /**< Used to force precheck mode, which i essence is downloading without writing to flash memory */
  IFWD_DL_dll_parameter_skip_dl_if_sw_verified,      /**< Only for flashless systems. If set the download is skipped if the boot-ROM and PSI is able to verify that the SW on the target is valid. */
  IFWD_DL_dll_parameter_force_full_emulation,        /**< Include all data transfers in emulation mode */
  IFWD_DL_dll_parameter_set_special_boot_sequence,   /**< If 0 or not used the boot is normal via boot-ROM. If 1 target SW resident EBL is assumed */
  IFWD_DL_dll_parameter_set_winusb_guid,             /**< Sets an alternative GUID when looking for a WinUSB driver */
} IFWD_DL_dll_parameter_enum;


/**
 * @brief Parameters that can be read from DLL by the #IFWD_DL_dll_get_parameter function.
 */
typedef enum
{
  IFWD_DL_dll_get_parameter_filechecksum = 0,   /**< Gets checksum for a downloadable file. */
  IFWD_DL_dll_get_parameter_flash_id,           /**< Gets the Flash ID for the connected target. This is only valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_flash_voltage,      /**< Gets the Flash voltage for the connected target. It is an uint16_t that indicates mv. If not zero then FLASH supports high voltage programming. This is only valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_ebl_info_boot_mode, /**< Gets the boot mode. Can be 0xBB (safe mode), 0xCC (secure mode) or 0xDD (USB mode). This is only valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_ebl_info_major_ver, /**< Gets the major version of the used EBL on target. This is only valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_ebl_info_minor_ver, /**< Gets the minor version of the uarget. This is only valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_ebl_info_name,      /**< Gets the EBL info string. This is onsed EBL on tly valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_ebl_info_cust_cfg,  /**< Returns the custom configuration values (EFUSED on target: SEC X, SEC Y) */
  IFWD_DL_dll_get_parameter_ebl_info_expected_mem_class,  /**< Tells which memory class target wants first. This is only for secure targets where a previous download was interrupted. Target will only accept the same memory class to be downloaded first. If 0 is returned, then nothing is expected. This is only valid AFTER target has booted. */
  IFWD_DL_dll_get_parameter_zip_get_size,       /**< Gets the size in bytes of a ZIP image injected into the FLS file. The value 0 indicates that no ZIP file is found. */
  IFWD_DL_dll_get_parameter_zip_get_data        /**< Returns a pointer to the ZIP image. Always get the ZIP data size before accessing this function */
} IFWD_DL_dll_get_parameter_enum;


/**
 * @brief This is used when setting the DLL parameter #IFWD_DL_dll_parameter_erase_mode by the #IFWD_DL_set_dll_parameter() function. It tells how target should erase FLASH before writing new downloaded data.
   The #IFWD_DL_target_erase_all and #IFWD_DL_target_erase_all_incl_bad_blocks options are only used when downloading FLS or FLB files (it would not make sense to erase the whole FLASH when only downloading e.g. an EEPROM file).
 */
typedef enum
{
  IFWD_DL_target_erase_before_write = 0,   /**< Unconditional erase each area before write. This is default. Will erase the needed areas before downloading to them. */
  IFWD_DL_target_erase_all,                /**< Erase whole flash before download. This will erase the whole physical flash before downloading is started, i.e. all areas will be erased even if nothing is written to them. This only applies to FLS and FLB files (it would not make sense to erase the whole FLASH when only downloading e.g. an EEPROM file). */
  IFWD_DL_target_erase_nothing,            /**< The name is slightly misleading, since it is a conditional erase for each area before write. If the first 256 bytes of an area are empty then nothing is erased. This can be enabled to get faster download, since the erase can be skipped for the pages that are already erased. */
  IFWD_DL_target_erase_all_incl_bad_blocks, /**< Erase whole flash including bad blocks before download (NAND only). Do not check this option unless you are using NAND and want to low level format the NAND FLASH. This option is useful when the NAND blocks have been marked as bad by error. It will can cause target to startup much slower than normal on the first power up, because the entire FLASH will need to be checked. This only applies to FLS and FLB files (it would not make sense to erase the whole FLASH when only downloading e.g. an EEPROM file). */
  IFWD_DL_target_erase_last_valid = IFWD_DL_target_erase_all_incl_bad_blocks /* used for valid range check */
} IFWD_DL_dll_parameter_erase_mode_enum;


/**
 * @brief Target Boot mode setting. Specifies how target starts up when power if turned on (and not download is in progress).
 */
typedef enum
{
  IFWD_DL_target_boot_mode_normal = 0, /**< The phone starts in normal mode after EEP file download. */
  IFWD_DL_target_boot_mode_test        /**< The phone starts in test mode after EEP file download. */
} IFWD_DL_target_boot_mode_enum;

/**
 * @brief Tells what the function #IFWD_DL_download_dffs_file should download. Use IFWD_DL_dffs_load_sel_nand_disk if in doubt.
 */
typedef enum
{
  IFWD_DL_dffs_load_sel_none = 0, /**< No part of the DFFS file is downloaded.  */
  IFWD_DL_dffs_load_sel_static,   /**< Only the static part of the DFFS file is downloaded. */
  IFWD_DL_dffs_load_sel_dynamic,  /**< Only the dynamic part of the DFFS file is downloaded (memory class DYN_FFS_CLASS). */
  IFWD_DL_dffs_load_sel_both,     /**< Both static and dynamic are downloaded. */
  IFWD_DL_dffs_load_sel_nand_disk /**< Download NAND disk image (memory class ROOT_DISK_CLASS, CUST_DISK_CLASS or USER_DISK_CLASS). */
} IFWD_DL_dffs_load_sel_enum;

/**
 * @brief Values to use with function call #IFWD_DL_set_dll_parameter and parameter type #IFWD_DL_dll_parameter_set_special_boot_mode - to modify boot sequence flow.
 */
typedef enum
{
  IFWD_DL_boot_seq_normal = 0,          /**< (default) Normal flow using Boot-ROM */
  IFWD_DL_boot_seq_use_sw_resident_ebl, /**< Expect the EBL to be build-in the target SW - skip all except the last step of the process where EBL sends the start-up mode and capability list */
  IFWD_DL_boot_seq_last_valid = IFWD_DL_boot_seq_use_sw_resident_ebl /* used for valid range check */
} IFWD_DL_boot_seq_enum;

/*---------------------------------------------*/
/* Structures                                  */
/*---------------------------------------------*/

#define MAC_PRESERVING 0   /**< Means do not erase or download to the specified partitions. Used in IFWD_DL_nand_control_bitfield_type. */
#define MAC_INCLUDING  1   /**< Means only flash the specified partitions (leave the rest alone). Used in IFWD_DL_nand_control_bitfield_type */
#define MAC_NOSPECIFICATION 0xFFFF  /**< Used in Normal mode to say that there are no special partition handling. Used in IFWD_DL_nand_control_bitfield_type */

/**
  Specifies which NAND partitions are allowed to be changed or which are allowed to be flashed.

  When flashing in Binary mode an whole image containing all NAND partitions is used.
  This structure is used to specify if certain partitions should not be used or written to.
  The Partitions indexes are defined in the "/dwddrv/NAND_drivers/src/NAND_spare_area.h" file.
  This bitfield can be also be used in Normal mode.
  In Normal mode the "Specification" argument can only be MAC_PRESERVING (meaning that certain partitions should be left alone) or MAC_NOSPECIFICATION (no partitions are preserved).
  In Binary mode the "Specification" argument can only be MAC_PRESERVING (meaning that certain partitions should be left alone) or MAC_INCLUDING (meaning that ONLY the specified partions can be flashed).
 */
typedef struct {
  unsigned short Specification;       /**< Specification type. MAC_PRESERVING: do not erase the following partitions. MAC_INCLUDING: only flash the following partitions (not an allowed value in Normal Mode). MAC_NOSPECIFICATION: This value is only allowed in normal mode. It indicates that there are no special handling needed for each of the individual partitions. */
  unsigned char PartitionBit[32];     /**< Holds one bit for each NAND partition (range 0-254). The code to select a partition bit is <code>PartitionBit[partition>>3] |= (1<<(partition & 0x07));</code>.  The file "/dwddrv/NAND_drivers/src/NAND_spare_area.h" contains the partition definitions. */
} IFWD_DL_nand_control_bitfield_type;


/**
 * @brief Modem Control Signals Specifications that can be set and read as input.
 */
typedef struct
{
  uint8_t DTR;  /**< Sets DTR modem output (type is #IFWD_DL_modem_control_output_enum). */
  uint8_t RTS;  /**< Sets RTS modem output (type is #IFWD_DL_modem_control_output_enum). */
  uint8_t CTS;  /**< Result of CTS modem signal read (type is #IFWD_DL_modem_control_input_enum). */
  uint8_t DSR;  /**< Result of DSR modem signal read (type is #IFWD_DL_modem_control_input_enum). */
  uint8_t RING; /**< Result of RING modem signal read (type is #IFWD_DL_modem_control_input_enum). */
  uint8_t RLSD; /**< Result of RLSD modem signal read (type is #IFWD_DL_modem_control_input_enum). */
} IFWD_DL_modem_control_signals_type;


/*---------------------------------------------*/
/* External functions                          */
/*---------------------------------------------*/

#undef  DECLSPEC

#ifdef _WIN32
  #ifdef TEST_MMI
    #define DECLSPEC __declspec(dllimport) __cdecl
  #else
    #define DECLSPEC  __declspec(dllexport)
  #endif

  #if __GNUC__
    #define USE_UNDERSCORE
  #endif
#else
  #if !defined(ANDROID) && defined(__GNUC__) && __GNUC__ >= 4
    #define DECLSPEC
    // __attribute__((visibility("default")))
  #else
    #define DECLSPEC
  #endif
#endif

#ifdef LINT
  #undef  DECLSPEC
  #define DECLSPEC
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/**
   Returns the DLL version.

   @return A DLL version number string, e.g. "4.30.0.0".
    This is the same information you would get if you right click on the DLL and click "Properties" and select the "Version" tab.
    The first number is the branch number, second is the version in that branch. If the DLL is branched off from one of the main branches, then the second number may be postpendend with a special variant name, e.g. "4.30_FASTER_USB.0.0".
    The last two digits are usually not used (in older version this was the interface version of the DLL interface).
    The version reported in the FlashToolE2.exe is the first two numbers in the DLL version, combined with the last two numbers in the FlashToolE2.exe version (to form a unique version number that identifies both the DLL and the FlashTool used).
*/
#ifdef USE_UNDERSCORE
  char* DECLSPEC _IFWD_DL_get_dll_version(void);
#else
  char* DECLSPEC IFWD_DL_get_dll_version(void);
#endif

/**
This function returns the SW version data from the FLS file and from the target. The address where the data is read is determined by the SW_Version_Addr setting from the PRG file (MakePrg) used when the FLS file was created.
As the SW version data is customer specific the DLL can not give any interpretation of the data.
The maximum size of the data is 1400 bytes.
        @param channel        The logical channel (1..N)
        @param fls_file_name  Name of the FLS file.
        @param file_data      Pointer to the area where the SW version data from the file should be written.
        @param target_data    Pointer to the area where the SW version data from the target should be written. If this pointer is NULL only the file data is returned.
        @param data_size      The size of the data area to be read from the file and the target.
        @param *status        Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
        @return #IFWD_DL_status_enum  status.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_get_sw_version_data(uint8_t channel,char *fls_file_name,uint8_t *file_data,uint8_t *target_data,uint16_t data_size,char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_get_sw_version_data(uint8_t channel,char *fls_file_name,uint8_t *file_data,uint8_t *target_data,uint16_t data_size,char *status);
#endif

/**
   Download DLL function set a parameter in the DLL.

   @param parameter_code  This is a #IFWD_DL_dll_parameter_enum that tells what to set. It is stores as a uint8_t to avoid differences in the enum size on different compilers.
   @param parameter_value  The value to set. Dependant on what we are setting this can also be a "char *" that is typecast to a uint32_t.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. No callbacks are used. The function will return at once.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_set_dll_parameter(/*IFWD_DL_dll_parameter_enum*/ uint8_t parameter_code, uintptr_t parameter_value, char* status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_set_dll_parameter(/*IFWD_DL_dll_parameter_enum*/ uint8_t parameter_code, uintptr_t parameter_value, char* status);
#endif

/**
   Download DLL function get a parameter from the DLL.

   @param parameter_code  This is a #IFWD_DL_dll_get_parameter_enum is wanted. It is stores as a uint8_t to avoid differences in the enum size on different compilers.
   @param result_output   Where the result is put by the DLL. If the output is a ASCII string, then result_output must be the address of a char pointer, e.g. "(uint32_t*)&my_string_ptr", where my_string_ptr is declared as "char *my_string_ptr;". After the call the pointer, my_string_ptr, points to the result string. Do not free or delete this string (it is handled in the DLL).
   @param input_parameter  Optional input parameter that can be e.g. a file name dependant in the parameter_code.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_dll_get_parameter(/*IFWD_DL_dll_get_parameter_enum*/ uint8_t parameter_code, uint32_t *result_output, char *input_parameter);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_dll_get_parameter(/*IFWD_DL_dll_get_parameter_enum*/ uint8_t parameter_code, uint32_t *result_output, char *input_parameter);
#endif

/**
   Return the initial chip response (without chip unique data).

   @param  channel     The channel set/read from (1 to N). If only one channel is used, then 1 is used.
   @param  response    Pointer to a area, allocated by the caller, where the chip response data can be stored. This area is zero-set before the response is copied.
   @param  alloc_size  Size of the allocated area. If the allocated area is smaller than the response, the data will be truncated to fit.
   @return IFWD_DL_status_enum   IFWD_DL_OK or IFWD_DL_Error.
*/
#ifdef USE_UNDERSCORE
  uint32_t DECLSPEC _IFWD_DL_get_chip_response(uint8_t channel, uint8_t *response, uint32_t alloc_size);
#else
  uint32_t DECLSPEC IFWD_DL_get_chip_response(uint8_t channel, uint8_t *response, uint32_t alloc_size);
#endif

/**
This function provides a function pointer to the hosts call-back function. The call-back function is used to pass unsolicited information from the DLL to the host program.

Parameter name  Dir.  Function
  @param HostCallBack  Pointer to the call-back function the DLL can call.
  @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
  @return #IFWD_DL_status_enum  status.

The call back function provides a pointer to a status structure which contains all relevant information.
Note: The hosts handling of the call back function must be as short as possible, and in no circumstances may it stop while waiting for user input.
VERY IMPORTANT NOTE: As the callback is called from a thread within the DLL, the host handling of the callback must ensure proper synchronization. For example Borland VCL components can not be called directly in the callback function.
(see section on callback handling in interface specification).
*/
typedef void (*callback_func)(uint8_t, IFWD_DL_status_enum, char *);

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_init_callback(callback_func cb, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_init_callback(callback_func cb, char *status);
#endif

// Added this to bind with C++ functions
//void DECLSPEC IFWD_DL_init_callback_with_data(callback_func cb, void* data);


/**
   Opens a COM port or USB port.

   @param channel  The channel set/read from (1 to N). If only one channel is used, then 1 is used.
   @param *comm_port_name  The COM port ASCII string. Must begin with either "\\\\.\\COM" or "\\\\.\\USB", e.g. "\\\\.\\COM13" for COM port 13 (the "\\\\.\\" is C-style notation for the string "\\.\").
   @param *ctrl_port_name  Must be the same as the 'comm_port_name' string (this was originally used to control some hardware on another COM port, but isn't used anymore - just set it as 'comm_port_name').
   @param baud_rate Serial line speed, e.g. 57600,115200,230400,460800 or 921600. Higher rates are supported, but not guranteed to work dependant on hardware.
   @param *status Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum   IFWD_DL_OK or IFWD_DL_Error. No callbacks are used. The function will return when the port is open or an error occured.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_open_comm_port(uint8_t channel, char *comm_port_name, char *ctrl_port_name, uint32_t baud_rate, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_open_comm_port(uint8_t channel, char *comm_port_name, char *ctrl_port_name, uint32_t baud_rate, char *status);
#endif


  /**
     Opens a prolific COM port to be used for resetting the target via its GPIO pin(s).

     @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
     @param *reset_port_name  The COM port ASCII string. Must begin with "\\\\.\\COM" e.g. "\\\\.\\COM13" for COM port 13 (the "\\\\.\\" is C-style notation for the string "\\.\").
     @param gpio_mask Bit field that defines the gpio to use for reset. Bit 0 is gpio_0 etc.
     @param *status Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
     @return IFWD_DL_status_enum   IFWD_DL_OK or IFWD_DL_Error. No callbacks are used. The function will return when the port is open or an error occured.
  */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_setup_prolific_gpio_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_setup_prolific_gpio_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask, char *status);
#endif


/**
This function closes the specified logical channel. This involves closing the communication port(s) and freeing allocated memory.

  @param channel Logical channel (1..N) to close
  @param *status Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
  @return IFWD_DL_status_enum
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_close_comm_port(uint8_t channel, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_close_comm_port(uint8_t channel, char *status);
#endif

/**
   Force use of separate boot components.

   Use this function to specify Ram-PSI and/or EBL to overrule those injected in the FLS file.
   This function (if used) must be called before #IFWD_DL_boot_target is called.

   @param channel  The channel to boot on (1 to N). If only one channel is used, then 1 is used.
   @param *psi_file_name  Name of a Ram-PSI (.fls) file. Set to NULL if an separate PSI shall not be used.
   @param *ebl_file_name  Name of a EBL (.fls) file. Set to NULL if an separate EBL shall not be used.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.

 */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_force_boot_components(uint8_t channel, char *psi_file_name, char *ebl_file_name);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_force_boot_components(uint8_t channel, char *psi_file_name, char *ebl_file_name);
#endif

/**
   Download DLL function to boot target.

   This function makes target ready to start downloading, instead of starting like normal.
   Target must be reset (or the power turned on) after calling this function.

   @param channel  The channel to boot on (1 to N). If only one channel is used, then 1 is used.
   @param *ref_file  Name of a file from which the hardware info is read. This is typically the first FLS file to download.
   @param *modem_control_signals  Describes how to set the modem output signals DTR and RTS just after boot starts (if any changes). This can be used to power on the phone.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_boot_target(uint8_t channel, char *ref_file, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_boot_target(uint8_t channel, char *ref_file, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status);
#endif

  /**
     Download DLL function to do first stage of the target boot process.

     This function sends ATAT to the target and receives the chip response.
     Target must be reset (or the power turned on) after calling this function.
     The function #IFWD_DL_get_chip_response can be called afterward to get the chip response data.
     The function #IFWD_DL_boot_target can be called afterward to complete the boot process.
     NOTE this function only works for S-Gold and X-Gold chips.

     Platform codes currently defined are: (defined in the file 'devices.h')
     E_GOLD_CHIPSET_V2     0
     E_GOLD_LITE_CHIPSET   1
     S_GOLD_CHIPSET_V1     2
     S_GOLD_LITE_CHIPSET   3
     E_GOLD_CHIPSET_V3     4
     S_GOLD_CHIPSET_V2     5
     S_GOLD_CHIPSET_V3     6
     E_GOLD_RADIO_V1       7
     E_GOLD_VOICE_V1       8
     S_GOLD_RADIO_V1       9
     X_GOLD_618            10
     X_GOLD_102            11
     X_GOLD_213            12
     X_GOLD_110            13
     X_GOLD_626            14
     X_GOLD_223            15
     X_GOLD_215            16
     X_GOLD_706            17
     X_GOLD_636            18
     X_GOLD_225            19
     X_GOLD_716            20
     X_GOLD_631            21
     X_GOLD_221            22
     X_GOLD_726            23
     X_GOLD_632            24
     X_GOLD_222            25
     X_GOLD_633            26

     @param channel  The channel to boot on (1 to N). If only one channel is used, then 1 is used.
     @param platform_code  A code that is used to select which chip to boot on.
     @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
     @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
  */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_pre_boot_target(uint8_t channel, uint32_t platform_code, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_pre_boot_target(uint8_t channel, uint32_t platform_code ,char *status);
#endif

/**
   Download DLL function to boot target.

   This function makes target ready to start downloading, instead of starting like normal.
   Target must be reset (or the power turned on) after calling this function.
   The difference between this function and IFWD_DL_boot_target, is that this function gets a pointer to the reference data instead of a file.

   @param channel  The channel to boot on (1 to N). If only one channel is used, then 1 is used.
   @param *ref_file_header Pointer to FLS header data.
   @param *modem_control_signals Describes how to set the modem output signals DTR and RTS just after boot starts (if any changes). This can be used to power on the phone.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_boot_bin_target(uint8_t channel, uint8_t *ref_file_header, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_boot_bin_target(uint8_t channel, uint8_t *ref_file_header, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status);
#endif

/**
   Sets modem control output signals (DTR and RTS), and reads back the modem input signals (CTS, DSR, RING and RLSD).

   This function can be used to set and/or read modem control signals.

   It can be a bit confusing that it can do both at the same time, but if e.g. no output is wanted then just to set the DTR and RTS to IFWD_DL_mco_no_change.
   When the function returns the given "modem_control_signals" structure will contain the read signals for CTS, DSR, RING and RLSD.

   @param channel  The channel set/read from (1 to N). If only one channel is used, then 1 is used.
   @param *modem_control_signals  Pointer to modem control signals to used to set and read modem control signals.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum   IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_set_get_control_signal(uint8_t channel, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_set_get_control_signal(uint8_t channel, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status);
#endif

/**
  Call this function to download a FLS file.

   @param channel  The channel to download to (1 to N). If only one channel is used, then 1 is used.
   @param *fls_file_name  Pointer to filename to download. Prefix the filename with "|nn|" to specify download of TOC index nn. If TOC index is not specified the index 1 is used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_fls_file(uint8_t channel, char *fls_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_fls_file(uint8_t channel, char *fls_file_name, char *status);
#endif

/**
  Call this function to download an EEP file.
  The target can be set to start in normal or test mode depending on the 'BootMode' parameter.

   @param channel  The channel to download to (1 to N). If only one channel is used, then 1 is used.
   @param *eep_file_name  Pointer to filename to download. Prefix the filename with "|nn|" to specify download of TOC index nn. If TOC index is not specified the index 1 is used.
   @param boot_mode  0 for normal mode else test mode (is of #IFWD_DL_target_boot_mode_enum type).
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_eep_file(uint8_t channel, char *eep_file_name,  /*IFWD_DL_target_boot_mode_enum*/ uint8_t boot_mode, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_eep_file(uint8_t channel, char *eep_file_name,  /*IFWD_DL_target_boot_mode_enum*/ uint8_t boot_mode, char *status);
#endif

/**
   Download a DFFS file.

   @param channel  The channel set/read from (1 to N). If only one channel is used, then 1 is used.
   @param *dffs_file_name  Pointer to the filename to download. Prefix the filename with "|nn|" to specify download of TOC index nn. If TOC index is not specified the index 1 is used.
   @param load_selection   A #IFWD_DL_dffs_load_sel_enum that tell which part to download, use IFWD_DL_dffs_load_sel_nand_disk if in doubt.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum   IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_dffs_file(uint8_t channel, char *dffs_file_name, /*IFWD_DL_dffs_load_sel_enum*/ uint8_t load_selection, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_dffs_file(uint8_t channel, char *dffs_file_name, /*IFWD_DL_dffs_load_sel_enum*/ uint8_t load_selection, char *status);
#endif

/**
   Download a Cust file.

   @param channel  The channel set/read from (1 to N). If only one channel is used, then 1 is used.
   @param *cust_file_name  Pointer to the filename to download. Prefix the filename with "|nn|" to specify download of TOC index nn. If TOC index is not specified the index 1 is used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum   IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_cust_file(uint8_t channel, char *cust_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_cust_file(uint8_t channel, char *cust_file_name, char *status);
#endif


/* This function is not implemented */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_upload_eep_file(uint8_t channel, char *template_file_name, char *uploaded_eep_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_upload_eep_file(uint8_t channel, char *template_file_name, char *uploaded_eep_file_name, char *status);
#endif

/**
The host calls this function to update a selection of parameters in the static EEPROM on the target.

The process is:
<ul>
<li>The whole static EEPROM image is uploaded and stored temporarily in the DLL.</li>
<li>The delta update algorithm is then applied to update the image.</li>
<li>The static EEPROM image is erased in the target.</li>
<li>The updated EEPROM image is downloaded into the target.</li>
</ul>

The update process uses a DEP file which contains the parameters to be updated and their new values. Furthermore it uses two CFG files which contain the layout for a specific EEPROM version. One CFG file is used for parsing the current EEPROM content and the other CFG file is used when the updated image is written.

The CFG files are selected automatically by reading the expected EEPROM version from the FLS image in the target and the version parameter from the static EEPROM image which is uploaded from the target.

As different target projects can have different EEPROM layouts, the parameter 'ProjectPrefix' is used to give the project name which is used for the automatic file selection.

The DEP file is a pure text file and it does not contain the information's needed to provide read/write access to the static EEPROM area. Therefore the name of a valid EEP file must also be given as parameter to this function. Please note that this file is only used for reference - it is not downloaded.

   @param channel  The channel to use (1 to N). If only one channel is used, then 1 is used.
   @param dep_file_name  Name of the DEP file.
   @param project_prefix_string  Project prefix string.
   @param ref_eep_file_name The name of the EEP file to be used as reference.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_update_static_eeprom(uint8_t channel, char *dep_file_name, char *project_prefix_string, char *ref_eep_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_update_static_eeprom(uint8_t channel, char *dep_file_name, char *project_prefix_string, char *ref_eep_file_name, char *status);
#endif

/**
   Uploads a Binary Image.

   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param image_start_address  The absolute address of the image to be read.
   @param image_length  The length (in bytes) of the image to be read.
   @param bin_file_name The name of the BIN file in which the uploaded image is stored.
   @param ref_file_name Name of file to be used as reference.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_upload_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *bin_file_name, char *ref_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_upload_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *bin_file_name, char *ref_file_name, char *status);
#endif



/**
Call this function to download a binary file to the targets flash.

NOTE: For security reasons there are parts of the flash image that may not be downloaded to, and therefore the name of a valid downloadable file (FLS, EEP, DFFS or CUST) must be given as parameter. Please note that this file is only used for reference - it is not downloaded.
If no file name is given, the file specified during boot will be used.
The security parameters within the referenced file will determine if the read access to the requested area can be granted.
If read is not allowed an error will be issued.

   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param image_start_address The absolute address of the image to be written.
   @param image_length  The length (in bytes) of the image to be written. If the length is set to 0, data from 'offset' to the end of the file is automatically downloaded.
   @param image_offset The offset into the BIN file where the image is read.
   @param bin_file_name  The name of the BIN file to be downloaded.
   @param ref_file_name  Name of file to be used as reference.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, uint32_t image_offset, char *bin_file_name, char *ref_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, uint32_t image_offset, char *bin_file_name, char *ref_file_name, char *status);
#endif


/**
Call this function to erase a part of the targets flash memory.
NOTE: For security reasons there are parts of the flash image that may not be erased and therefore the name of a valid downloadable file (FLS, EEP, DFFS or CUST) must be given as parameter.
Please note that this file is only used for reference - it is not downloaded.
If no file name is given, the file specified during boot will be used.

The security parameters within the referenced file will determine if the read access to the requested area can be granted.
If read is not allowed an error will be issued.

   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param image_start_address The absolute address of the image to be erased.
   @param image_length The length (in bytes) of the image to be erased. If the length is set to 0, the DLL will perform a ChipErase.
   @param *ref_file_name Name of file to be used as reference.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_erase_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length,char *ref_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_erase_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length,char *ref_file_name, char *status);
#endif

/* This function is not implemented */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_verify_flash_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *ref_file_name, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_verify_flash_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *ref_file_name, char *status);
#endif

/**
This function terminates the current process on the selected channel.
The state of the target can not be guaranteed after aborting a process, but if power to target is toggled a new download can be performed if needed.
This function only makes sense to call when #IFWD_DL_dll_parameter_stay_in_function is 0 (otherwise there is nothing to abort).

   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_abort_process(uint8_t channel, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_abort_process(uint8_t channel, char *status);
#endif

/**
  This function is used to issue a HW reset on the target.
  The reset procedure is hardware dependant, so there are four different parameters that specify how the reset should be carried out.
  It is beyond the scope of this document to explain the correct values for a given hardware - If default values do not work, then contact Infineon for help.
   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param mode Currently only mode = 1 is supported (EGold lite+ SW-I2C). For platforms where the reset functionality is integrated with the CPU on the same chip the parameters and mode values are not used.
   @param p1  Default value=16. Number of the PCL used for the I2C clock signal.
   @param p2  Default=17. Number of the PCL used for the I2C data signal.
   @param p3  Default=0. Not currently used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_force_target_reset(uint8_t channel, uint8_t mode, uint8_t p1, uint8_t p2, uint8_t p3, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_force_target_reset(uint8_t channel, uint8_t mode, uint8_t p1, uint8_t p2, uint8_t p3, char *status);
#endif

/**
  The DLL does not know when the last download is completed. This function is used to tell the DLL when all downloads are done.
  This function sends an 'End of process' message to the target. The host must call this function when all the scheduled processes are done. The target can then use it to control status lights, calculate checksum etc.

   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_end_of_process(uint8_t channel, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_end_of_process(uint8_t channel, char *status);
#endif

/*--------------------*/
/* AT-Mode functions: */
/*--------------------*/

/**
   Call this function to enable the AT-command mode.
   @param channel  The channel to switch to AT-mode on (1 to N). If only one channel is used, then 1 is used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_start_AT_mode(uint8_t channel, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_start_AT_mode(uint8_t channel, char *status);
#endif

/**
   Call this function to stop AT-command mode, so download can be carried out again.
   @param channel  The channel to stop AT-mode on (1 to N). If only one channel is used, then 1 is used.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
   */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_stop_AT_mode(uint8_t channel, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_stop_AT_mode(uint8_t channel, char *status);
#endif

/**
   Call this function to send an AT-command to target.
   The function #IFWD_DL_start_AT_mode must have been called prior to calling this, to set target in AT-mode, and when finished #IFWD_DL_stop_AT_mode should be called.
   Any AT command responses are sent using the callback function (see #IFWD_DL_init_callback).

   @param channel  The channel to send AT-command on (1 to N). If only one channel is used, then 1 is used.
   @param *data The ASCII data to send including the "<CR>" that AT commands end with.
   @param  length The number of data bytes to send.
   @param *status  Pointer to text buffer where any errors can be described. The buffer should be able to hold a minimum of 500 chars.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_AT_send(uint8_t channel, char *data, uint16_t length, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_AT_send(uint8_t channel, char *data, uint16_t length, char *status);
#endif

/**
 * @brief File handling emulation in case customer wants to use their own memory based file system.

 @param *ExtFileHandlers Set this to NULL to revert to default Harddisk operation, or set it to point to a #IFWD_ExtFileHandleType.

NOTE: No files in any channels are allowed to be open when calling this function.
Failure to comply with this can lead to memory leaks and unclosed files, and invalid handles.
*/
#ifdef USE_UNDERSCORE
  void DECLSPEC _IFWD_DL_set_external_file_handling(IFWD_ExtFileHandleType *ExtFileHandlers);
#else
  void DECLSPEC IFWD_DL_set_external_file_handling(IFWD_ExtFileHandleType *ExtFileHandlers);
#endif


/*------------------------------------------------*/
/* TOC handling functions (toc index starts at 0) */
/*------------------------------------------------*/

/**
   Gets the number of downloadable items contained in a single file.
   All downloadable files will have at least one TOC element (TOC = Table Of Contents).

   @param *filename The filename to examine.
   @param *nof_output Where to write to number result.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_nof_items(char *filename, uint32_t *nof_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_nof_items(char *filename, uint32_t *nof_output);
#endif

/**
   Gets the memory class of a specified index in the Table Of Contents for a file.
   The memory classes are define in the header file: libFlsTool/src/FlsMetaFile.h, e.g. CODE_CLASS.

   @param *filename The filename to examine.
   @param toc_index Index to look at. First index is 0. All files will have at least one item.
   @param *mem_class_output Where to write to result.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_memory_class(char *filename, uint32_t toc_index, uint32_t *mem_class_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_memory_class(char *filename, uint32_t toc_index, uint32_t *mem_class_output);
#endif


/**
   Gets the filename given for the given index.
   This is only used to display a user friendly name in the MMI when presenting the list of TOC files in a file. It is not really needed.
   The given filename may only be a dummy name dependant on what was put there during the release procedure.

   @param *filename The filename to examine.
   @param toc_index Index to look at. First index is 0. All files will have at least one item.
   @param *filename_output Where to write the file name. The buffer must be able to hold at least 300 bytes.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_filename(char *filename, uint32_t toc_index, char *filename_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_filename(char *filename, uint32_t toc_index, char *filename_output);
#endif

/**
   Gets the checksum for a given TOC index in a filename.

   @param *filename The filename to examine.
   @param toc_index Index to look at. First index is 0. All files will have at least one item.
   @param *checksum_output Where to write the checksum.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_checksum(char *filename, uint32_t toc_index, uint32_t *checksum_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_checksum(char *filename, uint32_t toc_index, uint32_t *checksum_output);
#endif


/**
   Gets the filesize for a given TOC index in a filename.

   @param *filename The filename to examine.
   @param toc_index Index to look at. First index is 0. All files will have at least one item.
   @param *filesize_output Where to write the filesize.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_filesize(char *filename, uint32_t toc_index, uint32_t *filesize_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_filesize(char *filename, uint32_t toc_index, uint32_t *filesize_output);
#endif

/**
   Gets the flash technology expected by a given filename.

   @param *filename The filename to examine.
   @param *flash_technology_output Where to write the flash technology value as defined in FlsMetaFile.h.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_flash_technogy(char *filename, uint32_t *flash_technology_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_flash_technogy(char *filename, uint32_t *flash_technology_output);
#endif

/**
   Gets the HW platform expected by a given filename.

   @param *filename The filename to examine.
   @param *hw_platform_output Where to write the HW platform value as defined in FlsMetaFile.h.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_hw_platform(char *filename, uint32_t *hw_platform_output);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_hw_platform(char *filename, uint32_t *hw_platform_output);
#endif

typedef enum
{
  IFWD_DL_sw_option_flashless_enabled = 0,        /**< Make the function check to see if the specified SW contains the info needed to generate the NVM files for Flashless system. */
  IFWD_DL_sw_option_force_u32_enum = 0x000FFFFF   /**< Force enum to 32 bit size.  */
} IFWD_DL_sw_option_code_enum;

/**
   Check to see if a given option is availabel in a given SW.

   @param *filename The filename to examine.
   @param toc_index Index to look at. First index is 0. All files will have at least one item.
   @param option_code An option to check for in the given SW, as defined by #IFWD_DL_sw_option_code_enum
   @param *option_result 0x00000000 if the option is not available.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_sw_option(char *filename,   uint32_t toc_index,   /* IFWD_DL_sw_option_code_enum */ uint32_t option_code,   uint32_t *option_result);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_sw_option(char *filename,   uint32_t toc_index,   /* IFWD_DL_sw_option_code_enum */ uint32_t option_code,   uint32_t *option_result);
#endif

/**
   Get a value of the System SW Version part inside the Code section.

   @param *filename The filename to examine.
   @param option_code An option to check for in the given SW, as defined by #IFWD_DL_sys_version_code_enum
   @param *option_result Buffer where to save the resulting strings
   @param size Size of the option_result buffer
   @return IFWD_DL_status_enum.
*/

typedef enum
{
  IFWD_DL_sys_version_short = 0,
  IFWD_DL_sys_version_customer,
  IFWD_DL_sys_version_subsystem,
  IFWD_DL_sys_version_all,
} IFWD_DL_sys_version_code_enum;

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_get_sys_version(const char *filename, uint32_t option_code, char *option_result, uint32_t size);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_get_sys_version(const char *filename, uint32_t option_code, char *option_result, uint32_t size);
#endif


/**
   Set the parameters for handling of NVM areas on flash-less sytems.

   @param *nvm_path   The path (without trailing backslash) to the folder where the target specific NVM files are stores.
   @param nvm_options Options to detirmine if the areas should be erased.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum  DECLSPEC _IFWD_DL_NVM_set_flashless_option (uint8_t *nvm_path, uint32_t nvm_options);
#else
  IFWD_DL_status_enum  DECLSPEC IFWD_DL_NVM_set_flashless_option (uint8_t *nvm_path, uint32_t nvm_options);
#endif

/**
   Gets the number of progress bars that will be displayed during download of a given file with the current settings.

   @param channel  The channel (1 to N). If only one channel is used, then 1 is used.
   @param *filename The filename to examine.
   @param *weights_output Not used yet - set to NULL. It is intended to give the total progress bar a more smooth progress by weighting quicker operations less. The weight for each progressbar is written here. E.g. if the erase progress is estimated to be less time consuming, then the weight will be less. The given pointer must be able to hold 50 int values.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  uint32_t DECLSPEC _IFWD_DL_dll_get_nof_progressbars(uint8_t channel, char *filename, uint32_t *weights_output);
#else
  uint32_t DECLSPEC IFWD_DL_dll_get_nof_progressbars(uint8_t channel, char *filename, uint32_t *weights_output);
#endif

/**
   Gets the number of NVM items for which there are user options to be set.

   @param *filename The name of the file containing the NVM structure.
   @param *nof_options The number of options.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_NVM_get_nof_user_options(char *filename, uint32_t *nof_options);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_NVM_get_nof_user_options(char *filename, uint32_t *nof_options);
#endif

/**
   Gets the NVM user option tag for the given index.

   @param *filename The name of the file containing the NVM structure.
   @param index Index of the tag to be returned.
   @param *nvm_tag The NVM structure tag as defiened by #T_NVM_TAG.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_NVM_get_user_option(char *filename, uint32_t index, uint32_t *nvm_tag);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_NVM_get_user_option(char *filename, uint32_t index, uint32_t *nvm_tag);
#endif

/**
   Set the user option for the given index.

   @param *filename The name of the file containing the NVM structure.
   @param index Index of the tag for which the option must be set.
   @param value true for setting the option.
   @return IFWD_DL_status_enum.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_NVM_set_user_option(char *filename,uint32_t index, uint8_t value);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_NVM_set_user_option(char *filename,uint32_t index, uint8_t value);
#endif


/**
   Puts data into the DLL trace file (if trace is enabled). Is used to add useful information like program and driver versions or whatever program using the DLL finds useful.

   @param channel What channel to write the trace into to (1-32).
   @param *data Pointer to data to write.
   @param length Length of data to write (if it is a string, then remember to include the NULL-termination)
   @param tr_code Tells the TraceViewer how to interpretate the trace data. 1 means ASCII string, 0 is reserved for raw binary data, but you can define your own types, and tell the TraceViewer how to display the data and save it in the .DDF file for the traceviewer.
   @return true if trace is enabled (data was written), FALSE if trace is disabled.
*/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_DL_trace_insert(uint8_t channel, uint8_t *data, uint32_t length, uint8_t tr_code);
#else
  uint8_t DECLSPEC IFWD_DL_trace_insert(uint8_t channel, uint8_t *data, uint32_t length, uint8_t tr_code);
#endif


/**
 * @brief Definition of actions possible over the OutOfSessionComm interface.
*/
typedef enum
{
  IFWD_DL_oosc_action_nothing = 0, /**< Take no action. */
  IFWD_DL_oosc_action_write,       /**< Write data according to its type */
  IFWD_DL_oosc_action_read,        /**< Read data according to its type  */
  IFWD_DL_oosc_action_erase,       /**< Erase data according to its type */
  IFWD_DL_oosc_action_commit,      /**< Tells target that we are done with everything */
  IFWD_DL_oosc_nof_actions         /**< Used to check parameter values   */
} IFWD_DL_oosc_action_enum;

/**
 * @brief Definition of data types that can be handled over the OutOfSessionComm
 *        Interface.
*/
typedef enum
{
  IFWD_DL_oosc_type_hw_details = 0, /**< HW details structure as received during the boot process */
  IFWD_DL_oosc_type_rd_cert,        /**< R&D certificate                            */
  IFWD_DL_oosc_type_cc_cert,        /**< CC  certificate                            */
  IFWD_DL_oosc_type_hw_cert,        /**< HW  certificate                            */
  IFWD_DL_oosc_type_mi_cert,        /**< Mobile ID                                  */
  IFWD_DL_oosc_type_ticket,         /**< Ticket                                     */
  IFWD_DL_oosc_type_secure_area,    /**< All of above in the entire secure area     */
  IFWD_DL_oosc_type_manifest,       /**< Manifest data type                         */
  IFWD_DL_oosc_type_all_flash,      /**< The whole flash area                       */
  IFWD_DL_oosc_type_flash_first_16, /**< The first 16 words programmed to the flash */
  IFWD_DL_oosc_type_flash_id,       /**< The first 16 words in flash in ID-Mode     */
  IFWD_DL_oosc_type_xml_data,       /**< XML meta data for control of target action */
  IFWD_DL_oosc_type_nand_bad_blocks, /**< Status of the blocks in a NAND flash      */
  IFWD_DL_oosc_type_key_cert,       /**< Key Ring certificate */
  IFWD_DL_oosc_nof_types            /**< Used to check parameter values             */
} IFWD_DL_oosc_type_enum;

/**
  Call this function to perform an action over the OutOfSessionComm interface.

   @param channel  The channel to download to (1 to N). If only one channel is used, then 1 is used.
   @param action   An action as defined by #IFWD_DL_oosc_action_enum.
   @param type     A data type as defined by #IFWD_DL_oosc_type_enum.
   @param length   The length in bytes of the data to be transferred.
   @param spare    Currently not used. Must be set to 0x00000000.
   @param *data    Pointer to the structure owned by the caller where the data is read from or written to
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error. If IFWD_DL_OK is return and callbacks are used, then wait for the callback result.
*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_out_of_session_comm(uint8_t  channel,
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_out_of_session_comm(uint8_t  channel,
#endif
                                                         uint32_t action,
                                                         uint32_t type,
                                                         uint32_t length,
                                                         uint32_t spare,
                                                         uint8_t  *data);

/**
   Call back function used for the secure channel functions.

     @param secure_channel_data_type    Identifies the type of blob_data. 0=NonceAndVersion, 1-N Not defined.
     @param secure_channel_data_length  Length of the blob_data in bytes.
     @param secure_channel_data_ptr     Pointer to the blob data. (Do not expect the data to be valid after this call back function returns)
*/

typedef uint32_t (*sec_ch_callback_type)(uint32_t*, size_t*, uint8_t**); //

/**
   Call this function to setup the secure channel operation.

   @param channel      The channel to download to (1 to N). If only one channel is used, then 1 is used.
   @param options      Set bits to enable options. Bit 0: Force creation of RD cert.  Bit 1..31 Not used (set to 0)
   @param callback_fx  Function pointer for call back.
   @param cert_files   Pointer to NULL terminated array of string pointers, e.g. {"a_file", "another_file", "last_file", NULL}.
   @return #IFWD_DL_status_enum  IFWD_DL_OK or IFWD_DL_Error.
*/


#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files, char *status);
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files, char *status);
#endif

/**
   Changes the physical device port where to send data for the download.
   The boot phase is done so we change to p_fw_download_port_name for downloading the firmware.
   We use the same logical channel since the channel context is not changed.

   @param channel What channel to write the trace into to (1-32).
   @param *p_fw_download_port_name name of the device used for the firmware download.
   @param baud_rate Serial line speed, e.g. 57600,115200,230400,460800 or 921600. Higher rates are supported, but not guranteed to work dependant on hardware.
 **/
void IFWD_switch_port(uint8_t channel, char *p_fw_download_port_name, uint32_t baud_rate);

/**
 *  In case the library is used by another process, this function can retrieve an ampty channel
 *
 * @return Either a channel number or zero for "All channels occupied"
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_get_next_free_channel();
#else
  uint8_t DECLSPEC IFWD_get_next_free_channel();
#endif

/**
 *  Retrieve the communication handle fd in order to perform low level I/O Control (ioctl)
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_get_comm_handle(uint8_t id);
#else
  uint8_t DECLSPEC IFWD_get_comm_handle(uint8_t id);
#endif

/**
 * Runs the USB Device Mapper
 *
 * @return An allocated path for the device id
 **/
#ifdef USE_UNDERSCORE
  char* DECLSPEC _IFWD_device_map(uint8_t id);
#else
  char* DECLSPEC IFWD_device_map(uint8_t id);
#endif

/**
 * Saves the current USB Device Map
 *
 * @return true on success
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_device_map_save(const char* map);
#else
  uint8_t DECLSPEC IFWD_device_map_save(const char* map);
#endif

/**
 * Loads a USB Device Map
 *
 * @return true on success
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_device_map_load(const char* map);
#else
  uint8_t DECLSPEC IFWD_device_map_load(const char* map);
#endif

/**
 * Sets the verbosity of individual modules.
 *
 **/
#ifdef USE_UNDERSCORE
  void DECLSPEC _IFWD_set_module_verbose(MODULE_enum module, int verbose);
#else
  void DECLSPEC IFWD_set_module_verbose(MODULE_enum module, int verbose);
#endif

/**
 *  Describe Test_Setup()
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _Test_Setup(void *data);
#else
  uint8_t DECLSPEC Test_Setup(void *data);
#endif


#ifdef __cplusplus
  }
#endif

#endif
/*! @} */
