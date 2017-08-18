/*   ----------------------------------------------------------------------
     Copyright (C) 2014 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     Copyright (C) 2009 - 2010 Infineon Technologies Denmark A/S (IFWD)
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/inc/ebl2_files.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-29    11:20:42 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */
#ifndef __EBL2_FILES_H__
#define __EBL2_FILES_H__

#ifdef FLASHTOOL_E2
  #define FILES_DEF(eNUM,tEXT) tEXT,
  #define files_enum_type
#else
  #define FILES_DEF(eNUM,tEXT) eNUM,
  #define files_enum_type files_enum_t
#endif


#ifdef FLASHTOOL_E2
  const char* const ebl2_files[] =
#else
  typedef enum
#endif
{
   FILES_DEF(eblfile_version,"version")
   FILES_DEF(eblfile_EBL2_error,"EBL2_error")
   FILES_DEF(eblfile_ebl,"ebl")
   FILES_DEF(eblfile_prot,"prot")
   FILES_DEF(eblfile_usart0,"usart0")
   FILES_DEF(eblfile_EBL2_flash_drv_nor,"EBL2_flash_drv_nor")
   FILES_DEF(eblfile_EBL2_storage_nor,"EBL2_storage_nor")
   FILES_DEF(eblfile_comm_dev,"comm_dev")
   FILES_DEF(eblfile_platform,"platform")
   FILES_DEF(eblfile_cryptoAPI,"cryptoAPI")
   FILES_DEF(eblfile_dslb_rsa_init,"dslb_rsa_init")
   FILES_DEF(eblfile_EBL2_stubs,"EBL2_stubs")
   FILES_DEF(eblfile_rsa_egold,"rsa_egold")
   FILES_DEF(eblfile_sha1_egold,"sha1_egold")
   FILES_DEF(eblfile_EBL2_security,"EBL2_security")

   /* Added files due to synchronous mode restructuring */
   /* Keep old definitions for backward compatibility   */
   FILES_DEF(eblfile_error_app,"error_app")
   FILES_DEF(eblfile_flashlogic_app,"flashlogic_app")
   FILES_DEF(eblfile_main_app,"main_app")
   FILES_DEF(eblfile_prot_1_16_app,"prot_1_16_app")
   FILES_DEF(eblfile_security_app,"security_app")
   FILES_DEF(eblfile_storage_app,"storage_app")
   FILES_DEF(eblfile_asc0_drv,"asc0_drv")
   FILES_DEF(eblfile_asc1_drv,"asc1_drv")
   FILES_DEF(eblfile_comm_drv,"comm_drv")
   FILES_DEF(eblfile_rsa_egold_drv,"rsa_egold_drv")
   FILES_DEF(eblfile_rsa_egold_init_drv,"rsa_egold_init_drv")
   FILES_DEF(eblfile_sha1_egold_drv,"sha1_egold_drv")
   FILES_DEF(eblfile_crypto_hal_drv,"crypto_hal_drv")
   FILES_DEF(eblfile_flash_drv,"flash_drv")
   FILES_DEF(eblfile_platform_drv,"platform_drv")
   FILES_DEF(eblfile_prot_2_32_app,"prot_2_32_app")
   FILES_DEF(eblfile_prot_3_32_app,"prot_3_32_app")
   FILES_DEF(eblfile_storage_nor,"storage_nor")
   FILES_DEF(eblfile_storage_sd,"storage_sd")
   FILES_DEF(eblfile_storage_nand,"storage_nand")
   FILES_DEF(eblfile_storage_onenand,"storage_onenand")
   FILES_DEF(eblfile_storage_ram,"storage_ram")
   FILES_DEF(eblfile_storage_emmc,"storage_emmc")
   FILES_DEF(eblfile_storage_pcm,"storage_pcm")
   FILES_DEF(eblfile_last_file, "Dummy_file")
} files_enum_type;

#endif
