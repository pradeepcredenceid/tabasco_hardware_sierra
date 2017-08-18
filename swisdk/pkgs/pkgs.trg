#-------------------------------------------------------------------------------
#
#  $Id: pkgs.trg,v 1.14 2009/06/01 23:17:36 blee Exp $
#
#  Name:
#    pkgs.trg
#
#  Description:
#    Define packages here for inclusion into pkgs.mak 
#
# Copyright (c) 2008 by Sierra Wireless, Incorporated.  All Rights Reserved.
#-------------------------------------------------------------------------------
#
# Variable definition - Packages, Ordered Alphabetically
AMDIR = am
APDIR = ap
AXDIR = apdx
AUDIR = apui
CNDIR = cn
DDDIR = dd
DLDIR = dl
DRDIR = dr
DSDIR = ds
ERDIR = er
HDDIR = hd
HPDIR = hp
ICDIR = ic
MMDIR = mm
OSDIR = os
PIDIR = pi
QLDIR = ql
SDKDIR = sdk
SLDIR = sl
SUDIR = su
USDIR = us
UIDIR = ui
CIDIR = ci
#
# Order is important in the following line
SUBDIRS =  $(OSDIR) $(MMDIR) $(ERDIR) $(SLDIR) $(QLDIR) $(PIDIR) $(DLDIR) $(ICDIR) $(USDIR) $(CIDIR) $(HDDIR) $(CNDIR) $(DDDIR) $(AMDIR) $(DSDIR) $(SUDIR) $(HPDIR) $(DRDIR) $(UIDIR) $(SDKDIR) $(APDIR) $(AXDIR) $(AUDIR) 


