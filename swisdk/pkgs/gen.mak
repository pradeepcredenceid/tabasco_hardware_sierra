#-------------------------------------------------------------------------------
#
#  $Id: gen.mak,v 1.19 2010/06/10 00:28:14 epasheva Exp $
#
#  Name:
#    gen.mak
#
#  Description:
#    Generic Makefile to build the pkgs/{package}  
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default). Test programs are
#                     also built when present
#     clean         - delete object directory and image files
#
#   This file is include with the {package}.mak files using
#     
#     include ../gen.mak
#
#   If GDB is to be used, then run the make at the top level with SYMBOLS=ON
#   and this will cause the compiler to create symbolic information in the 
#   object files
#
# Copyright (c) 2008,2009 by Sierra Wireless, Incorporated. All Rights Reserved
#-------------------------------------------------------------------------------
#.SUFFIXES:

#-------------------------------------------------------------------------------
# Global Settings
#-------------------------------------------------------------------------------
CFLAGS     = -c -MMD 
INCLUDE    = -I.. $(APDX_INCLUDE) $(APUI_INCLUDE) -I/usr/local/include
LDFLAGS    = -static -lrt -lpthread 

 

DEPSDIR    = ./dep
INSTALLDIR = 
DBGFLAGS=-g
#-------------------------------------------------------------------------------
# In order to support Kernel 2.4, we use an additional set of usb libraries 
# from libusb_0.1.2 which must be built separately. When built, the following 
# additional directories and library are needed for building the SDK. 
# This is not required in kernel 2.6.
#-------------------------------------------------------------------------------
KERN_VER := $(shell uname -r |  sed 's/^\([0-9].[0-9]\).*/\1/')
ifeq (2.4,$(findstring 2.4,$(KERN_VER)))
 EXTRA_KERN24_LDFLAGS = -L/usr/local/lib -lusb
endif

#-------------------------------------------------------------------------------
# if CPU is not specified, default to i386
# otherwise, accept only i386 or arm9
#
# For arm9 download ts7800-crosstool-linux-oldabi-0.28rc39.tar.bz2
# from ftp://ftp.embeddedarm.com/ts-arm-sbc/ts-7800-linux/cross-toolchains
# Then uncompress the toolchain and 'sudo' copy /opt folder to /usr/local 
# directory of your PC.
# Tested on Ubuntu 8.04 only.
# 
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPU)),)
  CPU = i386
endif

ifeq ($(CPU),arm9)
  CROSS_COMPILE = /usr/local/opt/crosstool/arm-linux/gcc-3.3.4-glibc-2.3.2/bin/arm-linux-
endif

# $(CROSS_COMPILE) is empty if not defined
CC := $(CROSS_COMPILE)gcc -Wall
AR := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib

#-------------------------------------------------------------------------------
# Set Radio Technology flag for compiler. Available flags are:
# ALL - both CDMA and UMTS libraries are and must be included in linker
# CDMA - CDMA libraries only
# UMTS - UMTS libraries only
#
# Default to ALL, if not specifed in make command line
#
TECHFLAG=-D ALL

ifeq ($(TECHNOLOGY),ALL)
TECHFLAG=-D ALL
endif
ifeq ($(TECHNOLOGY),CDMA)
TECHFLAG=-D CDMA
endif
ifeq ($(TECHNOLOGY),UMTS)
TECHFLAG=-D UMTS
endif

#-------------------------------------------------------------------------------
# DEBUG ON means the compiler should build for possible GDB debugging sessions
# 
# Default is for no debugging which considerably reduces the size of the final
# image, especially when preparing a release for distribution
#
ifeq ($(SYMBOLS),ON)
DBGFLAGS := -g
CFLAGS := $(CFLAGS) $(DBGFLAGS)
endif

#-------------------------------------------------------------------------------
# Variable definitions
#-------------------------------------------------------------------------------

