#-------------------------------------------------------------------------------
#
#  $Id: ds.mak,v 1.3 2009/04/17 19:21:32 bsiu Exp $
#
#  Name:
#    ds.mak
#
#  Description:
#    Makefile to build the pkgs/ds package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     amtest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f ds.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings-
#
#-------------------------------------------------------------------------------
PKG  = ds
SRCS = dstask_sdk.c dsshell.c  ds_app.c
OBJS = $(OBJSDIR)/dstask_sdk.o $(OBJSDIR)/dsshell.o $(OBJSDIR)/ds_app.o

HDRS = dsidefs.h dsudefs.h dsiproto.h dsuproto.h

APIOBJ = 
DEMUXOBJ  = $(OBJSDIR)/ds_app.o
COREOBJ = 
SDKOBJ = $(OBJSDIR)/dstask_sdk.o $(OBJSDIR)/dsshell.o

# LIB and OBJ files required by $(PKG) test program
TESTLIBS = $(APILIB) $(CMBASICLIB) $(CORELIB) $(SDKLIB) $(CORELIB) $(SDKLIB)

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: ds.mak,v $
