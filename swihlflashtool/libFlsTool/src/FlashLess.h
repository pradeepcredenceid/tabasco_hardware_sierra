/*
 *       Copyright (C) 2012-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *  Adds a static, dynamic and calibration field in a flash file.
 *  It is also possible to sign the flash file with a certificate.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/FlashLess.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/7 $
 *  $Date:       2014-03-13    15:38:34 UTC $
 *   Resonsible:
 *   Comment:
 *
 */
#ifndef FlashLessH
#define FlashLessH

#include "OS.h"
#include <stdint.h>

// The version number to display
#define   FLASHLESS_VERSION   "1.0.4"

#ifdef __cplusplus
  #include <string>
#endif

/**
 * Doing "Structure Padding" for packing the uint32_t
 */
#ifndef PACKED
  #if (defined (__GNUC__) || defined(__CC_ARM)) \
   && !defined(LINT)
    #define PACKED __attribute__((packed))
  #else
    #define PACKED
    #pragma option push -a1     ///< Packing the uint32_t up against the uint8_t's
  #endif
#endif
typedef struct
{
  uint8_t size;
  uint8_t chip_id;
  uint8_t bc_ver;
  uint8_t bo_cfg;
  uint8_t sec_lvl;
  uint8_t fl_stat;
  uint8_t cus_cfg;
  uint8_t num_x;
  uint8_t num_y;

  uint32_t snum_1 PACKED;
  uint32_t snum_2 PACKED;
  uint32_t snum_3 PACKED;
  uint8_t  v1_boot_code_ver;
  uint8_t  v1_es_ver;
  uint8_t  v1_sub_ver;
  uint8_t  v1_patch_img_ver;
  uint8_t  v1_spare_usif_config;
  uint8_t  v1_spare;
} boot_response_t;
#if (!defined(__GNUC__) && !defined(__CC_ARM))
  #pragma option pop
#endif

/* FlashLess capabilities */
#define FLASHLESS_STATIC_ERASE    1
#define FLASHLESS_DYNAMIC_ERASE   2
#define FLASHLESS_CALIB_ERASE     4


#ifdef __cplusplus
extern "C" {
#endif

  /**
   *  Get the current version of this library
   **/
  char *FlashlessGetVersion(void);
  char *IFWD_get_dll_version(void);  //< DEPRECATED

  /**
   * Set the verbosity (output)
   */
  void FlashlessSetVerbose(uint8_t _verbose);

  /**
   * Copy & return the current error string.
   * If buffer is NULL a pointer to the original error string is returned.
   *
   * @param buffer   Allocated buffer pointer for storing the message
   * @param size     Size of the allocated buffer
   */
  char* FlashlessGetErrorString(char* buffer, uint32_t size);

  /**
   * Used by the Download Library
   *
   * Generates a unique NVM path with BootResp & NVM_Path.
   * Erases files if specified by the NVM_EraseOptions.
   * Runs the FlashlessGenFile
   * Saves the
   * Get the EBL parameters (after generating via FlashlessGenFile)
   */
  uint32_t FlashlessSetPath(uint8_t *RomBootResp,
                       uint8_t **FileName,
                       uint8_t *NVM_Path,
                       uint32_t NVM_EraseOptions,
                       uint8_t **EblData,
                       uint8_t *EblCrc,
                       uint32_t *EblLength);

  uint32_t FlashlessGenFile(
        const char* InFile,
        const char* OutFile,
        const char* NVM_Path,
        const char* CertPath,
        const char* SecPath);

  //< This function does nothing.
  uint32_t FlashlessEnd(void);

  #ifdef __cplusplus
  //< Generic for C++ applications
    uint32_t FlashlessGenerate(
          std::string& InFile,
          std::string& OutFile,
          std::string& NVM_Path,
          std::string& CertPath,
          std::string& SecPath);
  #endif

#ifdef __cplusplus
}
#endif

#endif

