#-------------------------------------------------------------------------------
#
#  Name:
#    qa.mak
#
#  Description:
#    Makefile to build the pkgs/qa package
#
#   The following make targets are available in this makefile:
#
#     all           - make .o and .a image files (default)
#                     Test programs are also built when present
#     clean         - delete object directory and image files
#
#   The above targets can be made with the following command:
#
#     make -f ap.mak [CPU={i386, arm9}] [target]
#       defaults to i386 if CPU is omitted

# Copyright (c) 2011-2013 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = qa

SRCS =  $(SRCS_DMS) \
        $(SRCS_WDS) \
        $(SRCS_CBK) \
        $(SRCS_NAS) \
        $(SRCS_SMS) \
        $(SRCS_FMS) \
        $(SRCS_PDS) \
        $(SRCS_CAT) \
        $(SRCS_RMS) \
        $(SRCS_DCS) \
        $(SRCS_OMADM) \
        $(SRCS_SAR) \
        $(SRCS_SWIOMADMS) \
        $(SRCS_VOICE) \
        $(SRCS_SWI) \
        $(SRCS_UIM) \
        $(SRCS_AUDIO) \
        $(SRCS_IMS) \
        $(SRCS_IMSA) \
        $(SRCS_SWIAUDIO) \
        $(SRCS_QOS) \
        $(SRCS_LOC) \
        test/src/qaGobiApiWdsTest.c \
        test/src/qaGobiApiDmsTest.c \
        test/src/qaGobiApiNasTest.c \
        test/src/qaGobiApiCbkTest.c \
        test/src/qaGobiApiDcsTest.c \
        test/src/qaGobiApiCatTest.c \
        test/src/qaGobiApiPdsTest.c \
        test/src/qaGobiApiRmsTest.c \
        test/src/qaGobiApiFmsTest.c \
        test/src/qaGobiApiSmsTest.c \
        test/src/qaGobiApiOmaDmTest.c \
        test/src/qaGobiApiSarTest.c \
        test/src/qaGobiApiSwiOmadmsTest.c \
        test/src/qaGobiApiVoiceTest.c \
        test/src/qaGobiApiUimTest.c \
        test/src/qaGobiApiAudioTest.c \
        test/src/qaGobiApiImsTest.c \
        test/src/qaGobiApiImsaTest.c \
        test/src/qaGobiApiSwiAudioTest.c \
        test/src/qaGobiApiQosTest.c \
        test/src/qaGobiApiLocTest.c \
        src/qaQmiBasic.c \
        src/qatestroutines.c

QMI_INCLUDE = -I.. \
              -I../qa/dcs/inc \
              -I../qa/wds/inc \
              -I../qa/dms/inc \
              -I../qa/sms/inc \
              -I../qa/nas/inc \
              -I../qa/rms/inc \
              -I../qa/pds/inc \
              -I../qa/cat/inc \
              -I../qa/fms/inc \
              -I../qa/swioma/inc \
              -I../qa/omadm/inc \
              -I../qa/sar/inc \
              -I../qa/swi/inc \
              -I../qa/voice/inc \
              -I../qa/uim/inc \
              -I../qa/audio/inc \
              -I../qa/ims/inc \
              -I../qa/imsa/inc \
              -I../qa/swiaudio/inc \
              -I../qa/qos/inc \
              -I../qa/loc/inc \
              -I../qa/cbk/cat/inc \
              -I../qa/cbk/dcs/inc \
              -I../qa/cbk/dms/inc \
              -I../qa/cbk/fms/inc \
              -I../qa/cbk/nas/inc \
              -I../qa/cbk/omadm/inc \
              -I../qa/cbk/pds/inc \
              -I../qa/cbk/swioma/inc \
              -I../qa/cbk/voice/inc \
              -I../qa/cbk/uim/inc \
              -I../qa/cbk/wds/inc \
              -I../qa/cbk/wms/inc \
              -I../qa/cbk/qos/inc \
              -I../qa/cbk/ims/inc \
              -I../qa/cbk/imsa/inc \
              -I../qa/cbk/loc/inc \
              -I../qa/cbk/inc \
              -I../qa/test/inc \
              -I../qa/inc \
              -I../qm \
              -I../am \
              -I../sl \
              -I../ic \
              -I../dr \
              -I../os

#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------

SLQSAPIOBJ    = $(OBJSDIR)/qaQmiBasic.o \

