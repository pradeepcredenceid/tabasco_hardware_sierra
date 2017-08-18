/**
 *       Copyright (C) 2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   Layout of spare area in NAND flash
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/inc/nand_spare_area_internal.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/1 $
 *  $Date:       2014-03-13    15:39:18 UTC $
 *   Responsible: gjoerup
 *   Comment:
 *     Checkin performed by OptiCM
 */

#ifndef NAND_SPARE_AREA_INTERNAL_H
#define NAND_SPARE_AREA_INTERNAL_H

#define MAX_PARTITIONS 28

#define ERR_FREE_BLOCK_ID(ID1,ID2) (ID1|ID2)

// ------------------ PARTITION ID SPECIFICATION -------------------

#define  NAND_ID_PSI_IMAGE           0x00 /* partition ID for PSI code image           */
#define  NAND_ID_CODE_IMAGE          0x01 /* partition ID for main code image          */
#define  NAND_ID_ROOT_DISC           0x02 /* partition ID for root disc (RW)           */
#define  NAND_ID_CUSTOMIZATION_DISC  0x03 /* partition ID for customization disc (RO)  */
#define  NAND_ID_USER_DISC           0x04 /* partition ID for User Disc (RW - USBMS)   */
#define  NAND_ID_USER_DISC_EXTEND1   0x05 /* Extention to NAND_ID_USER_DISC        */
#define  NAND_ID_STA_SDS_IMAGE       0x06 /* partition ID for Static Simple Data Storage disc */
#define  NAND_ID_DYN_SDS_IMAGE       0x07 /* partition ID for Dynamic Simple Data Storage disc */
#define  NAND_ID_EXCEPTION_LOG_IMAGE 0x08 /* partition ID for exception log area */
#define  NAND_ID_FTL_STARTUP_DISC    0x09 /* partition ID for Flash Translation Layer Startup area */
#define  NAND_ID_SECURITY            0x0A /* partition ID for Security data and code */
#define  NAND_ID_DSP_IMAGE           0x0B /* partition ID for Aeneas firm ware */
#define  NAND_ID_DEBUG_DATA          0x0C /* partition ID for Debug data */
#define  NAND_ID_STARTUP_IMAGE       0x0D /* partition ID for Startup Image. */
#define  NAND_ID_PARTITION_TABLE     0x0E /* partition ID for the partition table */
#define  NAND_ID_CUSTOMIZATION_DATA  0x0F /* partition ID for RAW customization data */
#define  NAND_ID_SLB_IMAGE           0x10 /* partition ID for SLB code image */
#define  NAND_ID_UA_IMAGE            0x11 /* partition ID for UA partition */
#define  NAND_ID_CALIB_BACKUP_IMAGE  0x12 /* partition ID for holding calib backup (if not present STA_SDS_IMAGE will hold it */
#define  NAND_ID_BAD_BLOCK_TABLE     0x13 /* partition ID for bad block table partition */
#define  NAND_ID_GSM_FW_IMAGE        0x14 /* partition ID for GSM firmware */
#define  NAND_ID_CDS_IMAGE           0x15 /* partition ID for core dump image */
#define  NAND_ID_NVM_STATIC_CALIB    0x16 /* partition ID for static calib data in NVM */
#define  NAND_ID_NVM_STATIC_FIX      0x17 /* partition ID for static fix data in NVM */
#define  NAND_ID_NVM_DYNAMIC         0x18 /* partition ID for dynamic data in NVM */
#define  NAND_ID_USBRO               0x19 /* partition ID for RO USB disc */
#define  NAND_ID_PAGING_IMAGE        0x1A /* partition ID for paging image */
#define  NAND_ID_CERTIFICATE_DATA    0x1B /* partition ID for certificate data. */
#define  NAND_ID_LTE                 0x1C /* partition ID for LTE */
#define  NAND_ID_CA5_MULTICORE       0x1D /* block ID for Multicore system SW */
#define  NAND_ID_UA_BUFFER           0x1E /* block ID for UA data */
#define  NAND_ID_NVM_BACKUP          0x1F /*block ID for NVM backup*/

