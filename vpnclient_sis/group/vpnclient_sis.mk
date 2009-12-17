#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  Creates either localised or nonlocalised mVPN Client SIS files 
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

VERSION=091014
RDDERDIR=../data/RDTest_02.der
RDKEYDIR=../data/RDTest_02.key
LOCALISATION_FILES=$(EPOCROOT)epoc32/data/Z/resource/vpnecomnotifier.r05
S60PLATFORM=v42

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD :

#
# Localisation files must exist
#
	@echo --
	if exist $(LOCALISATION_FILES) @echo Building localised version
	if not exist $(LOCALISATION_FILES) @echo Localisation files are not available! Exiting..
	if not exist $(LOCALISATION_FILES) @echo --
	if not exist $(LOCALISATION_FILES) exit 1
	@echo --

#
# vpn policy installer
#
	@echo Processing temp_nokia_vpn_vpnpolins_armv5.pkg
	$(EPOCROOT)epoc32\tools\makesis -v temp_nokia_vpn_vpnpolins_armv5.pkg mVPN_vpnpolins_armv5.sis
	@echo Signing created mVPN_vpnpolins_armv5.sis
	$(EPOCROOT)epoc32\tools\signsis mVPN_vpnpolins_armv5.sis mVPN_vpnpolins_armv5.sis $(RDDERDIR) $(RDKEYDIR)
	
#
# If localisation files exist create localised SIS files.
#
# UREL 
	@echo Processing temp_nokia_vpn_client_localised_armv5_urel.pkg
	$(EPOCROOT)epoc32\tools\makesis -v temp_nokia_vpn_client_localised_armv5_urel.pkg mVPN_RnD_$(S60PLATFORM)_$(VERSION)_urel.sis
	@echo Signing created mVPN_RnD_$(S60PLATFORM)_$(VERSION)_urel.sis
	$(EPOCROOT)epoc32\tools\signsis mVPN_RnD_$(S60PLATFORM)_$(VERSION)_urel.sis mVPN_RnD_$(S60PLATFORM)_$(VERSION)_urel.sis $(RDDERDIR) $(RDKEYDIR)	
# UDEB	
	@echo Processing temp_nokia_vpn_client_localised_armv5_udeb.pkg
	$(EPOCROOT)epoc32\tools\makesis -v temp_nokia_vpn_client_localised_armv5_udeb.pkg mVPN_RnD_$(S60PLATFORM)_$(VERSION)_udeb.sis
	@echo Signing created mVPN_RnD_$(S60PLATFORM)_$(VERSION)_udeb.sis
	$(EPOCROOT)epoc32\tools\signsis mVPN_RnD_$(S60PLATFORM)_$(VERSION)_udeb.sis mVPN_RnD_$(S60PLATFORM)_$(VERSION)_udeb.sis $(RDDERDIR) $(RDKEYDIR)

		    
# remove policy installer
	@echo Remove unnecessary temporary sis files
	if exist mVPN_vpnpolins_armv5.sis erase mVPN_vpnpolins_armv5.sis
	if exist temp_* erase temp_*
	
CLEAN : 
	if exist mVPN_RnD_$(S60PLATFORM)_$(VERSION)_urel.sis erase mVPN_RnD_$(S60PLATFORM)_$(VERSION)_urel.sis
	if exist mVPN_RnD_$(S60PLATFORM)_$(VERSION)_udeb.sis erase mVPN_RnD_$(S60PLATFORM)_$(VERSION)_udeb.sis
	if exist mVPN_vpnpolins_armv5.sis erase mVPN_vpnpolins_armv5.sis
	if exist temp_* erase temp_*
	if exist *.BACKUP erase *.BACKUP
	

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing		
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES : do_nothing

FINAL : do_nothing
