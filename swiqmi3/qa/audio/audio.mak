#-------------------------------------------------------------------------------
#
#  Name:
#    audio.mak
#
#  Description:
#    Makefile to build the pkgs/qa/audio package
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
SRCS_AUDIO = audio/src/qaGobiApiAudio.c \
             audio/src/qaAudioSLQSGetAudioProfile.c \
             audio/src/qaAudioSLQSSetAudioProfile.c \
             audio/src/qaAudioSLQSGetAudioPathConfig.c \
             audio/src/qaAudioSLQSSetAudioPathConfig.c \
             audio/src/qaAudioSLQSGetAudioVolTLBConfig.c \
             audio/src/qaAudioSLQSSetAudioVolTLBConfig.c
#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
AUDIOOBJ = $(OBJSDIR)/qaGobiApiAudio.o \
           $(OBJSDIR)/qaAudioSLQSGetAudioProfile.o \
           $(OBJSDIR)/qaAudioSLQSSetAudioProfile.o \
           $(OBJSDIR)/qaAudioSLQSGetAudioPathConfig.o \
           $(OBJSDIR)/qaAudioSLQSSetAudioPathConfig.o \
           $(OBJSDIR)/qaAudioSLQSGetAudioVolTLBConfig.o \
           $(OBJSDIR)/qaAudioSLQSSetAudioVolTLBConfig.o