#-------------------------------------------------------------------------------
#
#  $Id: pi.mak,v 1.6 2008/11/11 01:44:24 epasheva Exp $
#
#  Name:
#    pi.mak
#
#  Description:
#    Makefile to build the pkgs/pi package 
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#     pitest        - builds the test program if available
#
#   The above targets can be made with the following command:
#
#     make -f pi.mak [target]
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = pi
SRCS = pi.c 
OBJS = $(OBJSDIR)/pi.o 
HDRS = piudefs.h piuproto.h

COREOBJ = $(OBJS)
SDKOBJ = $(OBJS)

TESTLIBS = $(CORELIB)

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: pi.mak,v $
