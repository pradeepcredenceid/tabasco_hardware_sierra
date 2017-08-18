/*  -------------------------------------------------------------------------
    Copyright (C) 2013-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/uta_secure_channel_tags.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-23    10:54:39 UTC $
     ---------------------------------------------------------------------- */

#if !defined(uta_secure_channel_tags_h)
#define uta_secure_channel_tags_h

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/* Secure channel group tags */
typedef enum UtaSecChannelGroupTag{
  UTA_SEC_SECURE_CHANNEL_REQ=0x9100,
  UTA_SEC_SECURE_CHANNEL_REPLY=0x9200,
  UTA_SEC_SIGNED_DATA=0x9202
}UtaSecChannelGroupTag;

/* Secure channel element tag */
typedef enum UtaSecChannelTagId{
  UTA_SEC_SIGNATURE=1,
  UTA_SEC_NONCE,
  UTA_SEC_HASH_MD5,
  UTA_SEC_HASH_SHA1,
  UTA_SEC_HASH_SHA256,
  UTA_SEC_REQ_ID,
  UTA_SEC_REQ_DATA,
  UTA_SEC_REPLY_DATA
}UtaSecChannelTagId;

/* SIM validate element tag */
typedef enum UtaSecSimValGroupTag{
  UTA_SEC_SIM_VAL_REQ=0xA100,
  UTA_SEC_SIM_VAL_REPLY=0xA101,
  UTA_SEC_SIM_VAL_FILE=0xA200,
  UTA_SEC_SIM_VAL_FILE_REQ=0xA201,
}UtaSecSimValGroupTag;

typedef enum UtaSecSimValTagId{
  UTA_SEC_SIM_VAL_SIM_IDX,
  UTA_SEC_SIM_VAL_FILE_ID,
  UTA_SEC_SIM_VAL_FILE_DATA,
  UTA_SEC_SIM_VAL_FILE_TYPE,
  UTA_SEC_SIM_VAL_FILE_TYPE_DATA,
  UTA_SEC_SIM_VAL_FILE_PATH,
  UTA_SEC_SIM_VAL_VEDICT
}UtaSecSimValTagId;

/* SIM validate element tag */
typedef enum UtaSecSysVersionGroupTag{
  UTA_SEC_SYS_VERSION_VAL_REQ=0xB100,
  UTA_SEC_SYS_VERSION_VAL_REPLY=0xB101,
}UtaSecSysVersionGroupTag;

typedef enum UtaSecSysVersionTagId{
	UTA_SEC_SYS_VERSION_SW_VERSION,
  UTA_SEC_SYS_VERSION_SW_VERSION_VERDICT
}UtaSecSysVersionTagId;

typedef enum UtaSecSysVersionVerdictTagId{
	UTA_SEC_SYS_VERSION_SW_VERSION_BOOT,
  UTA_SEC_SYS_VERSION_SW_VERSION_NOBOOT,
  UTA_SEC_SYS_VERSION_SW_VERSION_BOOT_RESTRICTED
}UtaSecSysVersionVerdictTagId;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // uta_secure_channel_tags_h
