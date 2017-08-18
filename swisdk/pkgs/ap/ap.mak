#-------------------------------------------------------------------------------
#
#  $Id: ap.mak,v 1.34 2009/04/01 17:58:02 rfiler Exp $
#
#  Name:
#    ap.mak
#
#  Description:
#    Makefile to build the pkgs/ap package 
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

# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Local Settings
#-------------------------------------------------------------------------------
PKG  = ap
SRCS = SwiIntCore.c \
       SwiApiCmBasic.c SwiIntUpkCmBasic.c SwiIntPkCmBasic.c \
       SwiApiGsmNetwork.c SwiIntUpkGsmNetwork.c SwiIntPkGsmNetwork.c \
       SwiApiGsmPhonebook.c SwiIntUpkGsmPhonebook.c SwiIntPkGsmPhonebook.c \
       SwiApiGsmBasic.c SwiIntUpkGsmBasic.c \
       SwiApiNotify.c SwiIntDecodeNotify.c \
       SwiIntDecodeNotifyGsm.c SwiIntDecodeNotifyCdma.c \
       SwiApiGsmSim.c SwiIntUpkGsmSim.c SwiIntPkGsmSim.c \
       SwiApiGsmSs.c \
       SwiApiCmUpgrade.c SwiIntUpkCmUpgrade.c SwiIntPkCmUpgrade.c \
       SwiApiCmLbs.c SwiIntUpkCmLbs.c SwiIntPkCmLbs.c\
       SwiApiGsmLbs.c SwiIntUpkGsmLbs.c SwiIntPkGsmLbs.c \
       SwiIntUtil.c \
       aptestroutines.c aptestnotify.c aptestvoice.c \
       aptestgsmsim.c aptestgsmbasic.c aptestgsmnet.c \
       aptestcmupgrade.c aptestcmbasic.c aptestgsmlbs.c \
       aptestgsmphonebook.c  aptestgsmstk.c\
       SwiApiCdmaBasic.c \
       SwiIntCdma.c \
       SwiApiCdmaIota.c \
       SwiApiCdmaNetwork.c \
       SwiApiCdmaRuim.c \
       SwiApiCdmaSms.c \
       SwiApiCdmaVoice.c \
       SwiIntPkCdmaBasic.c \
       SwiIntPkCdmaInternal.c \
       SwiIntPkCdmaNetwork.c \
       SwiIntPkCdmaRuim.c \
       SwiIntPkCdmaVoice.c \
       SwiIntUpkCdmaBasic.c \
       SwiIntUpkCdmaInternal.c \
       SwiIntUpkCdmaNetwork.c \
       SwiIntUpkCdmaRuim.c \
       SwiApiCdmaLbs.c \
       SwiApiCmDm.c \
       SwiIntPkCdmaLbs.c \
       SwiIntPkCmDm.c \
       SwiIntUpkCdmaLbs.c \
       SwiIntUpkCmDm.c \
       SwiIntUpkCdmaSms.c \
       SwiApiGsmStk.c SwiIntUpkGsmStk.c SwiIntPkGsmStk.c \
       SwiApiGsmVoice.c \
       SwiIntPkGsmVoice.c \
       SwiIntUpkGsmVoice.c 

HDRS = apidefs.h apudefs.h apiproto.h apuproto.h \
       SwiRcodes.h \
       SwiApiCmBasic.h SwiApiGsmNetwork.h SwiApiGsmBasic.h \
       SwiApiGsmPhonebook.h \
       SwiApiNotify.h SwiIntDecodeNotify.h \
       SwiApiGsmSim.h \
       SwiApiGsmSs.h \
       SwiApiCmUpgrade.h \
       SwiApiCmLbs.h \
       SwiApiGsmLbs.h \
       SwiApiCdmaBasic.h \
       SwiIntCdma.h \
       SwiApiCdmaIota.h \
       SwiApiCdmaNetwork.h \
       SwiApiCdmaRuim.h \
       SwiApiCdmaSms.h \
       SwiApiCdmaVoice.h \
       SwiIntPkCdmaBasic.h \
       SwiIntPkCdmaInternal.h \
       SwiIntPkCdmaNetwork.h \
       SwiIntPkCdmaRuim.h \
       SwiIntPkCdmaVoice.h \
       SwiIntUpkCdmaBasic.h \
       SwiIntUpkCdmaInternal.h \
       SwiIntUpkCdmaNetwork.h \
       SwiIntUpkCdmaRuim.h \
       SwiApiCdmaLbs.h \
       SwiApiCmDm.h \
       SwiIntPkCdmaLbs.h \
       SwiIntPkCmDm.h \
       SwiIntUpkCdmaLbs.h \
       SwiIntUpkCmDm.h \
       SwiIntUpkCdmaSms.h \
       SwiIntUpkCmLbs.h \
       SwiIntPkCmLbs.h \
       SwiApiGsmVoice.h \
       SwiApiGsmStk.h
