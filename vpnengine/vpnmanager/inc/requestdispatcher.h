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
* Description: Dispatches requests from clients to CVpnApiServant.
*
*/

#ifndef __REQUESTDISPATCHER_H__
#define __REQUESTDISPATCHER_H__

#include <e32base.h>

class RFs;
class CVpnApiServant;


/**
 * Server-level request dispatcher.
 * Dispatches the requests received from VPN Manager
 * client to the actual request servants that operate
 * at the server level (of which there is only one
 * instance).
 */
class CRequestDispatcher : public CBase
    {
public:
    static CRequestDispatcher* NewL(RFs& aFs);
    ~CRequestDispatcher();

    TBool ServiceL(const RMessage2& aMessage, CSession2* aSession);
    
private:
    CRequestDispatcher(RFs& aFs);
    void ConstructL();

public:
    CVpnApiServant* iVpnApiServant;
    RFs& iFs;
    };


#endif  // __REQUESTDISPATCHER_H__
