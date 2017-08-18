#-------------------------------------------------------------------------------
#
#  $Id: us.mak,v 1.7 2008/11/11 01:45:52 epasheva Exp $
#
#  Name:
#    us.mak
#
#  Description:
#    Makefile to build the pkgs/us package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     qltest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f us.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted

# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = us
SRCS = usscan_sdk.c usdetect_sdk.c
OBJS = $(OBJSDIR)/usscan_sdk.o $(OBJSDIR)/usdetect_sdk.o
HDRS = usudefs.h usidefs.h usuproto.h usiproto.h

SDKOBJ = $(OBJS)

# LIB and OBJ files required by $(PKG) test program
TESTLIBS =  $(SDKLIB) $(CORELIB)
#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: us.mak,v $
