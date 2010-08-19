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
* Description: Helper class for extended functionality for policy provision process.
*
*/

#ifndef EXTENDERHELPER_H_
#define EXTENDERHELPER_H_


#include <e32base.h>

#include "vpnnotifierdefs.h"

class CExtender;

NONSHARABLE_CLASS(CExtenderHelper) : public CActive
    {
public:
    static CExtenderHelper* NewL();
    ~CExtenderHelper();
    
private:
    CExtenderHelper();
    void ConstructL();
    
private: // From CActive
    void DoCancel();
    void RunL();


public:
    void StartNotifierL(CExtender* aExtender);
    
private:
    RNotifier iNotifier;
    
    CExtender* iExtender;
    
    TPckgBuf<TVpnDialogInfo>    iDialogInfoDes;
    TPckgBuf<TVpnDialogOutput>  iDialogResponseDes;
    
    };
#endif // __EXTENDERHELPER__
