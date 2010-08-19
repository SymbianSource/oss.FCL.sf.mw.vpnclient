/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CmManager related utility functionality
*
*/

#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include <cmpluginvpndef.h>

#include "cmutils.h"


EXPORT_C void CmUtils::GetPolicyIdL( const TUint32 aVpnApId, TVpnPolicyId& aPolId )
{
    using namespace CMManager;
    
    RCmManager cmManager;
    
    cmManager.OpenL();
    CleanupClosePushL( cmManager );      
    
    RCmConnectionMethod cm = cmManager.ConnectionMethodL( aVpnApId );
    CleanupClosePushL( cm );

    HBufC* policy = cm.GetStringAttributeL( EVpnServicePolicy );

    if( policy && policy->Length() <= aPolId.MaxLength() )
    {
        aPolId.Copy( *policy );
    }
    
    delete policy;

    CleanupStack::PopAndDestroy( 2 );  // cm and cmManager
}


/***/