#define  NAND_ID_USER_DISC_EXTEND2   0x20
#define  NAND_ID_USER_DISC_EXTEND3   0x21
#define  NAND_ID_USER_DISC_EXTEND4   0x22
#define  NAND_ID_USER_DISC_EXTEND5   0x23
#define  NAND_ID_ROOT_DISC_EXTEND1   0x24
#define  NAND_ID_ROOT_DISC_EXTEND2   0x25
#define  NAND_ID_ROOT_DISC_EXTEND3   0x26
#define  NAND_ID_ROOT_DISC_EXTEND4   0x27
#define  NAND_ID_ROOT_DISC_EXTEND5   0x28
#define  NAND_ID_CUSTOMIZATION_DISC_EXTEND1   0x29
#define  NAND_ID_CUSTOMIZATION_DISC_EXTEND2   0x2A
#define  NAND_ID_CUSTOMIZATION_DISC_EXTEND3   0x2B
#define  NAND_ID_CUSTOMIZATION_DISC_EXTEND4   0x2C
#define  NAND_ID_CUSTOMIZATION_DISC_EXTEND5   0x2D

/* One more cust disk */
#define  NAND_ID_CUSTOMIZATION_DISC_TWO   0x2E

#define NAND_ID_CODE_IMAGE_2         0x30 /* block ID for second code partition - for download recovery */
#define NAND_ID_DSP_IMAGE_2          0x31 /* block ID for second AENEAS image – for download recovery */
#define NAND_ID_LTE_2                0x32 /* block ID for second LTE image – for download recovery */


//--------------LINUX / ANDROID -----------
#define  NAND_ID_VLX                 0x40
#define  NAND_ID_LINUX_KERNEL        0x41
#define  NAND_ID_INITRD              0x42
#define  NAND_ID_INITRD_RECOVERY     0x43
#define  NAND_ID_SYSTEM              0x44 /**< Android system filesystem */
#define  NAND_ID_USERDATA            0x45 /**< Android userdata filesystem */
#define  NAND_ID_CACHE               0x46 /**< Android cache filesystem */
#define  NAND_ID_BOOT_IMG            0x47 /**< Android packed kernel+initrd image */
#define  NAND_ID_RECOVERY_IMG        0x48 /**< Android packed kernel+initrd image for recovery mode */
#define  NAND_ID_RECOVERY_IMG_MIRROR 0x49 /**< Android packed kernel+initrd image for recovery mode - mirror */
#define  NAND_ID_BCB                 0x4A /**< Android bootloader control block for recovery */
#define  NAND_ID_KDUMP_IMG           0x4B /**< Linux packed kernel + initrd for kdump mode */
#define  NAND_ID_LINUX_NVM_FS        0x4C /**< Linux NVM filesystem */
#define  NAND_ID_PRIMARY_GPT         0x4E /**< partition ID for partition for the primary GPT and MBR */
#define  NAND_ID_SECONDARY_GPT       0x4F /**< partition ID for partition for the secondary GPT */

#define NAND_ID_CUST_NVM_PLAIN       0x50 /* block ID for customer NVM partition */
#define NAND_ID_CUST_NVM_PROTECT     0x51 /* block ID for customer NVM partition */
#define NAND_ID_UI_DATA              0x52 /* block ID for customer UI data */

//--------------Firmware switching -----------
#define  NAND_ID_CARRIER_IMAGE_1   0x61 /*block ID for multi-carrier customized code image ,used in fw switching solution*/
#define  NAND_ID_CARRIER_IMAGE_2   0x62
#define  NAND_ID_CARRIER_IMAGE_3   0x63
#define  NAND_ID_CARRIER_IMAGE_4   0x64
#define  NAND_ID_CARRIER_IMAGE_5   0x65
#define  NAND_ID_CARRIER_IMAGE_6   0x66
#define  NAND_ID_CARRIER_IMAGE_7   0x67
#define  NAND_ID_CARRIER_IMAGE_8   0x68
#define  NAND_ID_CARRIER_IMAGE_9   0x69
#define  NAND_ID_CARRIER_IMAGE_10  0x6A

#define  NAND_ID_RAW_NAND_IMAGE      0xE0 /* block ID used for RAW NAND image up- and download */
#define  NAND_ID_PREFLASH            0xE1 /* block ID used if preflash is required.            */

#define  NAND_ID_EMPTY               0xFF /* block ID for an empty block */

//-------------------- IMAGE TYPE SPECIFICATION ----------------------

