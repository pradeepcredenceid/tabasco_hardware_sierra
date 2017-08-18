#-------------------------------------------------------------------------------
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
# Copyright (c) 2011-2013 by Sierra Wireless, Incorporated. All Rights Reserved
#-------------------------------------------------------------------------------
#.SUFFIXES:
#-------------------------------------------------------------------------------
ifndef QMI_BUILD_BASE
export QMI_BUILD_BASE:=$(shell until [ -d pkgs ]; do cd ..; done; pwd)
endif

include $(QMI_BUILD_BASE)/pkgs/slqscompile.mak
#-------------------------------------------------------------------------------
# Variable definitions
#-------------------------------------------------------------------------------
BUILDDIR        := $(QMI_BUILD_BASE)/build
OBJSDIR         := $(BUILDDIR)/obj/$(CPU)/pkgs/$(PKG)

BUILDLIBDIR     := $(BUILDDIR)/lib
CORELIBDIR      := $(BUILDLIBDIR)/$(CPU)
SDKLIBDIR       := $(BUILDLIBDIR)/pkgs/sdk/$(CPU)
DCSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
WDSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
SLQSAPILIBDIR   := $(BUILDLIBDIR)/$(CPU)
DMSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
NASLIBDIR       := $(BUILDLIBDIR)/$(CPU)
CBKLIBDIR       := $(BUILDLIBDIR)/$(CPU)
CATLIBDIR       := $(BUILDLIBDIR)/$(CPU)
RMSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
FMSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
SMSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
PDSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
OMADMLIBDIR     := $(BUILDLIBDIR)/$(CPU)
SWIOMADMSLIBDIR := $(BUILDLIBDIR)/$(CPU)
SARLIBDIR       := $(BUILDLIBDIR)/$(CPU)
VOICELIBDIR     := $(BUILDLIBDIR)/$(CPU)
SWILIBDIR       := $(BUILDLIBDIR)/$(CPU)
UIMLIBDIR       := $(BUILDLIBDIR)/$(CPU)
AUDIOLIBDIR     := $(BUILDLIBDIR)/$(CPU)
SWIAUDIOLIBDIR  := $(BUILDLIBDIR)/$(CPU)
QOSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
IMSLIBDIR       := $(BUILDLIBDIR)/$(CPU)
IMSALIBDIR       := $(BUILDLIBDIR)/$(CPU)
QMILIBDIR       := $(BUILDLIBDIR)/$(CPU)
LOCLIBDIR       := $(BUILDLIBDIR)/$(CPU)

CORELIB         := $(CORELIBDIR)/libslqscore.a
SDKLIB          := $(SDKLIBDIR)/libslqssdk.a
SLQSAPILIB      := $(SLQSAPILIBDIR)/libslqsapi.a

#
# QMI Services libraries
#
DCSLIB          := $(DCSLIBDIR)/libdcs.a
WDSLIB          := $(WDSLIBDIR)/libwds.a
DMSLIB          := $(DMSLIBDIR)/libdms.a
NASLIB          := $(NASLIBDIR)/libnas.a
CBKLIB          := $(CBKLIBDIR)/libcbk.a
CATLIB          := $(CATLIBDIR)/libcat.a
RMSLIB          := $(RMSLIBDIR)/librms.a
FMSLIB          := $(FMSLIBDIR)/libfms.a
SMSLIB          := $(SMSLIBDIR)/libsms.a
PDSLIB          := $(PDSLIBDIR)/libpds.a
OMADMLIB        := $(OMADMLIBDIR)/libomadm.a
SWIOMADMSLIB    := $(SWIOMADMSLIBDIR)/libswiomadms.a
SARLIB          := $(SARLIBDIR)/libsar.a
VOICELIB        := $(VOICELIBDIR)/libvoice.a
UIMLIB          := $(UIMLIBDIR)/libuim.a
AUDIOLIB        := $(AUDIOLIBDIR)/libaudio.a
IMSLIB          := $(IMSLIBDIR)/libims.a
IMSALIB         := $(IMSALIBDIR)/libimsa.a
SWIAUDIOLIB     := $(SWIAUDIOLIBDIR)/libswiaudio.a
LOCLIB          := $(LOCLIBDIR)/libloc.a

SWILIB          := $(SWILIBDIR)/libswi.a
QOSLIB          := $(SWILIBDIR)/libqos.a

#
# Sum of all QMI Services
#
QMILIB          := $(QMILIBDIR)/libslqsqmi.a