#-------------------------------------------------------------------------------
# Split the object files into their respective groups
#-------------------------------------------------------------------------------
CMBASICOBJ     = $(OBJSDIR)/SwiIntCore.o \
                 $(OBJSDIR)/SwiIntUtil.o \
                 $(OBJSDIR)/SwiApiCmBasic.o  \
                 $(OBJSDIR)/SwiIntPkCmBasic.o \
                 $(OBJSDIR)/SwiApiNotify.o \
                 $(OBJSDIR)/SwiIntDecodeNotify.o \
                 $(OBJSDIR)/SwiApiCmUpgrade.o \
                 $(OBJSDIR)/SwiIntPkCmUpgrade.o \
                 $(OBJSDIR)/SwiIntUpkCmUpgrade.o \
                 $(OBJSDIR)/SwiIntUpkCmBasic.o \
                 $(OBJSDIR)/SwiIntUpkGsmLbs.o \
                 $(OBJSDIR)/SwiIntUpkCdmaLbs.o \
                 $(OBJSDIR)/SwiIntUpkCmLbs.o


GSMBASICOBJ    = $(OBJSDIR)/SwiApiGsmBasic.o  \
                 $(OBJSDIR)/SwiIntDecodeNotifyGsm.o \
                 $(OBJSDIR)/SwiIntUpkGsmBasic.o \
                 $(OBJSDIR)/SwiIntUpkGsmNetwork.o \
                 $(OBJSDIR)/SwiIntUpkGsmPhonebook.o \
                 $(OBJSDIR)/SwiIntUpkGsmSim.o \
                $(OBJSDIR)/SwiIntUpkGsmStk.o \
                $(OBJSDIR)/SwiIntUpkGsmVoice.o 

GSMNETWORKOBJ  = $(OBJSDIR)/SwiApiGsmNetwork.o \
                 $(OBJSDIR)/SwiIntPkGsmNetwork.o
                 
GSMPHBKOBJ      = $(OBJSDIR)/SwiApiGsmPhonebook.o \
                 $(OBJSDIR)/SwiIntPkGsmPhonebook.o

GSMSIMOBJ      = $(OBJSDIR)/SwiApiGsmSim.o \
                 $(OBJSDIR)/SwiIntPkGsmSim.o
                 
GSMVOICEOBJ    = $(OBJSDIR)/SwiApiGsmVoice.o \
		 $(OBJSDIR)/SwiIntPkGsmVoice.o

GSMSTKOBJ      = $(OBJSDIR)/SwiApiGsmStk.o \
		 $(OBJSDIR)/SwiIntPkGsmStk.o

CMLBSOBJ       = $(OBJSDIR)/SwiApiGsmLbs.o \
                 $(OBJSDIR)/SwiIntPkGsmLbs.o \
                 $(OBJSDIR)/SwiApiCmLbs.o \
                 $(OBJSDIR)/SwiIntPkCmLbs.o 

CDMABASICOBJ   = $(OBJSDIR)/SwiApiCdmaBasic.o \
                 $(OBJSDIR)/SwiIntPkCdmaBasic.o \
                 $(OBJSDIR)/SwiIntUpkCdmaBasic.o \
                 $(OBJSDIR)/SwiIntDecodeNotifyCdma.o 


