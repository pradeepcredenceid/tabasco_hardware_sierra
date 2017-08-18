#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless RIL
#
# Based on reference-ril by The Android Open Source Project
# Modified by Sierra Wireless, Inc.
#
# Copyright (C) 2012 Sierra Wireless, Inc.
# Copyright 2006 The Android Open Source Project

# For shared lib
# =======================
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#
# Use SDK Version to check for which set of source and header files to
# build. One SDK version (so far) represents all variants of a given 
# release. Therefore, if 2.2.x contained more than one value for 'x'
# then only one SDK version check is necessary. We do this even though
# the SDK and GPS aren't formally related
#
# SDK  8 is Android 2.2.x - aka FROYO
# SDK  9 is isolated
# SDK 10 is Android 2.3.x - aka GINGERBREAD
# SDK 11 is Android 3.0   - aka HONEYCOMB (we don't support this)
# SDK 12 is Android 3.1
# SDK 14 is Android 4.0.1 - aka ICE CREAM SANDWICH
# SDK 15 is Android 4.0.x
# SDK 16 is Android 4.1.x - aka JELLY BEAN
# SDK 17 is Android 4.2.x - aka JELLY BEAN
# SDK 19 is Android 4.4   - aka KitKat
# SDK 21 is Android 5.0   - aka Lollipop
# SDK 22 is Android 5.1   - aka Lollipop
# SDK 23 is Android 6.0   - aka Marshmallow
# SDK 24 is Android 7.0   - aka Nougat
# SDK 25 is Android 7.1   - aka Nougat
SDK_ANDROID_22:= 8
SDK_ANDROID_23_31_40_41_42_44:= 10 11 12 14 15 16 17 19
SDK_ANDROID_50_51:= 21 22
SDK_ANDROID_60 := 23
SDK_ANDROID_70 := 24
SDK_ANDROID_70_71 := 24 25
ALL_SDK_VERSIONS:=$(SDK_ANDROID_22) $(SDK_ANDROID_23_31_40_41_42_44) $(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71)

LOCAL_CFLAGS += -DSLQS
# for ARM printout
LOCAL_CFLAGS += -DARM
LOCAL_CFLAGS += -DANDROID_LOGGING
LOCAL_CFLAGS += -DANDROID_NO_STACKDUMP
LOCAL_CFLAGS += -DSIERRA_ANDROID_RIL

ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64
endif

LOCAL_CLANG_CFLAGS += -Wno-error=tautological-pointer-compare \
                      -Wno-error=pointer-bool-conversion

# Default location for slqssdk as a string value.
LOCAL_CFLAGS += -DSDK_EXE='"/system/bin/slqssdk"'

LOCAL_C_INCLUDES :=    \
                    $(TOP)/hardware/sierra/swiqmi3/    \
                    $(TOP)/hardware/sierra/swiqmi3/am    \
                    $(TOP)/hardware/sierra/swiqmi3/pi    \
                    $(TOP)/hardware/sierra/swiqmi3/qa  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/test/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/dms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/wds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/nas/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/sms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/fms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/pds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cat/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/rms/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/dcs/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/swioma/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/omadm/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/sar/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/voice/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/uim/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/swi/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/dms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/wds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/nas/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/wms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/fms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/pds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/cat/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/dcs/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/swioma/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/omadm/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/voice/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/uim/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qm    \
                    $(TOP)/hardware/sierra/swiqmi3/os    \
                    $(TOP)/hardware/sierra/swiqmi3/sl    \
                    $(TOP)/hardware/sierra/swiril    \
                    $(TOP)/hardware/sierra/swicommon    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/audio/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/qos/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/ims/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/imsa/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/ims/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/imsa/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/qos/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/swiaudio/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/loc/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/loc/inc    \