# -------------------------------------------------------
# Build required programs, $(CPU)/slqssdk and qatest$(CPU)
#--------------------------------------------------------
TESTSRC = $(PKG)test.c
have_testsrc := $(wildcard $(TESTSRC))
ifeq ($(strip $(have_testsrc)),sdktest.c)
TESTOBJ = $(OBJSDIR)/$(PKG)test.o
TESTEXE = $(CPU)/slqs$(PKG)
else
ifeq ($(TESTSRC),qatest.c)
TESTSRC = src/qatest.c
TESTOBJ = $(OBJSDIR)/$(PKG)test.o
TESTEXE = $(PKG)test$(CPU)
endif
endif
#
# extract current directory name for special processing of some files
#
CURRENT_DIR = $(notdir $(PWD))
#
#-------------------------------------------------------------------------------
# Default Target
#-------------------------------------------------------------------------------
all:	$(OBJS)\
$(CORELIB) $(SDKLIB)  \
$(DCSLIB) \
$(WDSLIB) \
$(DMSLIB) \
$(NASLIB) \
$(CBKLIB) \
$(CATLIB) \
$(RMSLIB) \
$(FMSLIB) \
$(SMSLIB) \
$(PDSLIB) \
$(OMADMLIB) \
$(SWIOMADMSLIB) \
$(SARLIB) \
$(VOICELIB) \
$(SWILIB) \
$(UIMLIB) \
$(AUDIOLIB) \
$(IMSLIB) \
$(IMSALIB) \
$(SWIAUDIOLIB) \
$(QOSLIB) \
$(LOCLIB) \
$(QMILIB) \
$(SLQSAPILIB) \
$(TESTEXE) $(SDKTESTEXE) $(APITESTEXE)

$(OBJS): $(OBJSDIR)/%.o:%.c $(HDRS)
	@echo "QMI Build base: $(QMI_BUILD_BASE)"
	@echo $(INCLUDE) $(LDFLAGS)
	@echo "============================"
	@echo "--- Building  $(PKG) for $(CPU)..."
	@echo "----------------------------"
	@if [ ! -d $(OBJSDIR) ] ; then mkdir -p $(OBJSDIR) ; fi;
	$(CC) $(CFLAGS) $(SLQSFLAG) $(INCLUDE) $< -o $@

#--
# When timestamp resolution is in seconds, it is possible that refreshly compiled object files
# has the same timestamp as target library.
# Ref: http://www.gnu.org/software/autoconf/manual/autoconf-2.64/html_node/Timestamps-and-Make.html
# Force archiving work around this issue
#--
.PHONY: $(CORELIB) $(SDKLIB) $(SLQSAPILIB)

$(CORELIB): $(COREOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(COREOBJ) to $(CORELIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(CORELIBDIR) ] ; then mkdir -p $(CORELIBDIR) ; fi;
	$(AR) rvu $(CORELIB) $(COREOBJ)
	$(RANLIB) $(CORELIB)

$(SDKLIB): $(SDKOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SDKOBJ) to $(SDKLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SDKLIBDIR) ] ; then mkdir -p $(SDKLIBDIR) ; fi;
	$(AR) rvu $(SDKLIB) $(SDKOBJ)
	$(RANLIB) $(SDKLIB)

