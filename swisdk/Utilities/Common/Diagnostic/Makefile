#-------------------------------------------------------------------------------
#  $Id: Makefile,v 1.6 2009/06/01 23:35:50 blee Exp $
#
#  Name:           Makefile
#
#  Description:    Makefile for Utilities/gsm/Diagnostic
#
#   Usage:
#          make
#          make clean
#   
#          make CPU=arm9
#          make clean CPU=arm9
#
#          make TECHNOLOGY=ALL
#            This is when both CDMA and UMTS libraries are 
#            required by the application
#
#          Generates SwiDiagnostici386 or 
#          SwiDiagnosticarm9 executables
#          The CPU is stripped from the binary filename when copied to 
#              build/bin/$(CPU)/
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------
include ../../util_rules.mak

CXX = gcc
DBGFLAGS = -g
CCFLAGS = -Wall -I../../../build/include 

LDFLAGS = -static -lrt

#-------------------------------------------------------------------------------
# if CPU is not specified, default to i386
# otherwise, accept only i386 or arm9
#
# For arm9 download ts7800-crosstool-linux-oldabi-0.28rc39.tar.bz2
# from ftp://ftp.embeddedarm.com/ts-arm-sbc/ts-7800-linux/cross-toolchains
# Then uncompress the toolchain and 'sudo' copy /opt folder to /usr/local 
# directory of your PC.
# Tested on Fedora8 and Ubuntu 8.04.
# 
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPU)),)
  CPU = i386
endif
ifeq ($(CPU),i386)
  CXX = gcc
endif
ifeq ($(CPU),arm9)
  CXX=/usr/local/opt/crosstool/arm-linux/gcc-3.3.4-glibc-2.3.2/bin/arm-linux-gcc
endif

#-------------------------------------------------------------------------------
# Default to including only UMTS libraries. In the event that both 
# CDMA and UMTS libraries are compiled in, we include all libraries for 
# linker symbol resolution based on additional command line option.
#
LIBS = -L../../../build/lib/$(CPU) \
	-lswicmbasic -lswigsmbasic -lswigsmnetwork \
	-lswiapidx -lswiapi -lswicore -lpthread

ifeq ($(TECHNOLOGY),ALL)
  LIBS = -L../../../build/lib/$(CPU) \
	-lswicmbasic -lswigsmbasic -lswigsmnetwork \
	-lswicdmabasic -lswicdmainternal -lswicdmanetwork \
	-lswicdmaiota -lswicdmalbs -lswicdmaruim -lswicdmasms \
	-lswicdmavoice -lswicmdm -lswicmlbs \
	-lswiapidx -lswiapi -lswicore -lpthread
endif

ifeq ($(SYMBOLS),ON)
CCFLAGS:=$(CCFLAGS) $(DBGFLAGS)
endif

#-------------------------------------------------------------------------------
# Main target
#-------------------------------------------------------------------------------
TARGET = SwiDiagnostic
SOURCE = Diagnostic.c
INCLUDES = 
OBJECT = $(OBJSDIR)/Diagnostic.o
OBJSDIR = obj$(CPU)

$(TARGET)$(CPU): $(OBJECT) $(INCLUDES)
	${CXX} $(OBJECT) $(LDFLAGS) $(LIBS) -g -o $(TARGET)$(CPU) 
	cp $(TARGET)$(CPU) ../../../build/bin/$(CPU)/$(TARGET)

$(OBJECT): $(SOURCE)
	${CXX} ${CCFLAGS} -c $(SOURCE)
	@if [ -d $(OBJSDIR) ] ; then echo "$(OBJSDIR) exists..." ;\
	else mkdir -p $(OBJSDIR) ;\
	fi;
	mv *.o $(OBJSDIR)
#
#--------------------------------------------------------------------------------
# Clean binaries
#--------------------------------------------------------------------------------
.PHONY: clean
clean:
	@echo "Cleaning files..."    
	@-rm -f $(OBJECT) $(TARGET)$(CPU) 
	@-rm -f $(OBJECT) ../../../build/bin/$(CPU)/$(TARGET)

