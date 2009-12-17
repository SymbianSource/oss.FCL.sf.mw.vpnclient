/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Cleans VPN data from device
*
*/

#include <e32base.h>
#include <f32file.h>
#include <centralrepository.h> // link against centralrepository.lib
#include <settingsinternalcrkeys.h>

#include "vpncleaner.h"


LOCAL_C void setKeysL();


// The starting point
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TVpnCleaner vpnc;
    vpnc.Clean();
    
    TRAP_IGNORE( setKeysL() );
    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
    }


LOCAL_C void setKeysL()
    {
    // Connecting and initialization:
    CRepository* repository = CRepository::NewL( 
        KCRUidCommunicationSettings );
    
    TUint fileAttr;
    RFs fs;
    TInt err = fs.Connect();
    
    if ( KErrNone == err )
        {
        _LIT( KRomPath, "z:\\sys\\bin\\kmdserver.exe" ); 
        
        if ( KErrNone != fs.Att( KRomPath, fileAttr ) )
            {
            repository->Set( KSettingsVPNSupported, 0 );
            repository->Delete( KSettingsVPNImplementation );
            }
        }

    fs.Close();

    delete repository;
    }


// End of File
