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
SDK_ANDROID_70_71 := 24 25
ALL_SDK_VERSIONS:=$(SDK_ANDROID_22) $(SDK_ANDROID_23_31_40_41_42_44) $(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71)

ifeq ($(TARGET_ARCH), $(filter $(TARGET_ARCH), x86_64 arm64))
    LOCAL_CFLAGS += -DCPU64 -Wno-unused-parameter
endif

LOCAL_SRC_FILES:=       \
    swims_ossdkscanmain.c \
    swims_ossdk.c         \
    swims_ossdkcheck.c    
    
LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
    $(TOP)/hardware/sierra/swicommon

LOCAL_SHARED_LIBRARIES := \
    libcutils
    
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_C_INCLUDES :=  $(filter-out $(KERNEL_HEADERS), $(LOCAL_C_INCLUDES))
endif

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false


LOCAL_MODULE_TAGS := optional
#build shared lib
LOCAL_MODULE:= libswims
include $(BUILD_SHARED_LIBRARY)



