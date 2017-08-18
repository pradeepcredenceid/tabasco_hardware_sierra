/*   ----------------------------------------------------------------------
     Copyright (C) 2013-2014 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     Copyright (C) 1999 Infineon Technologies AG
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/inc/EBL2_nvm.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-29    11:20:33 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */

#ifndef EBL2_NVM_H
#define EBL2_NVM_H

#include <stdint.h>

// Flashtool should be independent of the NVM_STRUCT addr in target memory
#define FLASH_BASE          0x0
#define SYSTEM_IMAGE_OFFSET 0x0

/* Defines used by the NVM functions */
#define NVM_STRUCT_START_ADDR  (FLASH_BASE+SYSTEM_IMAGE_OFFSET+0x00000414) /* Address of the NVM struct in the FLS download image                      */
#define NVM_STRUCT_OFFSET       0x00040414UL             /* Offset to the NVM struct in the FLS download image            */
#define NVM_STRUCT_OFFSET_NAND  0x00040C14UL          /* Offset to the NVM struct in the FLS download image  for NAND          */
#define NVM_STRUCT_MAX_LENGTH   0xA00UL                 /* Length to read to local buffer - must hold the whole NVM table  */
#define NVM_STRUCT_MAGIC        0x5D60B742UL            /* Magic word must match the first word read at the given offset            */
#define NVM_STRUCT_SIG_OFFSET   0x700UL                /* Signature Offset - from start of NVM table - Used in secure mode*/

#define NVM_STRUCT_MAGIC_OFS           0    /* Offset into the first header to read the magic word */
#define NVM_STRUCT_VER_OFS             1    /* Offset into the first header to read the version */
#define NVM_STRUCT_REV_OFS             2    /* Offset into the first header to read the revision */

#define NVM_STRUCT_HEAD_LEN_OFS        0    /* Offset into the item header to read the length word                      */
#define NVM_STRUCT_HEAD_TAG_OFS        1    /* Offset into the item header to read the tag word                         */

#define NVM_STRUCT_ADDR                2    /* Offert into the item payload to read the address to be erased or written */
#define NVM_STRUCT_NOF_BYTES           3    /* Offert into the item payload to read the number of bytes to be erased or written */
#define NVM_STRUCT_ERASE_MODE          4    /* Offert into the item payload to read the erase mode                      */
#define NVM_STRUCT_WRITE_DATA          4    /* Offset into the item payload where the data to be written is placed      */

#define NVM_STRUCT_V2_MEM_TYPE         10   /* Offset into the item payload to read the memory type */
#define NVM_STRUCT_V2_OFFSET           11   /* Offset into the item payload to read the address (NOR) or partition offset (NAND) */
#define NVM_STRUCT_V2_PARTITION_ID     12   /* Offset into the item payload to read the partition ID (NAND) */
#define NVM_STRUCT_V2_NOF_BYTES        13   /* Offset into the item payload to read the number of bytes to erase or write */
#define NVM_STRUCT_V2_ERASE_MODE       14   /* Offert into the item payload to read the erase mode                      */
#define NVM_STRUCT_V2_WRITE_DATA       14   /* Offset into the item payload where the data to be written is placed      */

/* Offsets for the Follow write structure */
#define NVM_STRUCT_FW_FOLLOW_WRITE_TAG 2    /* Offert into the item payload to read the follow write tag                      */
#define NVM_STRUCT_FW_ADDR             3    /* Offert into the item payload to read the address to be written */
#define NVM_STRUCT_FW_NOF_BYTES        4    /* Offert into the item payload to read the number of bytes to be written */
#define NVM_STRUCT_FW_WRITE_DATA       5    /* Offset into the item payload where the data to be written is placed      */

#define NVM_STRUCT_V2_FW_FOLLOW_WRITE_TAG 11    /* Offert into the item payload to read the follow write tag                      */
#define NVM_STRUCT_V2_FW_ADDR             12    /* Offert into the item payload to read the address to be written */
#define NVM_STRUCT_V2_FW_PARTITION_ID     13    /* Offset into the item payload to read the partition ID (NAND) */
#define NVM_STRUCT_V2_FW_NOF_BYTES        14    /* Offert into the item payload to read the number of bytes to be written */
#define NVM_STRUCT_V2_FW_WRITE_DATA       15    /* Offset into the item payload where the data to be written is placed      */

#define NVM_STRUCT_OFFSET_VAL          2    /* Offset into the item payload to read the address offset to be applied */

#define NVM_STRUCT_CONDITION           2
#define NVM_STRUCT_COND_VAR            3
#define NVM_STRUCT_COND_VAL            4

#define NVM_STRUCT_HEAD_WORDS          3    /* Number of U32 words for magic_word and revisions.                        */
#define NVM_STRUCT_MAGIC_SIZE NVM_STRUCT_HEAD_WORDS*sizeof(uint32_t) /* Size of the first header part */
#define NVM_OPTION_DISABLE 0x00             /* Options disabled .. default if not set */
#define NVM_OPTION_ENABLE  0x01             /* Options enabled .. by the user         */

