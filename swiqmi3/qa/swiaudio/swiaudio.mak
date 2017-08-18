#-------------------------------------------------------------------------------
#
#  Name:
#    swiaudio.mak
#
#  Description:
#    Makefile to build the pkgs/qa/swiaudio package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
# Copyright (c) 2013 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
SRCS_SWIAUDIO = swiaudio/src/qaGobiApiSwiAudio.c \
                swiaudio/src/qaAudioSLQSGetM2MAudioProfile.c \
                swiaudio/src/qaAudioSLQSSetM2MAudioProfile.c \
                swiaudio/src/qaAudioSLQSGetM2MAudioVolume.c \
                swiaudio/src/qaAudioSLQSSetM2MAudioVolume.c \
                swiaudio/src/qaAudioSLQSSetM2MAudioAVCFG.c \
                swiaudio/src/qaAudioSLQSSetM2MAudioLPBK.c \
                swiaudio/src/qaAudioSLQSSetM2MAudioNVDef.c \
                swiaudio/src/qaAudioSLQSGetM2MSpkrGain.c \
                swiaudio/src/qaAudioSLQSSetM2MSpkrGain.c \
                swiaudio/src/qaAudioSLQSGetM2MAVMute.c \
                swiaudio/src/qaAudioSLQSSetM2MAVMute.c

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
SWIAUDIOOBJ = $(OBJSDIR)/qaGobiApiSwiAudio.o \
              $(OBJSDIR)/qaAudioSLQSGetM2MAudioProfile.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MAudioProfile.o \
              $(OBJSDIR)/qaAudioSLQSGetM2MAudioVolume.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MAudioVolume.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MAudioAVCFG.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MAudioLPBK.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MAudioNVDef.o \
              $(OBJSDIR)/qaAudioSLQSGetM2MSpkrGain.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MSpkrGain.o \
              $(OBJSDIR)/qaAudioSLQSGetM2MAVMute.o \
              $(OBJSDIR)/qaAudioSLQSSetM2MAVMute.o