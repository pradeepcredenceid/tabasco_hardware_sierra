#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless RIL
#
# Based on reference-ril by The Android Open Source Project
# Modified by Sierra Wireless, Inc.
#
# Copyright (C) 2011 Sierra Wireless, Inc.
# Copyright 2006 The Android Open Source Project

# For shared lib
# =======================
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# SDK  8 is Android 2.2.x - aka FROYO
# SDK  9 is isolated
# SDK 10 is Android 2.3.x - aka GINGERBREAD
# SDK 11 is Android 3.0   - aka HONEYCOMB (we don't support this)
# SDK 12 is Android 3.1   - aka HONEYCOMB (we don't support AT RIL for it)
# SDK 14 is Android 4.0.1 - aka ICE CREAM SANDWICH
# SDK 15 is Android 4.0.x
# SDK 16 is Android 4.1.x - aka JELLY BEAN
# SDK 16 is Android 4.2.x - aka JELLY BEAN
# SDK 19 is Android 4.4	  - aka KitKat
# SDK 21 is Android 5.0   - aka Lollipop
# SDK 22 is Android 5.1   - aka Lollipop
# SDK 23 is Android 6.0   - aka Marshmallow
# SDK 24 is Android 7.0   - aka Nougat
# SDK 25 is Android 7.1   - aka Nougat
SDK_ANDROID_22_23:= 8 10
SDK_ANDROID_31:= 12
SDK_ANDROID_40:= 14 15
SDK_ANDROID_41:= 16
SDK_ANDROID_42:= 17
SDK_ANDROID_44:= 19
SDK_ANDROID_50_51:= 21 22
SDK_ANDROID_60 := 23
SDK_ANDROID_70_71 := 24 25
RIL_VERSION_6:= $(SDK_ANDROID_31) $(SDK_ANDROID_40) $(SDK_ANDROID_41)
RIL_VERSION_7:= $(SDK_ANDROID_42) 
RIL_VERSION_9:= $(SDK_ANDROID_44)
RIL_VERSION_AT_ALL:= $(RIL_VERSION_3) $(SDK_ANDROID_40) $(SDK_ANDROID_41) $(SDK_ANDROID_42) $(SDK_ANDROID_44) $(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71)


# for ARM printout
LOCAL_CFLAGS += -DARM
LOCAL_CFLAGS += -DANDROID_LOGGING
LOCAL_CFLAGS += -DANDROID_NO_STACKDUMP

LOCAL_C_INCLUDES :=    \
                    $(TOP)/hardware/sierra/swisdk/pkgs/ \
                    $(TOP)/hardware/sierra/swicommon/

