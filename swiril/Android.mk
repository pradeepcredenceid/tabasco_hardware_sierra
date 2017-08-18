#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless RIL
#
# Based on reference-ril by The Android Open Source Project
# and U300 RIL by ST-Ericsson.
# Modified by Sierra Wireless, Inc.
#
# Copyright (C) 2011 Sierra Wireless, Inc.
# Copyright (C) ST-Ericsson AB 2008-2009
# Copyright 2006 The Android Open Source Project
#
# Based on reference-ril
# Modified for ST-Ericsson U300 modems.
# Author: Christian Bejram <christian.bejram@stericsson.com>
#
# XXX using libutils for simulator build only...
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# SDK  8 is Android 2.2.x - aka FROYO
# SDK  9 is isolated
# SDK 10 is Android 2.3.x - aka GINGERBREAD
# SDK 11 is Android 3.0   - aka HONEYCOMB (we don't support this)
# SDK 12 is Android 3.1
# SDK 14 is Android 4.0.1 - aka ICE CREAM SANDWICH
# SDK 15 is Android 4.0.x
# SDK 16 is Android 4.1.x - aka JELLY BEAN
# SDK 17 is Android 4.2.x - aka JELLY BEAN
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
SDK_ANDROID_70 :=24
SDK_ANDROID_71 :=25
RIL_VERSION_6:= $(SDK_ANDROID_31) $(SDK_ANDROID_40) $(SDK_ANDROID_41)
RIL_VERSION_7:= $(SDK_ANDROID_42) 
RIL_VERSION_9:= $(SDK_ANDROID_44)
ALL_SDK_VERSIONS:= $(SDK_ANDROID_22_23) $(SDK_ANDROID_31) $(SDK_ANDROID_40) $(SDK_ANDROID_41) $(SDK_ANDROID_42) $(SDK_ANDROID_44) $(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70) $(SDK_ANDROID_71)

# Check 1 - it's an error if none of the SDK Versions match the platform
#           this is running on
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(ALL_SDK_VERSIONS))" ""
    $(error -- SWIRIL: Unsupported Android version)
endif

# Check 2 - see if this is a Froyo and Gingerbraed build
ifneq "$(findstring $(PLATFORM_SDK_VERSION), $(SDK_ANDROID_22_23))" ""
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi.c \
        swiril_network_qmi.c
endif

# Check 3 - see if this is a Honeycomb MR1 or Ice Cream Sandwich build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(RIL_VERSION_6))" ""
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi_honeycomb.c \
        swiril_network_qmi_honeycomb.c
LOCAL_CFLAGS := -DAUDIO_PROVISIONING -DECC_LIST_INIT #-DHW_TI_OMAP #-DNO_LCD_OFF_IND
        $(info " ----------------- This is ICS QMI RIL with Audio Provisioning & ECC List support ----------------- ")
endif

# Check 4 - see if this is a Jelly Bean buld
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(RIL_VERSION_7))" ""
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi_honeycomb.c \
        swiril_network_qmi_honeycomb.c
endif

# Check 5 - see if this is a Ice Cream Sandwitch build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_40))" ""
LOCAL_CFLAGS += -DSWI_RIL_VERSION_3X_ICS
endif

# Check 6 - see if this is a KitKat build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_44))" ""
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi_honeycomb.c \
        swiril_network_qmi_honeycomb.c
LOCAL_CFLAGS := -DNO_LCD_OFF_IND                
    $(info " ----------------- This is KitKat QMI RIL support data only ----------------- ")
endif

# Check 7 - see if this is a Lollipop build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51))" ""
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64 -DSIERRA_ANDROID_RIL
endif
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi_honeycomb.c \
        swiril_network_qmi_honeycomb.c
LOCAL_CFLAGS += -DNO_LCD_OFF_IND                
    $(info " ----------------- This is Lollipop QMI RIL support data only ----------------- ")
endif

# Check 8 - see if this is a Marshmallow build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_60))" ""
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64 -DSIERRA_ANDROID_RIL
endif

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_CLANG_CFLAGS += -Wno-error=tautological-pointer-compare \
                          -Wno-error=pointer-bool-conversion
else ifeq ($(TARGET_PRODUCT),android_x86_64)
    LOCAL_CFLAGS += -DANDROID_6PLUS
else ifeq ($(TARGET_PRODUCT),android_x86)
    LOCAL_CFLAGS += -DANDROID_6PLUS
endif    
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi_honeycomb.c \
        swiril_network_qmi_honeycomb.c
    $(info " ----------------- This is Marshmallow QMI RIL support data only ----------------- ")
    $(info " ----------------- QMI notification control by GobiNet driver    ----------------- ")