#define  NAND_IT_FAT_IMAGE           0x01 /* Type Fatfile System  (FTuL image)                    (FTL & BBR) */
#define  NAND_IT_YAFFS_IMAGE         0x02 /* Type Yet Another Flash File System (Not supported)   (unused!!)  */
#define  NAND_IT_JFFS_IMAGE          0x03 /* Type Journaling Flash File System (Not supported)    (unused!!)  */
#define  NAND_IT_FFS_IMAGE           0x04 /* Type Flash File System (Not supported)               (unused!!)  */
#define  NAND_IT_CRAMFS_IMAGE        0x05 /* Type CRAM File System (Not supported)                (unused!!)  */
#define  NAND_IT_RAW_IMAGE           0x06 /* Type Code Image (Non FTL image)                      (neither)   */
#define  NAND_IT_SDS_IMAGE           0x07 /* Type Simple Data Storage Image (FTuL image)          (FTL & BBR) */
#define  NAND_IT_FTL_IMAGE           0x08 /* Type Flash Translation Layer Image (FTuL image)      (FTL & BBR) */
#define  NAND_IT_BBR_IMAGE           0x09 /* Type Bad Block Reallocation Layer Image (BBR image)  (BBR)       */
#define  NAND_IT_BOOTUP_IMAGE        0x0A /* Type SLB or PSI image (Non FTL image)                (neither)   */

//-------------------- OPTIONS SPECIFICATION ------------------

#define  NAND_OPT_VERIFICATION_REQUIRED     0x0001 /* Verification of the image contents and signature required at startup */
#define  NAND_OPT_READONLYIMAGE             0x0002 /* This image has to be protected with FCDP at startup time             */
#define  NAND_OPT_COPYTORAM                 0x0004 /* This image has to be copied in to RAM at startup time by SLB         */
#define  NAND_OPT_WIPE_OUT_DATA             0x0008 /* Data in this image has to be wiped-out when deleting                 */
#define  NAND_OPT_LOADMAP                   0x0010 /* Loadmap has to be located as the first in this image                 */
#define  NAND_OPT_SHORT_TIMEOUT             0x0020 /* Specify that only short timeout for POW for this partition           */
#define  NAND_OPT_PAGING_DATA               0x0040 /* This partition contains data object for paging                       */
#define  NAND_OPT_RO_BIT_CORRECTION_OK      0x0080 /* This partitions access right can be changed temporary to correct bit error */
#define  NAND_OPT_ACCEPTABLE_BIT_ERRORS     0x0700 /* Use 3 bits to determine number of acceptable bit errors. 0 = 0 bit, 1 = 1/8 * ECC_level, 2 = 2/8 * ECC_level, etc */
#define  NAND_OPT_TEST_ACCEPTED             0x0800 /* Specify that testsuite may modify contents on this partition. */
#define  NAND_OPT_ALLOW_UTA_BLOCK_IF_ACCESS 0x1000 /* Specify that this partition may be accessed from UTA flash block interface (FOTA access)*/
#define  NAND_OPT_ALLOW_UTA_BYTE_IF_ACCESS  0x2000 /* Specify that this partition may be accessed from UTA flash byte interface (Normal access)*/
#define  NAND_OPT_SECTOR_BASED_PT           0x4000 /* The block numbers in the partition entry is based on 512B sectors, not the actual block size*/
#define  NAND_OPT_PRESERVE_PARTITION        0x8000 /* On subsequent downloads or erases this partition will be preserved. */

/* The 3rd Least-Significant-Byte in Options field is used for FS-specific-options.
 * These options are relevant only for FS-partitions. These options are used to fill
 * Capabilities structure while registering FS-partitions to FS-devices layer.
 * Refer to \storage\inc\fs_devices_internal.h for the detaileed explanation on
 * individual options. */

// Leaving 2nd LSB for future-use by FTL-options. Ideally first 2 bytes for
// FTL-options and last 2 bytes for FS-options in PE.Options field.

#define  NAND_OPT_FS_DEVICE_ACCESS_INTERNAL          0x10000 /* */
#define  NAND_OPT_FS_DEVICE_ACCESS_EXTERNAL          0x20000 /* */
#define  NAND_OPT_FS_DEVICE_ACCESS_BOTH              0x30000 /* */
#define  NAND_OPT_FS_DEVICE_PLP_DISABLE              0x40000 /* */
#define  NAND_OPT_FS_DEVICE_PLP_RUN_CHECK_DISK       0x80000 /* */
#define  NAND_OPT_FS_DEVICE_PLP_ENABLE              0x100000 /* */
// Explicit 'NOT a System-desk' option to maintain backwards-compatibility. If
// we use 0x0 for NOT_SYSTEM_DISK then all the partitions in older PTs become
// non-system-disks.
#define  NAND_OPT_FS_DEVICE_SYSTEM_DISK             0x200000 /* */
#define  NAND_OPT_FS_DEVICE_NOT_SYSTEM_DISK         0x400000 /* */
#define  NAND_OPT_FS_PARTITION_REGISTER             0x800000 /* Specify Partition has to be registred as filesystem. */
#define  NAND_OPT_FS_DEVICE_ALLOCATION_NORMAL      0x8000000 /* */
#define  NAND_OPT_FS_DEVICE_ALLOCATION_SCATTER     0x4000000 /* */

