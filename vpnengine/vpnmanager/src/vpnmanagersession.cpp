/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Handles client requests.
*
*/



#include "vpnmanagerserver.h"
#include "vpnmanagerserverdefs.h"
#include "vpnmanagersession.h"
#include "requestdispatcher.h"
#include <e32svr.h>

CVpnManagerSession* CVpnManagerSession::NewL(CVpnManagerServer& aServer,
                                             CRequestDispatcher& aRequestDispatcher)
    {
    CVpnManagerSession* self = CVpnManagerSession::NewLC(aServer, aRequestDispatcher);
    CleanupStack::Pop(self);
    return self;
    }

CVpnManagerSession* CVpnManagerSession::NewLC(CVpnManagerServer& aServer,
                                              CRequestDispatcher& aRequestDispatcher)
    {
    CVpnManagerSession* self = new (ELeave) CVpnManagerSession(aServer, aRequestDispatcher);
    CleanupStack::PushL(self) ;
    self->ConstructL() ;
    return self ;
    }

CVpnManagerSession::CVpnManagerSession(CVpnManagerServer& aServer,
                                       CRequestDispatcher& aRequestDispatcher)
    : iServer(aServer), iRequestDispatcher(aRequestDispatcher)
    {
    }

void CVpnManagerSession::ConstructL()
    {
    iServer.IncrementSessions();
    }

CVpnManagerSession::~CVpnManagerSession()
    {
    iServer.DecrementSessions();
    }

void CVpnManagerSession::ServiceL(const RMessage2& aMessage)
    {
    iRequestDispatcher.ServiceL(aMessage, this);
    }
