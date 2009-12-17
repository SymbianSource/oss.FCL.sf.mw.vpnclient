#
# Copyright (c) 2006 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#	 This file provides the information required for building the
#  VPN Client sis files.
#

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD :
	@echo Processing stub_vpnpolins_armv5.pkg
	makesis -v -s stub_vpnpolins_armv5.pkg ipsecvpn_vpnpolins.SIS
	
	@echo Processing stub_nokia_vpn_client_armv5.pkg
	makesis -v -s stub_nokia_vpn_client_armv5.pkg ipsecvpn.SIS
	
	@echo Export stub sis files to $(EPOCROOT)epoc32\data\z\system\install
	copy ipsecvpn_vpnpolins.SIS $(EPOCROOT)epoc32\data\z\system\install
	copy ipsecvpn.SIS $(EPOCROOT)epoc32\data\z\system\install

CLEAN : 
	if exist ipsecvpn_vpnpolins.SIS erase ipsecvpn_vpnpolins.SIS
	if exist $(EPOCROOT)epoc32/data/z/system/install/ipsecvpn_vpnpolins.SIS erase $(EPOCROOT)epoc32/data/z/system/install/ipsecvpn_vpnpolins.SIS
	if exist $(EPOCROOT)epoc32/data/z/system/install/ipsecvpn.SIS erase $(EPOCROOT)epoc32/data/z/system/install/ipsecvpn.SIS
	if exist ipsecvpn.SIS erase ipsecvpn.SIS
	if exist *BACKUP erase *BACKUP

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing		
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	if exist ipsecvpn.SIS @echo ipsecvpn.SIS
	if exist ipsecvpn_vpnpolins.SIS @echo ipsecvpn_vpnpolins.SIS
	
FINAL : do_nothing
