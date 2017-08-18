/* ----------------------------------------------------------------------------

   Copyright (C) 2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   ----------------------------------------------------------------------------

   This document contains proprietary information belonging to IMC.
   Technologies Denmark A/S. Passing on and copying of this document, use
   and communication of its contents is not permitted without prior written
   authorisation.

   Revision Information:
  $File name:  /msw_tools/FlashTool/libFlsTool/inc/ebl2_std_errors.h $
  $CC-Version: .../dhkristx_140314_v0/1 $
  $Date:       2014-04-29    11:20:44 UTC $
  ---------------------------------------------------------------------------*/
#ifndef __EBL2_STD_ERROR_H__
#define __EBL2_STD_ERROR_H__

#ifdef FLASHTOOL_E2
  #define ERROR_DEF(eNUM,tEXT) tEXT,
  #define cfi_func_enum_type

  // Used for hosttest
  #if defined(EBL2_STD_ERROR_AS_ENUM_NAME)
    #undef ERROR_DEF
    #define ERROR_DEF(eNUM,tEXT) # eNUM,
  #endif
#else
  #undef ERROR_DEF
  #define ERROR_DEF(eNUM,tEXT) eNUM,
  #define cfi_func_enum_type cfi_func_enum_t
#endif


#ifdef FLASHTOOL_E2
  const char* const ebl2_std_errors[] =
#else
  typedef enum