# This is to make sure all .o 's are built
#
OBJS = $(QATESTOBJ) $(SLQSAPIOBJ) $(WDSOBJ) $(DCSOBJ) $(CBKOBJ) $(NASOBJ) $(DMSOBJ) \
       $(SMSOBJ) $(FMSOBJ) $(PDSOBJ) $(CATOBJ) $(RMSOBJ) $(SWIOMADMSOBJ) \
       $(OMADMOBJ) $(SAROBJ) $(VOICEOBJ) $(UIMOBJ) $(QOSOBJ) $(SWIOBJ) $(AUDIOOBJ) \
       $(IMSOBJ) $(IMSAOBJ) $(SWIAUDIOOBJ) $(LOCOBJ)

#
# This is not put into the libraries but linked directly
#
QATESTOBJ      = $(OBJSDIR)/qatestroutines.o \
                 $(OBJSDIR)/qaGobiApiWdsTest.o \
                 $(OBJSDIR)/qaGobiApiDmsTest.o \
                 $(OBJSDIR)/qaGobiApiNasTest.o \
                 $(OBJSDIR)/qaGobiApiCbkTest.o \
                 $(OBJSDIR)/qaGobiApiDcsTest.o \
                 $(OBJSDIR)/qaGobiApiCatTest.o \
                 $(OBJSDIR)/qaGobiApiPdsTest.o \
                 $(OBJSDIR)/qaGobiApiRmsTest.o \
                 $(OBJSDIR)/qaGobiApiFmsTest.o \
                 $(OBJSDIR)/qaGobiApiSmsTest.o \
                 $(OBJSDIR)/qaGobiApiOmaDmTest.o \
                 $(OBJSDIR)/qaGobiApiSarTest.o \
                 $(OBJSDIR)/qaGobiApiSwiOmadmsTest.o \
                 $(OBJSDIR)/qaGobiApiVoiceTest.o \
                 $(OBJSDIR)/qaGobiApiUimTest.o \
                 $(OBJSDIR)/qaGobiApiAudioTest.o \
                 $(OBJSDIR)/qaGobiApiImsTest.o \
                 $(OBJSDIR)/qaGobiApiImsaTest.o \
                 $(OBJSDIR)/qaGobiApiSwiAudioTest.o \
                 $(OBJSDIR)/qaGobiApiQosTest.o \
                 $(OBJSDIR)/qaGobiApiLocTest.o

# LIB and OBJ files and additional flags required by $(PKG) test program
#
TESTLIBS =   $(QATESTOBJ) $(WDSLIB) $(DMSLIB) $(NASLIB) $(DCSLIB) $(CBKLIB) \
             $(CATLIB) $(RMSLIB) $(FMSLIB) $(SMSLIB) $(PDSLIB) $(SWIOMADMSLIB) \
             $(OMADMLIB) $(SARLIB) $(VOICELIB) $(SWILIB) $(UIMLIB) $(QOSLIB) $(AUDIOLIB) \
             $(IMSLIB) $(IMSALIB) $(SWIAUDIOLIB) $(LOCLIB) $(SLQSAPILIB) $(CORELIB)

vpath %.c \
	cat/src \
	cbk/cat/src \
	cbk/dcs/src \
	cbk/dms/src \
	cbk/fms/src \
	cbk/nas/src \
	cbk/omadm/src \
	cbk/pds/src \
	cbk/src \
	cbk/swioma/src \
	cbk/uim/src \
	cbk/voice/src \
	cbk/wds/src \
	cbk/wms/src \
	cbk/qos/src \
	cbk/ims/src \
    cbk/imsa/src \
    cbk/loc/src \
	dcs/src \
	dms/src \
	fms/src \
	nas/src \
	omadm/src \
	pds/src \
	rms/src \
	sar/src \
	sms/src \
	src \
	swi/src \
	swioma/src \
	test/src \
	uim/src \
	voice/src \
	qos/src \
	wds/src \
	audio/src \
	ims/src \
    imsa/src \
    swiaudio/src \
    loc/src


#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------

# Include the services elements.
include nas/nas.mak
include dcs/dcs.mak
include wds/wds.mak
include dms/dms.mak
include cbk/cbk.mak
include fms/fms.mak
include rms/rms.mak
include cat/cat.mak
include pds/pds.mak
include sms/sms.mak
include omadm/omadm.mak
include sar/sar.mak
include swioma/swiomadms.mak
include voice/voice.mak
include swi/swi.mak
include uim/uim.mak
include audio/audio.mak
include ims/ims.mak
include imsa/imsa.mak
include swiaudio/swiaudio.mak
include qos/qos.mak
include loc/loc.mak

include ../gen.mak
#
