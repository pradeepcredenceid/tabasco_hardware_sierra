#-------------------------------------------------------------------------------
#
#  $Id: utilities.trg,v 1.5 2009/03/30 22:00:43 blee Exp $
#
#  Name:
#    utilities.trg
#
#  Description:
#    Define paths to all utilities here relative to the base utilities directory
#
# Copyright (c) 2008, 2009 by Sierra Wireless, Incorporated. All Rights Reserved
#-------------------------------------------------------------------------------
#
#
COM_MAIN = Common/Main

# CDMA Utilities Paths
CDMA_FWUPDATER = cdma/SwiFWUpdater

# UMTS Utilities Paths
UMTS_FIRMWAREDOWNLOAD = gsm/FirmwareDownload

# Common Utilities Paths
UMTS_DIAGNOSTIC = Common/Diagnostic
UMTS_RELAYAGENT = Common/Relayagent

#
UTILITIESDIRS = $(CDMA_FWUPDATER) $(UMTS_FIRMWAREDOWNLOAD) $(UMTS_DIAGNOSTIC) $(UMTS_RELAYAGENT)

#
# $Log: utilities.trg,v $
# Revision 1.5  2009/03/30 22:00:43  blee
# BUG #15436
# Added UMTS_RELAYAGENT
#
# Revision 1.4  2009/03/04 02:44:36  rfiler
# Replaced path to diagnostic utility to Common folder from gsm folder.
# Updated copyright information and added $Log to bottom of file
#
#
#
