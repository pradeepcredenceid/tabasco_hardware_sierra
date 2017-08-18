/*  -------------------------------------------------------------------------
    Copyright (C) 2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/inc/strings/psi_strings.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-29    11:20:55 UTC $
    ------------------------------------------------------------------------- 

    This file contains id/error code definitions and their corresponding texts

    DO NOT add anything besides TEXT_DEF statements here

    Add new entries at the end of the list ONLY - DO NOT CHANGE THE ORDER OF THE
    EXISTING CODES (the result will be incompatibilities between Flashtool and
    bootloader).

    ------------------------------------------------------------------------- */

#ifdef TEXT_DEF
  TEXT_DEF(PSIERR_NO_ERROR,"No error.")
  TEXT_DEF(PSIERR_PLATFORM_NOT_INITIALIZED,"Hw not initialized prior to platform setup.")
  TEXT_DEF(PSIERR_READ_ERROR,"Comm read returned an error,")
  TEXT_DEF(PSIERR_UNABLE_TO_BOOT_RAM_PSI,"Unable to boot system when running RAM PSI.")
  TEXT_DEF(PSIERR_INTERFACE_NOT_SUPPORTED,"Interface not supported.")
  TEXT_DEF(PSIERR_INVALID_EBL_LENGTH,"Invalid EBL length.")
  TEXT_DEF(PSIERR_INVALID_EBL_CHECKSUM,"Invalid EBL checksum.")
  TEXT_DEF(PSIERR_FAILS_OPEN_COMM_DRV,"Fails to open comm driver.")
  TEXT_DEF(PSIERR_INVALID_SYSTEM_SIGNATURE,"Invalid system signature.")
  TEXT_DEF(PSIERR_INVALID_SYSTEM_SIGNATURE_LOCATION,"Invalid system signature location.")
  TEXT_DEF(PSIERR_SEC_PACK_IN_EXCEPTION_LOG,"Valid sec pack in exception log.")
  TEXT_DEF(PSIERR_INVALID_EBL_SIGNATURE,"Ebl signature invalid.")
  TEXT_DEF(PSIERR_NO_SECURITY_ON_FUSED_BOARD,"Fused device running a signed PSI without any security.")
  TEXT_DEF(PSIERR_NONCE_VALIDATION_FAILURE,"Failed to validate NONCE.")
  TEXT_DEF(PSIERR_SNUM_VALIDATION_FAILURE,"Failed to validate SNUM.")
  TEXT_DEF(PSIERR_CHIPID_VALIDATION_FAILURE,"Failed to validate SNUM.")
  TEXT_DEF(PSIERR_VERSION_VALIDATION_FAILURE,"Failed to validate version.")
  TEXT_DEF(PSIERR_EBL_DIGEST_VALIDATION_FAILURE,"Failed to validate EBL digest.")
  TEXT_DEF(PSIERR_STACK_DIGEST_VALIDATION_FAILURE,"Failed to validate stack digest.")
  TEXT_DEF(PSIERR_NO_STACK_DIGEST_FAILURE,"Failed to retreive stack digest from manifest.")
  TEXT_DEF(PSIERR_NO_STACK_LENGTH_FAILURE,"Failed to retrieve stack length from manifest.")
  TEXT_DEF(PSIERR_MANIFEST_SIGNATURE_VALIDATION_FAILURE,"Failed to validate manifest signature.")
  TEXT_DEF(PSIERR_MANIFEST_VALIDATION_FAILURE,"Failed to validate manifest.")
  TEXT_DEF(PSIERR_MANIFEST_SIGNATURE_NOT_FOUND,"No signature in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_NO_TICKET_BODY,"No ticket body in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_NO_GPUK,"No gold public key in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_WRONG_ALGORITHM,"Not a SHA1 algorithm specified in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_NO_ALGORITHM,"No algorithm tag in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_SEQUENCE_ERROR,"Sequence error in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_INITIALIZE_ERROR,"Initialisation error in manifest.")
  TEXT_DEF(PSIERR_MANIEST_COULDNT_DECODE_SNUM,"Couldnt decode SNUM tag in manifest.")
  TEXT_DEF(PSIERR_MANIEST_COULDNT_DECODE_PSI_VERSION,"Couldnt decode psi version tag in manifest.")
  TEXT_DEF(PSIERR_MANIEST_COULDNT_DECODE_CHIPID_VERSION,"Couldnt decode chip id tag in manifest.")
  TEXT_DEF(PSIERR_MANIEST_COULDNT_DECODE_NONCE,"Couldnt decode nonce tag in manifest.")
  TEXT_DEF(PSIERR_MANIFEST_INPUT_PARAMETER_NULL,"Input parameter wrong.")
  TEXT_DEF(PSIERR_INVALID_EBL_MANIFEST_LENGTH,"Invalid EBL+Manifest length.")
  TEXT_DEF(PSIERR_RANDOM_NUMBER_GENERATION_FAILURE,"Failed to generate random number.")
  TEXT_DEF(PSIERR_ADD_TLV_RESPONSE_FAILURE,"Failed to add TLV data.")
  TEXT_DEF(PSIERR_FAILS_WRITE_COMM_DRV,"Failed to write data.")
  TEXT_DEF(PSIERR_TRANSMIT_BOOTRESPONSE_FAILURE,"Failed to transmit boot response.")
  TEXT_DEF(PSIERR_INVALID_SLB_SIGNATURE,"SLB signature invalid.")
  TEXT_DEF(PSIERR_INVALID_SLB_LENGTH,"Invalid SLB length.")
  TEXT_DEF(PSIERR_INVALID_SEC_PACK,"Invalid SEC pack.")
  TEXT_DEF(PSIERR_INVALID_PSI_BUILD,"None secure PSI build for secure HW.")
#endif