$(DCSLIB): $(DCSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(DCSOBJ) to $(DCSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(DCSLIBDIR) ] ; then mkdir -p $(DCSLIBDIR) ; fi;
	$(AR) rvu $(DCSLIB) $(DCSOBJ)
	$(RANLIB) $(DCSLIB)

$(WDSLIB): $(WDSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(WDSOBJ) to $(WDSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(WDSLIBDIR) ] ; then mkdir -p $(WDSLIBDIR) ; fi;
	$(AR) rvu $(WDSLIB) $(WDSOBJ)
	$(RANLIB) $(WDSLIB)

$(DMSLIB): $(DMSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(DMSOBJ) to $(DMSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(DMSLIBDIR) ] ; then mkdir -p $(DMSLIBDIR) ; fi;
	$(AR) rvu $(DMSLIB) $(DMSOBJ)
	$(RANLIB) $(DMSLIB)

$(NASLIB): $(NASOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(NASOBJ) to $(NASLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(NASLIBDIR) ] ; then mkdir -p $(NASLIBDIR) ; fi;
	$(AR) rvu $(NASLIB) $(NASOBJ)
	$(RANLIB) $(NASLIB)

$(CBKLIB): $(CBKOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(CBKOBJ) to $(CBKLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(CBKLIBDIR) ] ; then mkdir -p $(CBKLIBDIR) ; fi;
	$(AR) rvu $(CBKLIB) $(CBKOBJ)
	$(RANLIB) $(CBKLIB)

$(RMSLIB): $(RMSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(RMSOBJ) to $(RMSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(RMSLIBDIR) ] ; then mkdir -p $(RMSLIBDIR) ; fi;
	$(AR) rvu $(RMSLIB) $(RMSOBJ)
	$(RANLIB) $(RMSLIB)

$(CATLIB): $(CATOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(CATOBJ) to $(CATLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(CATLIBDIR) ] ; then mkdir -p $(CATLIBDIR) ; fi;
	$(AR) rvu $(CATLIB) $(CATOBJ)
	$(RANLIB) $(CATLIB)

$(FMSLIB): $(FMSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(FMSOBJ) to $(FMSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(FMSLIBDIR) ] ; then mkdir -p $(FMSLIBDIR) ; fi;
	$(AR) rvu $(FMSLIB) $(FMSOBJ)
	$(RANLIB) $(FMSLIB)

$(SMSLIB): $(SMSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SMSOBJ) to $(SMSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SMSLIBDIR) ] ; then mkdir -p $(SMSLIBDIR) ; fi;
	$(AR) rvu $(SMSLIB) $(SMSOBJ)
	$(RANLIB) $(SMSLIB)

$(PDSLIB): $(PDSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(PDSOBJ) to $(PDSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(PDSLIBDIR) ] ; then mkdir -p $(PDSLIBDIR) ; fi;
	$(AR) rvu $(PDSLIB) $(PDSOBJ)
	$(RANLIB) $(PDSLIB)

$(OMADMLIB): $(OMADMOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(OMADMOBJ) to $(OMADMLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(OMADMLIBDIR) ] ; then mkdir -p $(OMADMLIBDIR) ; fi;
	$(AR) rvu $(OMADMLIB) $(OMADMOBJ)
	$(RANLIB) $(OMADMLIB)

$(SWIOMADMSLIB): $(SWIOMADMSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SWIOMADMSOBJ) to $(SWIOMADMSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SWIOMADMSLIBDIR) ] ; then mkdir -p $(SWIOMADMSLIBDIR) ; fi;
	$(AR) rvu $(SWIOMADMSLIB) $(SWIOMADMSOBJ)
	$(RANLIB) $(SWIOMADMSLIB)

$(SARLIB): $(SAROBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SAROBJ) to $(SARLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SARLIBDIR) ] ; then mkdir -p $(SARLIBDIR) ; fi;
	$(AR) rvu $(SARLIB) $(SAROBJ)
	$(RANLIB) $(SARLIB)

$(VOICELIB): $(VOICEOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(VOICEOBJ) to $(VOICELIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(VOICELIBDIR) ] ; then mkdir -p $(VOICELIBDIR) ; fi;
	$(AR) rvu $(VOICELIB) $(VOICEOBJ)
	$(RANLIB) $(VOICELIB)

$(SWILIB): $(SWIOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SWIOBJ) to $(SWILIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SWILIBDIR) ] ; then mkdir -p $(SWILIBDIR) ; fi;
	$(AR) rvu $(SWILIB) $(SWIOBJ)
	$(RANLIB) $(SWILIB)

$(UIMLIB): $(UIMOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(UIMOBJ) to $(UIMLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(UIMLIBDIR) ] ; then mkdir -p $(UIMLIBDIR) ; fi;
	$(AR) rvu $(UIMLIB) $(UIMOBJ)
	$(RANLIB) $(UIMLIB)

$(AUDIOLIB): $(AUDIOOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(AUDIOOBJ) to $(AUDIOLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(AUDIOLIBDIR) ] ; then mkdir -p $(AUDIOLIBDIR) ; fi;
	$(AR) rvu $(AUDIOLIB) $(AUDIOOBJ)
	$(RANLIB) $(AUDIOLIB)

$(IMSLIB): $(IMSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(IMSOBJ) to $(IMSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(IMSLIBDIR) ] ; then mkdir -p $(IMSLIBDIR) ; fi;
	$(AR) rvu $(IMSLIB) $(IMSOBJ)
	$(RANLIB) $(IMSLIB)

$(IMSALIB): $(IMSAOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(IMSAOBJ) to $(IMSALIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(IMSALIBDIR) ] ; then mkdir -p $(IMSALIBDIR) ; fi;
	$(AR) rvu $(IMSALIB) $(IMSAOBJ)
	$(RANLIB) $(IMSALIB)

$(SWIAUDIOLIB): $(SWIAUDIOOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SWIAUDIOOBJ) to $(SWIAUDIOLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SWIAUDIOLIBDIR) ] ; then mkdir -p $(SWIAUDIOLIBDIR) ; fi;
	$(AR) rvu $(SWIAUDIOLIB) $(SWIAUDIOOBJ)
	$(RANLIB) $(SWIAUDIOLIB)

$(QOSLIB): $(QOSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(QOSOBJ) to $(QOSLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(LIBDIR) ] ; then mkdir -p $(QOSLIBDIR) ; fi;
	$(AR) rvu $(QOSLIB) $(QOSOBJ)
	$(RANLIB) $(QOSLIB)

$(LOCLIB): $(LOCOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(LOCOBJ) to $(LOCLIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(LOCLIBDIR) ] ; then mkdir -p $(LOCLIBDIR) ; fi;
	$(AR) rvu $(LOCLIB) $(LOCOBJ)
	$(RANLIB) $(LOCLIB)

$(SLQSAPILIB): $(SLQSAPIOBJ) $(WDSOBJ) $(DMSOBJ) $(QOSOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving $(SLQSAPIOBJ) to $(SLQSAPILIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(SLQSAPILIBDIR) ] ; then mkdir -p $(SLQSAPILIBDIR) ; fi;
	$(AR) rvu $(SLQSAPILIB) $(SLQSAPIOBJ) $(FMSOBJ) $(WDSOBJ) $(DMSOBJ) $(QOSOBJ)
	$(RANLIB) $(SLQSAPILIB)

$(QMILIB): $(DCSOBJ) $(WDSOBJ) $(DMSOBJ) $(NASOBJ) $(CBKOBJ) $(CATOBJ) \
		   $(RMSOBJ) $(FMSOBJ) $(SMSOBJ) $(PDSOBJ) $(SWIOMADMSOBJ) $(OMADMOBJ) \
		   $(SAROBJ) $(VOICEOBJ) $(SWIOBJ) $(UIMOBJ) $(QOSOBJ) $(AUDIOOBJ) \
           $(IMSOBJ) $(IMSAOBJ) $(SWIAUDIOOBJ) $(LOCOBJ)
	@echo "--------------------------------------------"
	@echo "--- Archiving to $(QMILIB)..."
	@echo "--------------------------------------------"
	@if [ ! -d $(QMILIBDIR) ] ; then mkdir -p $(QMILIBDIR) ; fi;
	$(AR) rvu $(QMILIB) $(DCSOBJ) $(WDSOBJ) $(DMSOBJ) $(NASOBJ) $(CBKOBJ)
	$(AR) rvu $(QMILIB) $(CATOBJ) $(RMSOBJ) $(FMSOBJ) $(SMSOBJ) $(PDSOBJ)
	$(AR) rvu $(QMILIB) $(SWIOMADMSOBJ) $(OMADMOBJ) $(SAROBJ) $(VOICEOBJ) $(SWIOBJ) 
	$(AR) rvu $(QMILIB) $(UIMOBJ) $(QOSOBJ) $(AUDIOOBJ) $(IMSOBJ) $(IMSAOBJ) $(SWIAUDIOOBJ) $(LOCOBJ)
	$(RANLIB) $(QMILIB)

#-------------------------------------------------------------------------------
# Test Targets
#-------------------------------------------------------------------------------
#
$(TESTEXE):  $(TESTSRC) $(OBJS) $(SDKLIB) $(CORELIB) $(SLQSAPILIB) $(HDRS)
	@echo "--------------------------"
	@echo "--- Building $(TESTEXE)..."
	@echo "--------------------------"
	@if [ ! -d $(CPU) ] ; then mkdir -p $(CPU) ; fi ;
	-$(CC) $(INCLUDE) $(SLQSFLAG) $(TESTSRC) $(TESTLIBS) \
    $(LDFLAGS) -DSDK_EXE='"$(SDK_EXE)"'\
    $(DBGFLAGS) -o $(TESTEXE)
#
#--------------------------------------------------------------------------------
# Clean files
#--------------------------------------------------------------------------------
.PHONY: clean
clean:
	@echo "Cleaning $(PKG) files..."
	rm -fr $(OBJSDIR)/*.o

.PHONY: debug
debug:
	@echo "CFLAGS="$(CFLAGS)
