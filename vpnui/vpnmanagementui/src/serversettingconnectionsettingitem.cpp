/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*   Nokia Corporation
*
* Description:   Setting item for the connection settings.
*
*/

#include "serversettingconnectionsettingitem.h"

#include <cmmanagerext.h>
#include <cmpluginvpndef.h>
#include <cmsettingsui.h>
#include <cmapplicationsettingsui.h>
#include <cmdestinationext.h>

using namespace CMManager;

CServerSettingConnectionSettingItem::CServerSettingConnectionSettingItem(TInt aIdentifier,
                                                                         TCmSettingSelection& aSelection)
:CAknSettingItem(aIdentifier), iExternalValue(aSelection)
    {
    }


CServerSettingConnectionSettingItem::~CServerSettingConnectionSettingItem()
    {
    delete iSettingText;
    }


void CServerSettingConnectionSettingItem::StoreL()
    {
    iExternalValue = iInternalValue;
    }


void CServerSettingConnectionSettingItem::LoadL()
    {
    iInternalValue = iExternalValue;
    UpdateSettingTextL();
    }


void CServerSettingConnectionSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    RCmManagerExt cmManager;
    cmManager.OpenLC();
    // Get supported bearer filter types
    const TInt KArrayGranularity = 10;
    RArray<TUint32> bearers = RArray<TUint32>( KArrayGranularity );
    CleanupClosePushL( bearers );
    cmManager.SupportedBearersL( bearers );
    // Do not include VPN bearer
    TInt index = bearers.Find( KPluginVPNBearerTypeUid );
    if ( index != KErrNotFound )
    	{
    	bearers.Remove( index );
    	}
    // Show settings page

    CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL( settings );
    TBool selected = settings->RunApplicationSettingsL( iInternalValue,
    										 			EShowDestinations |
    										 			EShowConnectionMethods,
    										 			bearers );
    CleanupStack::PopAndDestroy( 2 ); // settings, bearers

    if ( selected )
    	{
        UpdateSettingTextL();
    	UpdateListBoxTextL();
    	}

    CleanupStack::PopAndDestroy(); // cmManager

    }


const TDesC& CServerSettingConnectionSettingItem::SettingTextL()
    {
    if (iSettingText == NULL)
        {
        return CAknSettingItem::SettingTextL();
        }
    else
        {
        return *iSettingText;
        }
    }


void CServerSettingConnectionSettingItem::UpdateSettingTextL()
    {
    delete iSettingText;
    iSettingText = NULL;

    if ((iInternalValue.iResult == EDestination ||
        iInternalValue.iResult == EConnectionMethod) &&
        iInternalValue.iId != 0)
        {

        RCmManagerExt cmManager;
        cmManager.OpenLC();

        if ( iInternalValue.iResult ==  EDestination )
            {
            // Destination selected
            RCmDestinationExt dest;
            TRAPD( err, dest = cmManager.DestinationL( iInternalValue.iId ) );
            
            if( KErrNone == err )
                {
                CleanupClosePushL( dest );
                iSettingText = dest.NameLC();
                CleanupStack::Pop(iSettingText);
                CleanupStack::PopAndDestroy(); // dest
                }
            }
        else if ( iInternalValue.iResult == EConnectionMethod )
            {
            // Connection method selected
            RCmConnectionMethodExt conn;
            TRAPD( err, conn = cmManager.ConnectionMethodL( iInternalValue.iId ) );
            
            if( KErrNone == err )
                {
                CleanupClosePushL( conn );
                iSettingText = conn.GetStringAttributeL( ECmName );
                CleanupStack::PopAndDestroy(); // conn
                }
            }

        CleanupStack::PopAndDestroy(); // cmManager
        }
    }

/***/