#define  NAND_OPT_SECURE_TRIM                      0x1000000 /**< (FTLE) This partition must be trimmed using secure trim if supported by driver */
#define  NAND_OPT_SECURE_ERASE                     0x2000000 /**< (FTLE) This partition must be erased using secure erase if supported by driver */


#define  NAND_ALIGNMENT_MASK                      0xF0000000 /* Alignment bit mask - 2^"BIT MASK" e.g. 4K alignment => BIT MASK == 12 */

//--------------------- PATITION TABLE STRUCTURE SPECIFICATION ------

typedef struct
{
   uint32_t ID;                         //ID of partition which is specified in this entry.
   uint32_t StartBlock;                 //Physical start address of region, in which the partition is located.
   uint32_t EndBlock;                   //Physical end address of region, in which the partition is located.
   uint32_t MaxBlocksInSubPartition;    //Number of blocks in partition.
   uint32_t StartReservoirBlock;        //Physical start address of the reservoir that the partition is using.
   uint32_t EndReservoirBlock;          //Physiacl end address of the reservoir that the partition is using.
   uint32_t ImageType;                  //Type specification of the partition
   uint32_t Options;                    //Specific options for this image
   uint32_t Reserved;                   //FOTA virtual address.
}PartitionEntryStructType;

//NOTE: if MaxBlocksInSubPartition is 0x0, the Partition entry does not specify any partition.
typedef struct
{
   uint32_t                 PartitionMarker;/* Magic number to identify possible valid data*/
   PartitionEntryStructType Partition[MAX_PARTITIONS];
   uint32_t                 Reserved[3];
   //uint32_t                 PartitionCheckSum;
}PartitionStructType;

typedef struct
{
   PartitionStructType partition_struct;
   uint32_t            adress[MAX_PARTITIONS];
} PartitionTableBufferType;

typedef struct
{
  uint32_t start_addr;
  uint32_t total_length;
  uint32_t used_length;
  uint32_t flags;
} FTL_LoadMapType;

typedef struct
{
  uint32_t                 Reserved[780/4];
  PartitionTableBufferType PT;
  uint32_t                 LoadMagic;
  FTL_LoadMapType          LoadMap[8];
} FTL_SecPackType;

#define SECPACK_SIZE 2048   /* Size of the Sec Pack. ... please keep updated. */

#define  NAND_NO_OF_MECC_BYTES  4  /* Number of main ECC bytes in spare area    */
#define  NAND_NO_OF_SECC_BYTES  2  /* Number of spare ECC bytes in spare area   */

typedef struct {
  uint8_t MECC[NAND_NO_OF_MECC_BYTES];  /* Main ECC                                           */
  uint8_t BB16;                         /* Bad Block byte for 16 bit flash types              */
  uint8_t BB;                           /* Bad Block byte for 8 and 16 bit flash types        */
  uint8_t SN;                           /* Last two bits in each niple: Sequence Number of logical page */
                                        /* First two bits in each nipple: WriteGroup identification  (zero means that this virtual page has been written alone   */
                                        /* Specifies last virtual page in physical page this virtual page has been written together with. */
  uint8_t BI2;                          /* Block Identification for image type                */
  uint8_t LSN_EN;                       /* 4-bit container for LSN and 4-bit container for EN */
  uint8_t BI1;                          /* Block Identification for image type                */
  uint8_t EN[2];                        /* Erase Number of block                              */
  uint8_t LSN[2];                       /* Logic Sector Number of page                        */
  uint8_t SECC[NAND_NO_OF_SECC_BYTES];  /* Spare ECC                                          */
} T_NAND_SPARE_AREA_LAYOUT;


typedef struct {
  uint8_t                   DATA[512];
  T_NAND_SPARE_AREA_LAYOUT  NAND_SPARE_AREA;
} T_NAND_PAGE_LAYOUT;

//-------------- Partition configuration -------------------------- //
#if (defined(NAND_PRESENT) || defined(NANDDFMC_PRESENT))

#define MAC_PRESERVING 0
#define MAC_INCLUDING 1
#define MAC_NOSPECIFICATION 0xFFFF

typedef struct {
  uint16_t Specification;             /* Specification type
                                         0 = preserving - do not erase the following partitions.
                                         1 = including  - do only flash the following partitions
                                      */
  uint8_t PartitionBit[32];           /* Holds one bit for each partition - 1 if selected */
} T_MAC_PARTITION_CONFIG;

#endif

#endif /* NAND_SPARE_AREA_INTERNAL_H */