#define NVM_STRUCT_PAYLOAD_ERASE_PARAM 2    /* Offert into the item payload to read the erase parameter for erase items */
#define NVM_STRUCT_NO_TAG              0    /* Value to signal that a tag is not present           */

#define NVM_NOF_OPTIONS                256  /* Max number of options we can handle                                      */

/*************************  LOCAL TYPE DEFINITIONS  *************************/

typedef enum /* Must have size U32 */
{
   NVM_TAG_ERASE_CAL              = 0x00000001 /**< Identifies the version 1 Calibration data area items.  */
  ,NVM_TAG_ERASE_FIX              = 0x00000002 /**< Identifies the version 1 Fixed data area items.  */
  ,NVM_TAG_ERASE_DYN              = 0x00000003 /**< Identifies the version 1 Dynamic data area items.  */
  ,NVM_TAG_STARTUP_MODE_TEST      = 0x00000004 /**< Identifies the version 1 Set test start mode items.  */
  ,NVM_TAG_STARTUP_MODE_NORMAL    = 0x00000005 /**< Identifies the version 1 Set normal start mode items.  */
  ,NVM_TAG_TERMINATOR             = 0x00000006 /**< NVM structure version 1 terminator	*/
  ,NVM_TAG_ERASE_CAL_V2           = 0x00000007 /**< Identifies the version 2 Calibration data area items.  */
  ,NVM_TAG_ERASE_FIX_V2           = 0x00000008 /**< Identifies the version 2 Fixed data area items.  */
  ,NVM_TAG_ERASE_DYN_V2           = 0x00000009 /**< Identifies the version 2 Dynamic data area items.  */
  ,NVM_TAG_ERASE_MISC_V2          = 0x0000000A /**< Identifies the version 2 Misc data area items.	*/
  ,NVM_TAG_STARTUP_MODE_TEST_V2   = 0x0000000B /**< Identifies the version 2 Set test start mode items.  */
  ,NVM_TAG_STARTUP_MODE_NORMAL_V2 = 0x0000000C /**< Identifies the version 2 Set normal start mode items.  */
  ,NVM_TAG_TERMINATOR_V2          = 0x0000000D /**< NVM structure version 2 terminator	*/
  ,NVM_TAG_FOLLOW_WRITE           = 0x0000000E /**< Identifies the version 1 follow-write items */
  ,NVM_TAG_FOLLOW_WRITE_V2        = 0x0000000F /**< Identifies the version 2 follow-write items */
  ,NVM_TAG_CONDITIONAL            = 0x00000010 /**< Identifies the version 1 condition items */
  ,NVM_TAG_OFFSET                 = 0x00000011 /**< Identifies the version 1 set offset items */
  ,NVM_TAG_BLTDO                  = 0x00000012 /**< Identifies the version 1 Battery-Less Test and Development Operation items */
  ,NVM_TAG_BLTDO_V2               = 0x00000013 /**< Identifies the version 2 Battery-Less Test and Development Operation items */
  ,NVM_TAG_SET_U32_LGT            = 0x0FFFFFFF /**< Force enum to 32 bit size.	*/
} T_NVM_TAG;


typedef enum /* Must have size U32 */
{
   NVM_ERASE_NEVER       = 0x00000001
  ,NVM_ERASE_ALWAYS      = 0x00000002
  ,NVM_ERASE_OPTIONAL    = 0x00000003
  ,NVM_ERASE_SET_U32_LGT = 0x0FFFFFFF// Used to set length of enum
} T_NVM_ERASE_MODE;

typedef enum /* Size U32 */
{
  NVM_CONDITION_END_IF                  = 0x00000000
  ,NVM_CONDITION_ELSE                   = 0x00000001
  ,NVM_CONDITION_IF_EQ                  = 0x00000002
  ,NVM_CONDITION_OR_IF_EQ               = 0x00000003
  ,NVM_CONDITION_AND_IF_EQ              = 0x00000004
  ,NVM_CONDITION_IF_NOT_EQ              = 0x00000005
  ,NVM_CONDITION_OR_IF_NOT_EQ           = 0x00000006
  ,NVM_CONDITION_AND_IF_NOT_EQ          = 0x00000007
  ,T_NVM_CONDITION_TYPE_SET_U32_LGT     = 0x0FFFFFFF //Force enum to U32
} T_NVM_CONDITION_TYPE;

typedef enum /* Size U32 */
{
  NVM_VALUE_FLASH_SIZE                  = 0x00000000
  ,NVM_VALUE_FLASH_ID                   = 0x00000001
  ,T_NVM_VALUE_TYPE_SET_U32_LGT         = 0x0FFFFFFF //Force enum to U32
} T_NVM_VALUE_TYPE;

#endif