CORELIBDIR      = ../../build/lib/$(CPU)
APILIBDIR      := ../../build/lib/$(CPU)
APIDXLIBDIR    := ../../build/lib/$(CPU)
APIUILIBDIR    := ../../build/lib/$(CPU)
SDKLIBDIR       = ../sdk/obj$(CPU)

OBJSDIR         = obj$(CPU)

APILIB          = $(APILIBDIR)/libswiapi.a
APIDXLIB        = $(APIDXLIBDIR)/libswiapidx.a
APIUILIB        = $(APIUILIBDIR)/libswiapiui.a
CORELIB         = $(CORELIBDIR)/libswicore.a
SDKLIB          = $(SDKLIBDIR)/libsdk.a

#
# Functional groups
#
CMBASICLIB      = $(CORELIBDIR)/libswicmbasic.a
GSMBASICLIB     = $(CORELIBDIR)/libswigsmbasic.a
GSMNETWORKLIB   = $(CORELIBDIR)/libswigsmnetwork.a
GSMSIMLIB       = $(CORELIBDIR)/libswigsmsim.a
GSMPHBKLIB      = $(CORELIBDIR)/libswigsmphonebook.a
GSMVOICELIB     = $(CORELIBDIR)/libswigsmvoice.a
GSMSTKLIB       = $(CORELIBDIR)/libswigsmstk.a
CMLBSLIB        = $(CORELIBDIR)/libswicmlbs.a

CDMABASICLIB    = $(CORELIBDIR)/libswicdmabasic.a
CDMALBSLIB      = $(CORELIBDIR)/libswicdmalbs.a
CMDMLIB         = $(CORELIBDIR)/libswicmdm.a
CDMAINTERNALLIB = $(CORELIBDIR)/libswicdmainternal.a
CDMAIOTALIB     = $(CORELIBDIR)/libswicdmaiota.a
CDMANETWORKLIB  = $(CORELIBDIR)/libswicdmanetwork.a
CDMARUIMLIB     = $(CORELIBDIR)/libswicdmaruim.a
CDMASMSLIB      = $(CORELIBDIR)/libswicdmasms.a
CDMAVOICELIB    = $(CORELIBDIR)/libswicdmavoice.a

