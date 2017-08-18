#
# This source code is "Not a Contribution" under Apache license
#
# Sierra Wireless GPS Support
#
# Based on Android.mk as presented on Android Forums at the following
# URL: http://source.android.com/porting/gps.html
# Modified by Sierra Wireless, Inc.
#
# Copyright (C) 2011 Sierra Wireless, Inc.
#
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
SDK_ANDROID_60 :=23
SDK_ANDROID_70_71 :=24 25
ALL_SDK_VERSIONS:=$(SDK_ANDROID_22) $(SDK_ANDROID_23_31_40_41_42_44) $(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71)

# Check 1 - it's an error if none of the SDK Versions match the platform
#           this is running on
ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(ALL_SDK_VERSIONS))" ""
    $(error -- SWIGPS: Unsupported Android version)
endif

# Check 2 - see if this is a Froyo build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_22))" ""
    GPS_PLATFORM_SOURCE:= \
        swigps_froyo.c
    GPS_PLATFORM_HEADER:= \
        hardware/libhardware_legacy/include/hardware_legacy
endif

# Check 3 - see if this is a Gingerbread or Honeycomb build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_23_31_40_41_42_44))" ""
    GPS_PLATFORM_SOURCE:= \
        swigps_module.c \
        swigps_gingerplus.c
    GPS_PLATFORM_HEADER:= \
        /hardware/libhardware/include/hardware/
endif

# Check 4 - see if this is a Lollipop or Marshmallow
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60))" ""
    GPS_PLATFORM_SOURCE:= \
        swigps_module.c \
        swigps_gingerplus.c
    GPS_PLATFORM_HEADER:= \
        /hardware/libhardware/include/hardware/
    LOCAL_CFLAGS += -DNEW_USED_IN_FIX_MASK        
endif

# Check 5 - see if this is a Nougat build
ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_70_71))" ""
    GPS_PLATFORM_SOURCE:= \
        swigps_module.c \
        swigps_gingerplus.c
    GPS_PLATFORM_HEADER:= \
        /hardware/libhardware/include/hardware/
    LOCAL_CFLAGS += -DGNSS_SUPPORT        
endif

LOCAL_MODULE_TAGS := optional
# For QMI GPS support
LOCAL_MODULE:= libswigpsqmi

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

LOCAL_SHARED_LIBRARIES := \
    libcutils libutils libnetutils libswims

LOCAL_SRC_FILES := \
    swigps.c \
    swigps_outb.c \
    swigps_common.c \
    swigps_qmi.c \
    $(GPS_PLATFORM_SOURCE)

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
                    $(TOP)/$(GPS_PLATFORM_HEADER) \
                    $(TOP)/hardware/sierra/swiril \
                    $(TOP)/hardware/sierra/swicommon \
                    $(TOP)/hardware/sierra/swims
                    

ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_70_71))" ""                   
    LOCAL_MODULE_RELATIVE_PATH := 
else    
    ifeq ($(TARGET_PRODUCT),hikey)
        LOCAL_MODULE_RELATIVE_PATH := 
    else                    
        LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
    endif    
endif

ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif

ifeq ($(TARGET_PRODUCT),minnow_64p)
    LOCAL_CFLAGS +=-Wno-unused-parameter 
    LOCAL_MODULE_TARGET_ARCH := x86_64 
endif

ifeq ($(TARGET_PRODUCT),android_x86_64)
    LOCAL_CFLAGS +=-Wno-unused-parameter 
    LOCAL_MODULE_TARGET_ARCH := x86_64 
endif

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_C_INCLUDES :=  $(filter-out $(KERNEL_HEADERS), $(LOCAL_C_INCLUDES))
endif

include $(BUILD_SHARED_LIBRARY)



# For AT GPS support. NOTE: The AT RIL is not available for
# Honeycomb releases of Android although this makefile will
# build the AT GPS library for honeycomb. This library should
# not be used on honeycomb for that reason.
include $(CLEAR_VARS)



LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libswigpsat

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

LOCAL_SHARED_LIBRARIES := \
    libcutils libutils libnetutils libswims

LOCAL_SRC_FILES := \
    swigps.c \
    swigps_outb.c \
    swigps_common.c \
    swigps_at.c \
    $(GPS_PLATFORM_SOURCE)

LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \
                    $(TOP)/$(GPS_PLATFORM_HEADER) \
                    $(TOP)/hardware/sierra/swiril \
                    $(TOP)/hardware/sierra/swicommon \
                    $(TOP)/hardware/sierra/swims

ifneq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_70_71))" ""                   
    LOCAL_MODULE_RELATIVE_PATH := 
else    
    ifeq ($(TARGET_PRODUCT),hikey)
        LOCAL_MODULE_RELATIVE_PATH := 
    else                    
        LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
    endif    
endif

ifeq "$(findstring $(PLATFORM_SDK_VERSION),$(SDK_ANDROID_50_51) $(SDK_ANDROID_60) $(SDK_ANDROID_70_71))" ""
LOCAL_LDLIBS += -lpthread
endif

ifeq ($(TARGET_PRODUCT),minnow_64p)
    LOCAL_CFLAGS +=-Wno-unused-parameter 
    LOCAL_MODULE_TARGET_ARCH := x86_64
endif

ifeq ($(TARGET_PRODUCT),android_x86_64)
    LOCAL_CFLAGS +=-Wno-unused-parameter 
    LOCAL_MODULE_TARGET_ARCH := x86_64 
endif

ifeq ($(TARGET_PRODUCT),hikey)
    LOCAL_C_INCLUDES :=  $(filter-out $(KERNEL_HEADERS), $(LOCAL_C_INCLUDES))
endif

include $(BUILD_SHARED_LIBRARY)