# This library for SWIQMIAPI 
LOCAL_SRC_FILES:=       \
	os/swi_osapi.c    \
	er/er_api.c   \
	sl/slcommon.c sl/slmanage.c sl/slvsprintf.c sl/slucs2.c \
	pi/piLe.c pi/piBe.c    \
	ic/icipc_api.c    \
	ci/cipackage.c ci/ci_api.c   \
	qm/qmcommon.c qm/qm_api.c qm/qmdcscommon.c   \
	qm/qmfmscommon.c \
	am/am.c am/amqmi.c am/am_api.c \
	qa/src/qaQmiBasic.c \


# Append the qa source files for each of the services
include $(LOCAL_PATH)/qa/dms/dms.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_DMS)}

include $(LOCAL_PATH)/qa/wds/wds.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_WDS)}

include $(LOCAL_PATH)/qa/cbk/cbk.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_CBK)}

include $(LOCAL_PATH)/qa/nas/nas.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_NAS)}

include $(LOCAL_PATH)/qa/sms/sms.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_SMS)}

include $(LOCAL_PATH)/qa/fms/fms.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_FMS)}

include $(LOCAL_PATH)/qa/pds/pds.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_PDS)}

include $(LOCAL_PATH)/qa/cat/cat.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_CAT)}

include $(LOCAL_PATH)/qa/rms/rms.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_RMS)}

include $(LOCAL_PATH)/qa/dcs/dcs.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_DCS)}

include $(LOCAL_PATH)/qa/swioma/swiomadms.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_SWIOMADMS)}

include $(LOCAL_PATH)/qa/omadm/omadm.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_OMADM)}

include $(LOCAL_PATH)/qa/sar/sar.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_SAR)}

include $(LOCAL_PATH)/qa/voice/voice.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_VOICE)}

include $(LOCAL_PATH)/qa/uim/uim.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_UIM)}

include $(LOCAL_PATH)/qa/swi/swi.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_SWI)}

include $(LOCAL_PATH)/qa/audio/audio.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_AUDIO)}

include $(LOCAL_PATH)/qa/qos/qos.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_QOS)}

include $(LOCAL_PATH)/qa/ims/ims.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_IMS)}

include $(LOCAL_PATH)/qa/imsa/imsa.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_IMSA)}

include $(LOCAL_PATH)/qa/swiaudio/swiaudio.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_SWIAUDIO)}

include $(LOCAL_PATH)/qa/loc/loc.mak
LOCAL_SRC_FILES += ${addprefix qa/,$(SRCS_LOC)}

LOCAL_SHARED_LIBRARIES := \
    libcutils

# see if this is a Lollipop build
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif

# Disable prelink
LOCAL_PRELINK_MODULE := false


LOCAL_MODULE_TAGS := optional
#build shared lib
LOCAL_MODULE:= libswiqmiapi
include $(BUILD_SHARED_LIBRARY)

# For executable
# =======================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DSLQS
LOCAL_CFLAGS += -DANDROID_NO_STACKDUMP
LOCAL_CFLAGS += -DANDROID_LOGGING

ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64
endif

LOCAL_C_INCLUDES :=    \
                    $(TOP)/hardware/sierra/swiqmi3/    \
                    $(TOP)/hardware/sierra/swiqmi3/am    \
                    $(TOP)/hardware/sierra/swiqmi3/pi    \
                    $(TOP)/hardware/sierra/swiqmi3/qm    \
                    $(TOP)/hardware/sierra/swiqmi3/os    \
                    $(TOP)/hardware/sierra/swiqmi3/sl	\
                    $(TOP)/hardware/sierra/swiril \
                    $(TOP)/hardware/sierra/swicommon