#endif
{
   ERROR_DEF(eblerr_no_error,"No error.")
   ERROR_DEF(eblerr_free_text_full,"No room for more free text error messages.")
   ERROR_DEF(eblerr_ReqFlashEraseStart_fail,"Handling package type: ReqFlashEraseStart failed.")
   ERROR_DEF(eblerr_ReqFlashEraseCheck_fail,"Handling package type: ReqFlashEraseCheck failed.")
   ERROR_DEF(eblerr_ReqFlashWriteBlock_fail,"Handling package type: ReqFlashWriteBlock failed.")
   ERROR_DEF(eblerr_ReqFlashReadBlock_fail,"Handling package type: ReqFlashReadBlock failed.")
   ERROR_DEF(eblerr_ReqFlashSetAddress_fail,"Handling package type: ReqFlashSetAddress failed.")
   ERROR_DEF(eblerr_ReqFlashId_fail,"Handling package type: ReqFlashId failed.")
   ERROR_DEF(eblerr_ReqCfiInfo_1_fail,"Handling package type: ReqCfiInfo_1 failed.")
   ERROR_DEF(eblerr_ReqCfiInfo_2_fail,"Handling package type: ReqCfiInfo_2 failed.")
   ERROR_DEF(eblerr_ReqSecStart_fail,"Handling package type: ReqSecStart failed.")
   ERROR_DEF(eblerr_ReqSecEnd_fail,"Handling package type: ReqSecEnd failed.")
   ERROR_DEF(eblerr_CnfBaudChange_fail,"Handling package type: CnfBaudChange failed.")
   ERROR_DEF(eblerr_rx_pack_crc_fail,"The received package had CRC error.")
   ERROR_DEF(eblerr_hw_struct_no_valid_cs,"The HW struct did not contain a valid CS number.")
   ERROR_DEF(eblerr_erase_addr_outside_physical_range,"Erase address outside phycical flash range.")
   ERROR_DEF(eblerr_can_only_start_erase_when_idle,"Request to start erase but state machine was not idle.")
   ERROR_DEF(eblerr_can_only_start_write_when_idle,"Request to start programming but state machine was not idle.")
   ERROR_DEF(eblerr_can_not_get_cfi_when_not_idle,"Request to get device metrics but state machine was not idle.")
   ERROR_DEF(eblerr_can_not_set_cfi_when_not_idle,"Request to set device metrics but state machine was not idle.")
   ERROR_DEF(eblerr_flash_reset_failed,"Flash device reset failed.")
   ERROR_DEF(eblerr_flash_erase_setup_failed,"Flash device erase setup failed.")
   ERROR_DEF(eblerr_flash_check_busy_failed,"Flash device check busy failed.")
   ERROR_DEF(eblerr_flash_program_failed,"Flash device programming failed.")
   ERROR_DEF(eblerr_flash_calc_checksum_failed,"Flash device CRC calculation failed.")
   ERROR_DEF(eblerr_flash_read_failed,"Flash device read failed.")
   ERROR_DEF(eblerr_flash_init_failed,"Flash device initialization failed.")
   ERROR_DEF(eblerr_intel_efp_setup_failed,"Intel EFP mode setup failed.")
   ERROR_DEF(eblerr_intel_efp_vpp_failed,"Intel EFP voltage check failed.")
   ERROR_DEF(eblerr_intel_efp_program_failed,"Intel EFP mode program failed.")
   ERROR_DEF(eblerr_can_not_start_programm_at_odd_address,"Can not program at odd address.")
   ERROR_DEF(eblerr_amd_device_timeout,"Spansion device timeout.")
   ERROR_DEF(eblerr_amd_buffered_programming_failed,"Spansion buffered programming failed.")
   ERROR_DEF(eblerr_unknown_flash_command_set,"Unknown flash command set.")
   ERROR_DEF(eblerr_upload_ebl_does_not_support_this_primitive,"Primitive not supported by Upload EBL.")
   ERROR_DEF(eblerr_only_capable_to_upload,"Upload EBL error: Storage layer FLASH write attempt.")
   ERROR_DEF(eblerr_writefunc_undef,"Upload EBL error: Undef. write driver function called.")
   ERROR_DEF(eblerr_protocol_crc_error,"A package checksum error was detected by the protocol.")
   ERROR_DEF(eblerr_protocol_marker_error,"Protocol SOT or EOT marker is corrupted.")
   ERROR_DEF(eblerr_can_only_read_when_idle,"Request to read but state machine was not idle.")
   ERROR_DEF(eblerr_unknown_msg_type,"Unknown message type received.")
   ERROR_DEF(eblerr_protocol_comm_error,"Protocol failed in the communication driver call.")
   ERROR_DEF(eblerr_can_only_get_serial_when_idle,"Request to get serial number but state machine was not idle.")
   ERROR_DEF(eblerr_can_only_get_block_boundaries_when_idle,"Request to get block boundaries but state machine was not idle.")
   ERROR_DEF(eblerr_can_only_setup_hw_when_init,"Request to setup hw but state machine was not initialized/idle.")
   ERROR_DEF(eblerr_write_area_in_excl_area,"Request to write area within area occupied by the ROM or internal-RAM.")
   ERROR_DEF(eblerr_ind_error_msg,"Error in EBL.")
   ERROR_DEF(eblerr_setup_nvm,"Error setting NVM options.")
   ERROR_DEF(eblerr_handle_nvm,"Error handling NVM.")
   ERROR_DEF(eblerr_nvm_length,"Length of received NVM option list is too long.")
   ERROR_DEF(eblerr_unknown_nvm_tag,"NVM list tag is unknown.")
   ERROR_DEF(eblerr_ReqOutOfSessionControl_fail,"Handling package type: ReqOutOfSessionControl failed.")
   ERROR_DEF(eblerr_ReqOutOfSessionDataWrite_fail,"Handling package type: ReqOutOfSessionDataWrite failed.")
   ERROR_DEF(eblerr_ReqOutOfSessionDataRead_fail,"Handling package type: ReqOutOfSessionDataRead failed.")
   ERROR_DEF(eblerr_secure_area_full,"Not enough available space in secure area.")
   ERROR_DEF(eblerr_secure_area_cannot_find_certificate,"Cannot find certificate in secure area.")
   ERROR_DEF(eblerr_secure_area_range_exceeded,"The secure area range was exceeded.")
   ERROR_DEF(eblerr_secure_area_buffer_error,"Error occured while updating backup buffers for secure area.")
   ERROR_DEF(eblerr_secure_area_corrupted,"The secure area has been corrupted.")
   ERROR_DEF(eblerr_secure_area_invalid_certificate,"Invalid certificate.")
   ERROR_DEF(eblerr_signature_validation_error,"Signature validation failed.")
   ERROR_DEF(eblerr_xml_failed,"XML handling failed.")
   ERROR_DEF(eblerr_protocol_payload_size_exceed_error,"Payload size exceeds the max size.")
   ERROR_DEF(eblerr_write_area_outside_range,"Request to write area outside the allowed range.")
   ERROR_DEF(eblerr_storage_clock,"storage_clock")
   ERROR_DEF(eblerr_storage_end,"storage_end")
   ERROR_DEF(eblerr_storage_erase,"storage_erase")
   ERROR_DEF(eblerr_storage_get_block_boundaries,"storage_get_block_boundaries")
   ERROR_DEF(eblerr_storage_get_device_metrics,"storage_get_device_metrics")
   ERROR_DEF(eblerr_storage_get_device_serial,"storage_get_device_serial")
   ERROR_DEF(eblerr_storage_hw_setup,"storage_hw_setup")
   ERROR_DEF(eblerr_storage_init,"storage_init")
   ERROR_DEF(eblerr_storage_is_busy,"storage_is_busy")
   ERROR_DEF(eblerr_storage_is_empty,"storage_is_empty")
   ERROR_DEF(eblerr_storage_is_erasing,"storage_is_erasing")
   ERROR_DEF(eblerr_storage_read,"storage_read")
   ERROR_DEF(eblerr_storage_setup_boundary_table,"storage_setup_boundary_table")
   ERROR_DEF(eblerr_storage_set_address,"storage_set_address")
   ERROR_DEF(eblerr_storage_set_device_metrics,"storage_set_device_metrics")
   ERROR_DEF(eblerr_storage_start,"storage_start")
   ERROR_DEF(eblerr_storage_write,"storage_write")
   ERROR_DEF(eblerr_storage_io_ctrl,"storage_io_ctrl")
   ERROR_DEF(eblerr_get_storage_type,"get_storage_type")
   ERROR_DEF(eblerr_erase_all_flash,"erase_all_flash")
   ERROR_DEF(eblerr_partition_not_present,"Partition not present in flash resident partition table.")
   ERROR_DEF(eblerr_max_sec_packs_in_area,"There is not room for all the sec packs in the reserved area.")
   ERROR_DEF(eblerr_last_error,"Dummy")
} cfi_func_enum_type;

#endif
