#-------------------------------------------------------------------------------
#
#  $Id: os.mak,v 1.11 2009/06/01 23:28:59 blee Exp $
#
#  Name:
#    os.mak
#
#  Description:
#    Makefile to build the pkgs/os package 
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
#     make -f os.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted

# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = os
SRCS = swi_osapi.c swi_ossdk.c swi_ossdkcheck.c

OBJS = $(OBJSDIR)/swi_osstubs.o $(OBJSDIR)/swi_osapi.o $(OBJSDIR)/swi_ossdk.o \
       $(OBJSDIR)/swi_ossdkcheck.o

HDRS = swi_ossdk.h swi_osapi.h

APIOBJ   = $(OBJSDIR)/swi_osapi.o 
DEMUXOBJ = $(OBJSDIR)/swi_osapi.o 
SDKOBJ   = $(OBJSDIR)/swi_ossdk.o $(OBJSDIR)/swi_ossdkcheck.o

#
# Kernel 2.4.x uses 
# Extract the 2.4 or 2.6 portion of the kernel name
# Usually 2.4.xx or 2..6.xx-yy-string
#
KERN_VER := $(shell uname -r |  sed 's/^\([0-9].[0-9]\).*/\1/')
ifeq (2.4,$(findstring 2.4,$(KERN_VER)))
 SRCS = swi_osapi.c swi_ossdk.c 
 OBJS = $(OBJSDIR)/swi_osstubs.o $(OBJSDIR)/swi_osapi.o $(OBJSDIR)/swi_ossdk.o \
       $(OBJSDIR)/swi_ossdkcheck2.4.o
 SDKOBJ = $(OBJSDIR)/swi_ossdk.o $(OBJSDIR)/swi_ossdkcheck2.4.o
endif

# LIB and OBJ files required by $(PKG) test program
TESTLIBS =


#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: os.mak,v $