# Library for swiv2sdk APIs - required by the firmware download app
LOCAL_SRC_FILES:=       \
	os/swi_osapi.c    \
	er/er_api.c   \
	sl/slcommon.c sl/slmanage.c sl/slvsprintf.c sl/slucs2.c \
	pi/pi.c \
	ic/icipc_api.c  \
	ci/cipackage.c ci/ci_api.c   \
	am/am.c am/amadmin.c am/am_api.c am/amcns.c \
	cn/cnpkg_api.c \
	ap/SwiIntCore.c \
	ap/SwiApiCmBasic.c ap/SwiIntUpkCmBasic.c ap/SwiIntPkCmBasic.c \
	ap/SwiApiGsmNetwork.c ap/SwiIntUpkGsmNetwork.c ap/SwiIntPkGsmNetwork.c \
	ap/SwiApiGsmPhonebook.c ap/SwiIntUpkGsmPhonebook.c \
	ap/SwiIntPkGsmPhonebook.c \
	ap/SwiApiGsmBasic.c ap/SwiIntUpkGsmBasic.c \
	ap/SwiApiNotify.c ap/SwiIntDecodeNotify.c \
	ap/SwiIntDecodeNotifyGsm.c ap/SwiIntDecodeNotifyCdma.c \
	ap/SwiApiGsmSim.c ap/SwiIntUpkGsmSim.c ap/SwiIntPkGsmSim.c \
	ap/SwiApiGsmSs.c \
	ap/SwiApiCmUpgrade.c ap/SwiIntUpkCmUpgrade.c ap/SwiIntPkCmUpgrade.c \
	ap/SwiApiCmLbs.c ap/SwiIntUpkCmLbs.c ap/SwiIntPkCmLbs.c\
	ap/SwiApiGsmLbs.c ap/SwiIntUpkGsmLbs.c ap/SwiIntPkGsmLbs.c \
	ap/SwiIntUtil.c \
	ap/SwiApiCdmaBasic.c \
	ap/SwiIntCdma.c \
	ap/SwiApiCdmaIota.c \
	ap/SwiApiCdmaNetwork.c \
	ap/SwiApiCdmaRuim.c \
	ap/SwiApiCdmaSms.c \
	ap/SwiApiCdmaVoice.c \
	ap/SwiIntPkCdmaBasic.c \
	ap/SwiIntPkCdmaInternal.c \
	ap/SwiIntPkCdmaNetwork.c \
	ap/SwiIntPkCdmaRuim.c \
	ap/SwiIntPkCdmaVoice.c \
	ap/SwiIntUpkCdmaBasic.c \
	ap/SwiIntUpkCdmaInternal.c \
	ap/SwiIntUpkCdmaNetwork.c \
	ap/SwiIntUpkCdmaRuim.c \
	ap/SwiApiCdmaLbs.c \
	ap/SwiApiCmDm.c \
	ap/SwiIntPkCdmaLbs.c \
	ap/SwiIntPkCmDm.c \
	ap/SwiIntUpkCdmaLbs.c \
	ap/SwiIntUpkCmDm.c \
	ap/SwiIntUpkCdmaSms.c \
	ap/SwiApiGsmStk.c ap/SwiIntUpkGsmStk.c ap/SwiIntPkGsmStk.c \
	ap/SwiApiGsmVoice.c \
	ap/SwiIntPkGsmVoice.c \
	ap/SwiIntUpkGsmVoice.c 

LOCAL_SHARED_LIBRARIES := \
	libcutils

ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif

# Disable prelink
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
# build shared lib
LOCAL_MODULE:= libswisdkapi
include $(BUILD_SHARED_LIBRARY)

# For executable
# =======================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DARM
LOCAL_CFLAGS += -DANDROID_NO_STACKDUMP
LOCAL_CFLAGS += -DANDROID_LOGGING

LOCAL_C_INCLUDES :=    \
                    $(TOP)/hardware/sierra/swisdk/pkgs/ \
                    $(TOP)/hardware/sierra/swicommon/

# Executable for Firmware Download Daemon - "swisdk"
LOCAL_SRC_FILES:=       \
	os/swi_ossdk.c os/swi_ossdkcheck.c \
	mm/mm.c    \
	dd/dd.c dd/dd_sdk.c \
	er/er_sdk.c   \
	sl/slcommon.c sl/slmanage.c sl/slvsprintf.c sl/slucs2.c \
	ql/ql.c    \
	pi/pi.c \
	dl/dlpackage.c  \
	ic/icipc_sdk.c ic/icsmt_sdk.c ic/icticktask_sdk.c    \
	us/usscan_sdk.c us/usdetect_sdk.c   \
	ci/cipackage.c ci/ci_sdk.c   \
	am/am.c am/amipctask_sdk.c am/amadmin.c am/amcns.c \
	am/amdemux.c \
	ds/dstask_sdk.c ds/dsshell.c    \
	hp/hpairchange.c hp/hpmdmstate.c hp/hprx.c hp/hpfwdownload.c \
	hp/hputil.c \
	hp/hptx.c hp/hploopback.c hp/hprxtask_sdk.c hp/hptxtask_sdk.c \
	hp/hpphtask_sdk.c hp/hpreset.c \
	hd/hdrx.c hd/hdtx.c hd/hduser.c \
	cn/cnrrtask_sdk.c  \
	dr/drtask_sdk.c dr/drshell.c \
	su/su.c    \
	sdk/sdk.c
	
LOCAL_SHARED_LIBRARIES := \
	libcutils

ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif 

# Disable prelink
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= swisdk
include $(BUILD_EXECUTABLE)