# This executable for SWIQMISDK
LOCAL_SRC_FILES:=       \
	os/swi_ossdk.c os/swi_ossdkcheck.c os/swi_osapi.c  \
	mm/mm.c    \
	er/er_sdk.c   \
	sl/slcommon.c sl/slmanage.c sl/slvsprintf.c sl/slucs2.c \
	ql/ql.c    \
	pi/piLe.c pi/piBe.c    \
	dl/dlpackage.c  \
	ic/icipc_sdk.c ic/icsmt_sdk.c ic/icticktask_sdk.c    \
	us/usscan_sdk.c \
	ci/cipackage.c ci/ci_sdk.c   \
	qm/qmtask_sdk.c qm/qmcommon.c qm/qmdcs.c qm/qmdcscommon.c \
	qm/qmfmscommon.c qm/qmfms.c qm/qmqmisvc.c qm/qmparser.c \
	am/am.c am/amqmi.c am/amipctask_sdk.c am/amsdk.c am/amtimeout_sdk.c \
	ds/dstask_sdk.c ds/dsshell.c    \
	hd/hdrx.c hd/hdtx.c hd/hduser.c \
	im/imssdp_dload.c im/imssdp_done.c im/imssdp_donefinal.c  \
	im/imssdp_error.c im/imssdp_hello.c im/imssdp_image_preference.c  \
	im/imssdp_open_unframed.c im/imssdp_unframed_write.c im/imssdp.c  \
	im/imapi.c im/imcommon.c im/imssdp_dload_to_sdp.c im/imtask_sdk.c \
	im/imuser.c im/imutils.c im/imtaskgobi.c im/imssdpwrapper.c im/firehose.c \
	sm/sm.c  \
	su/su.c    \
	sdk/sdk.c
	
LOCAL_SHARED_LIBRARIES := \
	libcutils

# see if this is a Lollipop build
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif 

# Disable prelink
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= slqssdk
include $(BUILD_EXECUTABLE)

# For executable
# =======================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DSLQS
# for ARM printout
LOCAL_CFLAGS += -DARM

ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64
endif

LOCAL_CLANG_CFLAGS += -Wno-error=tautological-pointer-compare \
                      -Wno-error=pointer-bool-conversion

LOCAL_C_INCLUDES :=    \
                    $(TOP)/hardware/sierra/swiqmi3/qm    \
                    $(TOP)/hardware/sierra/swiqmi3/qa    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/wds/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/dms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/nas/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/sms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/fms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/pds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cat/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/rms/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/dcs/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/swioma/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/omadm/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/sar/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/voice/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/uim/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/swi/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/test/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/dms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/wds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/nas/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/wms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/fms/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/pds/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/cat/inc  \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/dcs/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/swioma/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/omadm/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/voice/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/uim/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/sl    \
                    $(TOP)/hardware/sierra/swiril    \
                    $(TOP)/hardware/sierra/swicommon    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/audio/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/qos/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/ims/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/imsa/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/ims/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/imsa/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/qos/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/swiaudio/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/loc/inc    \
                    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/loc/inc    \

# qa test code
LOCAL_SRC_FILES:=       \
	qa/test/src/qaGobiApiWdsTest.c qa/test/src/qaGobiApiDmsTest.c qa/test/src/qaGobiApiNasTest.c \
	qa/test/src/qaGobiApiCbkTest.c qa/test/src/qaGobiApiDcsTest.c qa/test/src/qaGobiApiCatTest.c \
	qa/test/src/qaGobiApiPdsTest.c qa/test/src/qaGobiApiRmsTest.c qa/test/src/qaGobiApiFmsTest.c \
	qa/test/src/qaGobiApiSmsTest.c qa/test/src/qaGobiApiSwiOmadmsTest.c qa/test/src/qaGobiApiOmaDmTest.c \
	qa/test/src/qaGobiApiSarTest.c qa/test/src/qaGobiApiVoiceTest.c qa/test/src/qaGobiApiUimTest.c \
	qa/test/src/qaGobiApiQosTest.c qa/test/src/qaGobiApiAudioTest.c qa/test/src/qaGobiApiImsTest.c \
	qa/test/src/qaGobiApiImsaTest.c qa/test/src/qaGobiApiSwiAudioTest.c qa/test/src/qaGobiApiLocTest.c \
	qa/src/qatestroutines.c qa/src/qatest.c

LOCAL_SHARED_LIBRARIES := \
	libcutils libswiqmiapi
	
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif 

# Disable prelink
LOCAL_PRELINK_MODULE := false


LOCAL_MODULE_TAGS := optional
# Build test executable
LOCAL_MODULE:= swiqmi_test
include $(BUILD_EXECUTABLE)

