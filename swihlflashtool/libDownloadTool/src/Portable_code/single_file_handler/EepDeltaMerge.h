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
* Description: Interface for: DWD Static eeprom update ANSI C library functions
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/EepDeltaMerge.h $
*  $CC-Version: .../oint_tools_flashtoole2_01/9 $
*  $Date:       2014-03-13    15:40:47 UTC $
*   Resonsible:
*   Comment:
*
*/

#ifndef EepDeltaMergeH
#define EepDeltaMergeH
/*---------------------------------------------------------------------------*/
#include "OS.h"
/*---------------------------------------------------------------------------*/


extern uint8_t EEP_linearize(uint16_t *eep_orig, uint32_t static_size, uint32_t block_size);
extern uint8_t EEP_linearize_bering(uint16_t *eep_orig, uint32_t static_size, uint32_t block_size);
extern void EEP_Init(void);
extern void EEP_FreeAllMem(void);
extern void EEP_Load_CurrCFG_file(char *CurrCFG_filename);
extern void EEP_Load_NewCFG_file(char *NewCFG_filename);
extern void EEP_Load_DEP_file(char *DEP_filename);
extern void EEP_CnvToBin(uint8_t *BinPtr, uint32_t BinSize);
extern void EEP_CnvFromBin(uint8_t *BinPtr, uint32_t BinSize);


/*---------------------------------------------------------------------------*/
#endif

