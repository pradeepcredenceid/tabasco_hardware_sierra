#-------------------------------------------------------------------------------
#
#  $Id: hp.mak,v 1.15 2008/10/22 01:04:11 rfiler Exp $
#
#  Name:
#    hp.mak
#
#  Description:
#    Makefile to build the pkgs/hp package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     hptest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f hp.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings- 
#-------------------------------------------------------------------------------
PKG  = hp
SRCS = hprxtask_sdk.c  hptxtask_sdk.c hprx.c hptx.c hpphtask_sdk.c \
       hploopback.c hputil.c hpfwdownload.c hpairchange.c hpmdmstate.c hpreset.c
OBJS = $(OBJSDIR)/hprxtask_sdk.o $(OBJSDIR)/hptxtask_sdk.o $(OBJSDIR)/hprx.o \
       $(OBJSDIR)/hptx.o $(OBJSDIR)/hputil.o $(OBJSDIR)/hpphtask_sdk.o \
       $(OBJSDIR)/hploopback.o  $(OBJSDIR)/hpfwdownload.o \
       $(OBJSDIR)/hpairchange.o $(OBJSDIR)/hpmdmstate.o $(OBJSDIR)/hpreset.o

HDR = hpidefs.h hpudefs.h hpiproto.h hpuproto.h

SDKOBJ = $(OBJS)

# LIB and OBJ files required by $(PKG) test program
TESTLIBS =   $(SDKLIB) $(CORELIB) 

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: hp.mak,v $
