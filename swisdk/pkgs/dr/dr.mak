#-------------------------------------------------------------------------------
#
#  $Id: dr.mak,v 1.1 2008/09/30 23:25:21 epasheva Exp $
#
#  Name:
#    dr.mak
#
#  Description:
#    Makefile to build the pkgs/dr package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     drtest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f dr.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings-
#
#-------------------------------------------------------------------------------
PKG  = dr
SRCS = drtask_sdk.c drshell.c  
OBJS = $(OBJSDIR)/drshell.o $(OBJSDIR)/drtask_sdk.o 

HDRS = dridefs.h drudefs.h driproto.h druproto.h

APIOBJ = 
COREOBJ = 
SDKOBJ = $(OBJSDIR)/drtask_sdk.o $(OBJSDIR)/drshell.o

# LIB and OBJ files required by $(PKG) test program
TESTLIBS = $(APILIB) $(CMBASICLIB) $(CORELIB) $(SDKLIB) $(CORELIB) $(SDKLIB)

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: dr.mak,v $
