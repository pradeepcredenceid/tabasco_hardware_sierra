/*   ----------------------------------------------------------------------
     Copyright (C) 2014 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     Copyright (C) 2009 - 2010 Infineon Technologies Denmark A/S (IFWD)
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/inc/strings/memtest_strings.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-29    11:20:52 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */
/*
    This file contains id/error code definitions and their corresponding texts

    DO NOT add anything besides TEXT_DEF statements here

    Add new entries at the end of the list ONLY - DO NOT CHANGE THE ORDER OF THE
    EXISTING CODES (the result will be incompatibilities between Flashtool and
    bootloader).

    ------------------------------------------------------------------------- */

#ifdef TEXT_DEF
  TEXT_DEF(E_DEVICE_PRESENT_TEST,"Device presence soldertest")
  TEXT_DEF(E_DATABUS_TEST,"Databus soldertest")
  TEXT_DEF(E_ACCESSWIDTH_TEST,"Byte mask soldertest")
  TEXT_DEF(E_ADDRESSBUS_TEST,"Addressbus soldertest")
  TEXT_DEF(E_ADDRESS_FILL_TEST,"Alternating addressbus stability test")
  TEXT_DEF(E_RANDOM_DATA_TEST,"Random fill stability test")
  TEXT_DEF(E_RMW_XOR_TEST,"Read-XOR-write stability test")
  TEXT_DEF(E_RMW_OR_TEST,	"Read-OR-write stability test")
  TEXT_DEF(E_RMW_AND_TEST,"Read-AND-write stability test")
  TEXT_DEF(E_SOLID_DATA_TEST,"Solid ones and zeroes stability test")
  TEXT_DEF(E_SOLID_DATA_TEST_16,"Solid ones and zeroes 16 bit stability test")
  TEXT_DEF(E_ALTERNAT_SINGLEBIT,"Alternating single bits stability test")
  TEXT_DEF(E_SEQUENTIAL_INCREMENT_TEST,"Sequential increment stability test")
  TEXT_DEF(E_CHECKERBOARD_TEST,"Alternating checkerboard stability test")
#endif

