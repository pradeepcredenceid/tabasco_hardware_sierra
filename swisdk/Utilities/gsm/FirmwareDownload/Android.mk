#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless Firmware Download application
#
# Based on reference-ril by The Android Open Source Project
# Modified by Sierra Wireless, Inc.
#
# Copyright (C) 2011 Sierra Wireless, Inc.
# Copyright 2006 The Android Open Source Project
#
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

# For executable
# =======================
include $(CLEAR_VARS)

# for ARM printout
LOCAL_CFLAGS += -DARM

LOCAL_C_INCLUDES :=    \
                    $(TOP)/hardware/sierra/swisdk/pkgs/ap
                                        
# Source files for this build
LOCAL_SRC_FILES:=   FirmwareDownload.c

LOCAL_SHARED_LIBRARIES := \
	libcutils libswisdkapi
	
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif

# Disable prelink
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
# Build test executable
LOCAL_MODULE:= swifwdnld
include $(BUILD_EXECUTABLE)