# -------------------------------
# Build test program only if present
#--------------------------------
TESTSRC = $(PKG)test.c
TESTOBJ = $(OBJSDIR)/$(PKG)test.o
have_testsrc := $(wildcard $(TESTSRC))
ifeq ($(strip $(have_testsrc)),)
else 
ifeq ($(strip $(have_testsrc)),sdktest.c)
TESTEXE = $(PKG)$(CPU)
else
ifeq ($(strip $(have_testsrc)),apuitest.c)
TESTEXE = $(PKG)$(CPU)
else
TESTEXE = $(PKG)test$(CPU)
endif
endif
endif
# -------------------------------
# Build GSM test program only if present
#--------------------------------
GSMTESTSRC = $(PKG)gsmtest.c
GSMTESTOBJ = $(OBJSDIR)/$(PKG)gsmtest.o
have_gsmtestsrc := $(wildcard $(GSMTESTSRC))
ifeq ($(strip $(have_gsmtestsrc)),)
else
GSMTESTEXE = $(PKG)gsmtest$(CPU)
endif
# -------------------------------
# Build CDMA test program only if present
#--------------------------------
CDMATESTSRC = $(PKG)cdmatest.c
CDMATESTOBJ = $(OBJSDIR)/$(PKG)cdmatest.o
have_cdmatestsrc := $(wildcard $(CDMATESTSRC))
ifeq ($(strip $(have_cdmatestsrc)),)
else
CDMATESTEXE = $(PKG)cdmatest$(CPU)
endif
# -------------------------------
# Build SDK test program only if present
#--------------------------------
SDKTESTSRC = $(PKG)testsdk.c
SDKTESTOBJ = $(OBJSDIR)/$(PKG)testsdk.o
have_sdktestsrc := $(wildcard $(SDKTESTSRC))
ifeq ($(strip $(have_sdktestsrc)),)
else
SDKTESTEXE = $(PKG)testsdk$(CPU)
endif
# -------------------------------
# Build API test program only if present
#--------------------------------
APITESTSRC = $(PKG)testapi.c
APITESTOBJ = $(OBJSDIR)/$(PKG)testapi.o
have_apitestsrc := $(wildcard $(APITESTSRC))
ifeq ($(strip $(have_apitestsrc)),)
else
APITESTEXE = $(PKG)testapi$(CPU)
endif
#
# extract current directory name for special processing of some files
#
CURRENT_DIR = $(notdir $(PWD))
#
#-------------------------------------------------------------------------------
# Default Target
#-------------------------------------------------------------------------------
all:	$(OBJS) $(CMNOBJ) $(APIOBJ) $(DEMUXOBJ) $(APIUIOBJ) $(SDKOBJ) $(COREOBJ) \
$(APILIB) $(APIDXLIB) $(APIUILIB) $(CORELIB) $(SDKLIB)  \
$(CMBASICLIB) $(GSMBASICLIB) $(GSMNETWORKLIB) $(GSMSIMLIB) $(GSMVOICELIB)\
$(GSMPHBKLIB) $(GSMSTKLIB) \
$(CMLBSLIB) $(CDMAGENERALLIB) $(TESTEXE) $(SDKTESTEXE) $(APITESTEXE) \
$(CDMABASICLIB) \
$(CDMALBSLIB) \
$(CMDMLIB) \
$(CDMAINTERNALLIB) \
$(CDMAIOTALIB) \
$(CDMANETWORKLIB) \
$(CDMARUIMLIB) \
$(CDMASMSLIB) \
$(CDMAVOICELIB)


$(OBJS): $(SRCS) $(HDRS) 
	@echo $(INCLUDE) $(LDFLAGS)
	@echo "============================" 
	@echo "--- Building  $(PKG) for $(CPU)..."
	@echo "----------------------------" 	
	@if [ -d $(OBJSDIR) ] ; then echo "$(OBJDIR) exists..." ;\
	else mkdir -p $(OBJSDIR) ;\
	fi;
	$(CC) $(CFLAGS) $(TECHFLAG) $(INCLUDE) $(SRCS)
	mv *.o $(OBJSDIR)
	@if [ -d $(DEPSDIR) ] ; then echo "$(DEPSDIR) exists..." ;\
	else mkdir -p $(DEPSDIR) ;\
	fi;
	mv *.d $(DEPSDIR)
		
