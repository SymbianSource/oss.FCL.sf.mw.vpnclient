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


#include "extenderhelper.h"
#include "extender.h"


CExtenderHelper* CExtenderHelper::NewL()
    {
    CExtenderHelper* self = new (ELeave) CExtenderHelper();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }
    
CExtenderHelper::~CExtenderHelper()
    {
    iExtender=NULL;
    Cancel();
    iNotifier.Close();
    }
    
CExtenderHelper::CExtenderHelper() : CActive(EPriorityNormal)
    {
    
    }

void CExtenderHelper::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iNotifier.Connect());

    }

//From CActive
void CExtenderHelper::DoCancel()
    {
    iNotifier.CancelNotifier(KUidVpnDialogNotifier);
    }

void CExtenderHelper::RunL()
    {
    ASSERT(iStatus.Int() == KErrCancel);
    iExtender->Cancel();
    
    }


void CExtenderHelper::StartNotifierL(CExtender* aExtender)
    {
    iExtender=aExtender;
    
    TVpnDialogInfo dialogInfo(TVpnDialog::EPolicyInstallInProgress, 0);
    iDialogInfoDes() = dialogInfo;
    
    iNotifier.StartNotifierAndGetResponse(iStatus, KUidVpnDialogNotifier,
                                                   iDialogInfoDes, iDialogResponseDes);
    SetActive();
    }
