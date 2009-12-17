/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Common definitions of the vpnmanagementui.dll
*
*/



#ifndef  __VPNMANAGEMENTUIDEFS_H__
#define  __VPNMANAGEMENTUIDEFS_H__

// INCLUDES
#include <bldvariant.hrh>

#include <eikmenub.h>			// for menu and cba support
#include <aknnavide.h>
#include <akntabgrp.h>
#include <akntitle.h>
#include <aknlists.h>
#include <aknViewAppUi.h>
#include <barsread.h>			// for TResourceReader
#include <StringLoader.h>

// _LITS
_LIT(KVpnManagementUiEnter, "\n");
_LIT(KVpnManagementUiEnterTwice, "\n\n");
_LIT(KVpnManagementUiSpaceHyphenSpace, " - ");
_LIT(KVpnManagementUiSpace, " ");

_LIT(KIpsecAcuPlugInDllName, "ipsecacuplugin.dll");

// Pathname of the MBM file containing icons for VPN Log view
_LIT( KVpnLogIcons, "\\resource\\apps\\VpnManagementUi.mbm" );

// FORWARD DECLARATIONS
// CONSTS
const TUid KUidVpnManagementUi = { 0x10200EC4 }; //UID of dll for help
const TUid KUidIpsecAcuPlugIn = { 0x101F6EB8 };

const TInt KChangeViewBack = -2;
const TInt KChangeViewPrevious = -1;
const TInt KChangeViewPolicy = 0;
const TInt KChangeViewServer = 1;
const TInt KChangeViewLog = 2;
const TInt KChangeViewSettings = 3;

// For setting the correct title 
const TInt KViewTitleManagementView = 4;
const TInt KViewTitlePolicyView = 5;
const TInt KViewTitleServerView = 6;
const TInt KViewTitleLogView = 7;
const TInt KViewTitleParametersView = 8;

// Granularity of the created iconarrays
LOCAL_D const TInt KGranularity = 4;

// "dd/mm/yyyy0"
const TInt KMaxLengthTextDateString = 11;				
// Maximum length of dynamic strings is 4092, 
// plus 13 resource strings with average lenght of 50 makes 650
// total 4742 rounded up to 5000
const TInt KMaxLengthTextDetailsBody = 5000;
			
// Max length for ave_list_setting_pane_1 is 52 and 
// for ave_list_setting_pane_2 51 (formatting 7) , total 103 + end mark
const TInt KMaxLengthTextSettListOneTwo = 104;
// Max length for ave_list_setting_pane_1 is 52
const TInt KMaxLengthTextSettListOne = 52;			

const TInt KMaxServerUrlLength = 512;
const TInt KMaxServerNameLength = 30;
const TInt KMaxUiSelectionNameLength = 50;

const TInt KMaxLogListLength = 50;
#endif // __VPNMANAGEMENTUIDEFS_H__

// End of File