$(APILIB): $(APIOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(APIOBJ) to $(APILIB)..."
	@echo "--------------------------------------------" 	
	@if [ -d $(APILIBDIR) ] ; then echo "$(APILIBDIR) exists..." ;\
	else mkdir -p $(APILIBDIR) ;\
	fi;
	$(AR) rvu $(APILIB) $(APIOBJ)
	$(RANLIB) $(APILIB)
	
$(APIDXLIB): $(DEMUXOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(DEMUXOBJ) to $(APIDXLIB)..."
	@echo "--------------------------------------------" 	
	@if [ -d $(APIDXLIBDIR) ] ; then echo "$(APIDXLIBDIR) exists..." ;\
	else mkdir -p $(APIDXLIBDIR) ;\
	fi;
	$(AR) rvu $(APIDXLIB) $(DEMUXOBJ)
	$(RANLIB) $(APIDXLIB)

$(APIUILIB): $(APIUIOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(APIUIOBJ) to $(APIUILIB)..."
	@echo "--------------------------------------------" 	
	@if [ -d $(APIUILIBDIR) ] ; then echo "$(APIUILIBDIR) exists..." ;\
	else mkdir -p $(APIUILIBDIR) ;\
	fi;
	$(AR) rvu $(APIUILIB) $(APIUIOBJ)
	$(RANLIB) $(APIUILIB)
	
$(CORELIB): $(COREOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(COREOBJ) to $(CORELIB)..."
	@echo "--------------------------------------------" 	
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CORELIB) $(COREOBJ)
	$(RANLIB) $(CORELIB)
	
$(SDKLIB): $(SDKOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(SDKOBJ) to $(SDKLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(SDKLIBDIR) ] ; then echo "$(SDKLIBDIR) exists..." ;\
	else mkdir -p $(SDKLIBDIR) ;\
	fi;
	$(AR) rvu $(SDKLIB) $(SDKOBJ)
	$(RANLIB) $(SDKLIB)
	

$(CMBASICLIB): $(CMBASICOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CMBASICOBJ) to $(CMBASICLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CMBASICLIB) $(CMBASICOBJ)
	$(RANLIB) $(CMBASICLIB)

$(GSMBASICLIB): $(GSMBASICOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(GSMBASICOBJ) to $(GSMBASICLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(GSMBASICLIB) $(GSMBASICOBJ)
	$(RANLIB) $(GSMBASICLIB)

$(GSMNETWORKLIB): $(GSMNETWORKOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(GSMNETWORKOBJ) to $(GSMNETWORKLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(GSMNETWORKLIB) $(GSMNETWORKOBJ)
	$(RANLIB) $(GSMNETWORKLIB)

$(GSMSIMLIB): $(GSMSIMOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(GSMSIMOBJ) to $(GSMSIMLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(GSMSIMLIB) $(GSMSIMOBJ)
	$(RANLIB) $(GSMSIMLIB)

$(GSMPHBKLIB): $(GSMPHBKOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(GSMPHBKOBJ) to $(GSMPHBKLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(GSMPHBKLIB) $(GSMPHBKOBJ)
	$(RANLIB) $(GSMPHBKLIB)

$(GSMVOICELIB): $(GSMVOICEOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(GSMVOICEOBJ) to $(GSMVOICELIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(GSMVOICELIB) $(GSMVOICEOBJ)
	$(RANLIB) $(GSMVOICELIB)

$(GSMSTKLIB): $(GSMSTKOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(GSMSTKOBJ) to $(GSMSTKLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(GSMSTKLIB) $(GSMSTKOBJ)
	$(RANLIB) $(GSMSTKLIB)

$(CDMAGENERALLIB): $(CDMAGENERALOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMAGENERALOBJ) to $(CDMAGENERALLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMAGENERALLIB) $(CDMAGENERALOBJ)
	$(RANLIB) $(CDMAGENERALLIB)

$(CMLBSLIB): $(CMLBSOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CMLBSOBJ) to $(CMLBSLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CMLBSLIB) $(CMLBSOBJ)
	$(RANLIB) $(CMLBSLIB) 

$(CDMABASICLIB): $(CDMABASICOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMABASICOBJ) to $(CDMABASICLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMABASICLIB) $(CDMABASICOBJ)
	$(RANLIB) $(CDMABASICLIB)

$(CDMALBSLIB): $(CDMALBSOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMALBSOBJ) to $(CDMALBSLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMALBSLIB) $(CDMALBSOBJ)
	$(RANLIB) $(CDMALBSLIB)

$(CMDMLIB): $(CMDMOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CMDMOBJ) to $(CMDMLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CMDMLIB) $(CMDMOBJ)
	$(RANLIB) $(CMDMLIB)

$(CDMAINTERNALLIB): $(CDMAINTERNALOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMAINTERNALOBJ) to $(CDMAINTERNALLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMAINTERNALLIB) $(CDMAINTERNALOBJ)
	$(RANLIB) $(CDMAINTERNALLIB)

$(CDMAIOTALIB): $(CDMAIOTAOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMAIOTAOBJ) to $(CDMAIOTALIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMAIOTALIB) $(CDMAIOTAOBJ)
	$(RANLIB) $(CDMAIOTALIB)

$(CDMANETWORKLIB): $(CDMANETWORKOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMANETWORKOBJ) to $(CDMANETWORKLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMANETWORKLIB) $(CDMANETWORKOBJ)
	$(RANLIB) $(CDMANETWORKLIB)

$(CDMARUIMLIB): $(CDMARUIMOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMARUIMOBJ) to $(CDMARUIMLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMARUIMLIB) $(CDMARUIMOBJ)
	$(RANLIB) $(CDMARUIMLIB)

$(CDMASMSLIB): $(CDMASMSOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMASMSOBJ) to $(CDMASMSLIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMASMSLIB) $(CDMASMSOBJ)
	$(RANLIB) $(CDMASMSLIB)

$(CDMAVOICELIB): $(CDMAVOICEOBJ)
	@echo "--------------------------------------------" 
	@echo "--- Archiving $(CDMAVOICEOBJ) to $(CDMAVOICELIB)..."
	@echo "--------------------------------------------" 
	@if [ -d $(CORELIBDIR) ] ; then echo "$(CORELIBDIR) exists..." ;\
	else mkdir -p $(CORELIBDIR) ;\
	fi;
	$(AR) rvu $(CDMAVOICELIB) $(CDMAVOICEOBJ)
	$(RANLIB) $(CDMAVOICELIB)

#-------------------------------------------------------------------------------
# Test Targets
#-------------------------------------------------------------------------------
$(APITESTEXE):  $(APITESTSRC) $(OBJ)
	@echo "--------------------------" 
	@echo "--- Building $(APITESTEXE)..." 
	@echo "--------------------------"
	-$(CC) $(INCLUDE) $(TECHFLAG) $(APITESTSRC) $(APITESTLIBS) $(LDFLAGS) \
    -o $(APITESTEXE)
#
$(SDKTESTEXE):  $(SDKTESTSRC) $(OBJ)
	@echo "--------------------------"
	@echo "--- Building $(SDKTESTEXE)..."
	@echo "--------------------------" 
	-$(CC) $(INCLUDE) $(TECHFLAG) $(SDKTESTSRC) \
    $(SDKTESTLIBS) $(LDFLAGS) -o $(SDKTESTEXE)

#
$(TESTEXE):  $(TESTSRC) $(OBJS) $(SDKLIB) $(CORELIB)
	@echo "--------------------------"
	@echo "--- Building $(TESTEXE)..."
	@echo "--------------------------"
	-$(CC) $(INCLUDE) $(TECHFLAG) $(TESTSRC) $(TESTLIBS) \
    $(LDFLAGS) $(EXTRA_KERN24_LDFLAGS) \
    $(DBGFLAGS) -o $(TESTEXE)

$(GSMTESTEXE):  $(GSMTESTSRC) $(OBJ)
	@echo "--------------------------"
	@echo "--- Building $(GSMTESTEXE)..."
	@echo "--------------------------"
	-$(CC) $(INCLUDE) $(TECHFLAG) $(GSMTESTSRC) $(TESTLIBS) $(LDFLAGS) \
    $(DBGFLAGS) -o $(GSMTESTEXE)
#
#--------------------------------------------------------------------------------
# Clean files at top level or package level, depending on where make is invoked
# PKGCLEANOBJS depends on .c files in each of the $(PKG) directory.  
#--------------------------------------------------------------------------------
PKGCLEANOBJS = $(SRCS:%.c=$(OBJSDIR)/%.o)
.PHONY: clean
clean:
	@echo "Cleaning $(PKG) files..."    
	@-rm -f $(TESTOBJ) $(TESTEXE) $(APITESTEXE) $(SDKTESTEXE) \
           $(DEPSDIR)/*.d *~ $(PKGCLEANOBJS) 
  
.PHONY: debug
debug:
	@echo "CFLAGS="$(CFLAGS)