CDMALBSOBJ     = $(OBJSDIR)/SwiApiCdmaLbs.o \
                 $(OBJSDIR)/SwiIntPkCdmaLbs.o 

CMDMOBJ        = $(OBJSDIR)/SwiApiCmDm.o \
                 $(OBJSDIR)/SwiIntPkCmDm.o \
                 $(OBJSDIR)/SwiIntUpkCmDm.o

CDMAINTERNALOBJ= $(OBJSDIR)/SwiIntCdma.o \
                 $(OBJSDIR)/SwiIntPkCdmaInternal.o \
                 $(OBJSDIR)/SwiIntUpkCdmaInternal.o

CDMAIOTAOBJ    = $(OBJSDIR)/SwiApiCdmaIota.o

CDMANETWORKOBJ = $(OBJSDIR)/SwiApiCdmaNetwork.o \
                 $(OBJSDIR)/SwiIntPkCdmaNetwork.o \
                 $(OBJSDIR)/SwiIntUpkCdmaNetwork.o

CDMARUIMOBJ    = $(OBJSDIR)/SwiApiCdmaRuim.o \
                 $(OBJSDIR)/SwiIntPkCdmaRuim.o \
                 $(OBJSDIR)/SwiIntUpkCdmaRuim.o

CDMASMSOBJ     = $(OBJSDIR)/SwiApiCdmaSms.o \
                 $(OBJSDIR)/SwiIntUpkCdmaSms.o

CDMAVOICEOBJ   = $(OBJSDIR)/SwiApiCdmaVoice.o \
                 $(OBJSDIR)/SwiIntPkCdmaVoice.o

#
# This is not put into the libraries but linked directly
#
APTESTOBJ      = $(OBJSDIR)/aptestroutines.o $(OBJSDIR)/aptestnotify.o \
                 $(OBJSDIR)/aptestgsmsim.o $(OBJSDIR)/aptestgsmbasic.o \
                 $(OBJSDIR)/aptestcmupgrade.o $(OBJSDIR)/aptestcmbasic.o \
                 $(OBJSDIR)/aptestgsmnet.o $(OBJSDIR)/aptestgsmlbs.o \
                 $(OBJSDIR)/aptestvoice.o $(OBJSDIR)/aptestgsmphonebook.o \
		 $(OBJSDIR)/aptestgsmstk.o

# This is to make sure all .o 's are built
#
OBJS = $(CMBASICOBJ) $(GSMBASICOBJ) $(GSMNETWORKOBJ) $(GSMVOICEOBJ) \
       $(GSMSIMOBJ) $(GSMPHBKOBJ) $(GSMSTKOBJ) \
       $(CMLBSOBJ) \
       $(CDMABASICOBJ) \
       $(CDMAINTERNALOBJ) \
       $(CDMAIOTAOBJ) \
       $(CDMANETWORKOBJ) \
       $(CDMARUIMOBJ) \
       $(CDMASMSOBJ) \
       $(CDMAVOICEOBJ) \
       $(CDMALBSOBJ) \
       $(CMDMOBJ) 


# LIB and OBJ files and additional flags required by $(PKG) test program
#
TESTLIBS =  $(APTESTOBJ) $(CMBASICLIB)  $(GSMNETWORKLIB) \
            $(GSMSIMLIB) $(CMLBSLIB)  $(GSMBASICOBJ) $(GSMVOICELIB) \
            $(GSMPHBKLIB) \
	    $(GSMSTKLIB) \
            $(CDMABASICOBJ) \
            $(CDMAINTERNALOBJ) \
            $(CDMAIOTAOBJ) \
            $(CDMANETWORKOBJ) \
            $(CDMARUIMOBJ) \
            $(CDMASMSOBJ) \
            $(CDMAVOICEOBJ) \
            $(CDMALBSOBJ) \
            $(CMDMOBJ) \
            $(APILIB) $(CORELIB) 

#-------------------------------------------------------------------------------
# Common Rules & Settings
#-------------------------------------------------------------------------------
include ../gen.mak

#
# $Log: ap.mak,v $
