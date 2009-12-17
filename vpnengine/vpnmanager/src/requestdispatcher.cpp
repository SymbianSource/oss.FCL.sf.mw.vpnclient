/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Dispatches requests from clients (API) to CVpnApiServant.
*
*/

#include "requestdispatcher.h"
#include "vpnapiservant.h"


CRequestDispatcher* CRequestDispatcher::NewL(RFs& aFs)
    {
    CRequestDispatcher* self = new (ELeave) CRequestDispatcher(aFs);
    CleanupStack::PushL(self) ;
    self->ConstructL() ;
    CleanupStack::Pop(self);
    return self ;
    }

void CRequestDispatcher::ConstructL()
    {
    iVpnApiServant = CVpnApiServant::NewL(iFs);
    }

CRequestDispatcher::CRequestDispatcher(RFs& aFs)
    : iFs(aFs)
    {
    }

CRequestDispatcher::~CRequestDispatcher()
    {
    delete iVpnApiServant;
    }

TBool CRequestDispatcher::ServiceL(const RMessage2& aMessage, CSession2* /*aSession*/)
    {
    TBool requestHandled = EFalse;

    requestHandled = iVpnApiServant->ServiceL(aMessage);
    
    if (!requestHandled)
        {
        User::Leave(KErrNotSupported);
        }

	return requestHandled;
    }