endif

# Check 9 - see if this is a Nougat build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_70) $(SDK_ANDROID_71))" ""
ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64 -DSIERRA_ANDROID_RIL
endif

LOCAL_CLANG_CFLAGS += -Wno-error=tautological-pointer-compare \
                      -Wno-error=pointer-bool-conversion
 
    LOCAL_PLATFORM_SOURCE:= \
        swiril_pdp_qmi_honeycomb.c \
        swiril_network_qmi_honeycomb.c
    $(info " ----------------- This is Nougat QMI RIL support data only ----------------- ")
    $(info " ----------------- QMI notification control by GobiNet driver    ----------------- ")
endif

# For swapping value of RIL_RAT_LTE and RIL_RAT_EHRPF for Android 7 and further
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge $(SDK_ANDROID_70) && echo true), true)
LOCAL_CFLAGS += -DRIL_RAT_SWAPPED_TO_ORIG
endif

LOCAL_SRC_FILES:= \
    swiril_main.c \
    ../swicommon/swiril_network.c \
    swiril_requestdatahandler.c \
    ../swicommon/swiril_sim.c \
    ../swicommon/swiril_misc.c \
    ../swicommon/swiril_cache.c \
    at_channel.c \
    ../swicommon/at_misc.c \
    ../swicommon/at_tok.c \
    ../swicommon/swiril_oem.c \
    swiril_main_qmi.c \
    swiril_sim_qmi.c \
    swiril_imsa_qmi.c \
    swiril_misc_qmi.c \
    swiril_device_qmi.c \
    ../swicommon/swiril_gps.c \
    ../swicommon/swiril_gps_inb.c \
    ../swigps/swigps_common.c \
    swiril_gps_qmi.c \
    swiril_gps_qmi_loc.c \
    swiril_oem_device.c \
    swiril_oem_qmi.c \
    swiril_sms_qmi.c \
    swiril_network_common_qmi.c \
    swiril_pdp_common_qmi.c \
    ../swicommon/fcp_parser.c \
    swiril_callhandling.c \
    swiril_services.c \
    $(LOCAL_PLATFORM_SOURCE)

LOCAL_SHARED_LIBRARIES := \
    libcutils libutils libril libnetutils libswims libswiqmiapi

# For asprinf
LOCAL_CFLAGS += -D_GNU_SOURCE -Wno-unused-parameter

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
    $(TOP)/hardware/ril/libril/ \
    $(TOP)/hardware/sierra/swigps/ \
    $(TOP)/hardware/sierra/swicommon

# Include the swiqmi3 include directories for RIL
LOCAL_C_INCLUDES += \
    $(TOP)/hardware/sierra/swiqmi3/ \
    $(TOP)/hardware/sierra/swiqmi3/am    \
    $(TOP)/hardware/sierra/swiqmi3/pi    \
    $(TOP)/hardware/sierra/swiqmi3/qa    \
    $(TOP)/hardware/sierra/swiqmi3/qa/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/cat/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/cbk/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/dcs/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/dms/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/fms/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/nas/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/pds/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/rms/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/sms/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/wds/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/swioma/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/omadm/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/sar/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/voice/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/uim/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/swi/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qm    \
    $(TOP)/hardware/sierra/swiqmi3/os    \
    $(TOP)/hardware/sierra/swiqmi3/sl    \
    $(TOP)/hardware/sierra/swiqmi3/qa/audio/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/swiaudio/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/qos/inc    \
    $(TOP)/hardware/sierra/swiqmi3/qa/ims/inc   \
    $(TOP)/hardware/sierra/swiqmi3/qa/imsa/inc   \
    $(TOP)/hardware/sierra/swiqmi3/qa/loc/inc   \
    $(TOP)/hardware/sierra/swiqmi3/qa/swiaudio/inc

# To include swims atlast so that SwiDataTypes.h is picked from SLQS 
LOCAL_C_INCLUDES += \
    $(TOP)/hardware/sierra/swims

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_C_INCLUDES :=  $(filter-out $(KERNEL_HEADERS), $(LOCAL_C_INCLUDES))
endif     
    
# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

# Build shared library
LOCAL_SHARED_LIBRARIES += \
    libcutils libutils
    
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70) $(SDK_ANDROID_71))" ""
LOCAL_LDLIBS += -lpthread
endif
 
LOCAL_CFLAGS += -DRIL_SHLIB

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libsierra-ril
include $(BUILD_SHARED_LIBRARY)
