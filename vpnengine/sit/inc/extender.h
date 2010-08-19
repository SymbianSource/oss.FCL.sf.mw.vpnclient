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
* Description: Extended functinality for policy provision process.
*
*/

#ifndef EXTENDER_H_
#define EXTENDER_H_

#include <e32base.h>
#include "vpnextapi.h"
#include "vpnnotifierdefs.h"


const TInt KStateCheckPolicyUpdateAvailability      = 1;
const TInt KStateAfterCheckPolicyUpdateAvailability = 2;
const TInt KStateAskUpdateConfirmation              = 3;
const TInt KStateAfterAskUpdateConfirmation         = 4;
const TInt KStateUpdatePolicy                       = 5;
const TInt KStateAfterUpdatePolicy                  = 6;
const TInt KStateShowUpdateCompleteNote             = 8;
const TInt KStateAfterShowUpdateCompleteNote        = 9;
const TInt KStateShowApActFailureNote               = 10;
const TInt KStateAfterShowApActFailureNote          = 11;
const TInt KStateBeforeEnrollCertificates           = 12;


static const TUid KDmMsrNotificationUid = {0x1020699E};

class CVpnConnStarter;
class CExtenderHelper;

class CExtender : public CActive
    {
public:
    static CExtender* NewL();
    ~CExtender();

public:
    void OnVpnApActStart(CVpnConnStarter* aVpnConnStarter);
    void OnVpnApActCancel();
    
    void OnVpnApActEnd(const TVpnPolicyId* aPolicyId, TInt aStatus, TUint32 aRealIapId);

private:
    CExtender();
    void ConstructL();

private:
    void GotoState(TInt aState);
    void SetCurrState(TInt aState);
    void SetNextState(TInt aState);
    TInt CurrState();
    TInt NextState();

    void ChangeStateL();
    void CancelOngoingOperation();
    
    void StateShowApActFailureNote();
    void StateAfterShowApActFailureNote();
    void StateCheckPolicyUpdateAvailabilityL();
    void StateAfterCheckPolicyUpdateAvailability();
    void StateAskUpdateConfirmation();
    void StateAfterAskUpdateConfirmationL();
    void StateUpdatePolicyL();
    
    void StateAfterUpdatePolicyL();
    
    void StateShowUpdateCompleteNote();
    void StateAfterShowUpdateCompleteNote();


    void ProcessComplete(TInt aStatus);
    void EndTask();

    TCmSettingSelection GetPolicyServerSelectionL();
    void ReportFailure(TUint32 aVpnIapId);
    
private: // From CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:
    CVpnConnStarter* iVpnConnStarter;
    TVpnPolicyId iPolicyId;
    TUint32 iRealIapId;

    TBool iPolicyUpdateAvailable;
    TInt iStateAfterUpdateConfirmation;

    RVpnExtApi iVpnExtServ;
    RNotifier iNotifier;

    TInt iCurrState;
    TInt iNextState;

    TInt iVpnActStatus;
    TInt iFinalStatus;
    
    CExtenderHelper* iExtenderHelper;

    TBool iShowCompletionNote;
    
    TPckgBuf<TVpnDialogInfo>    iDialogInfoDes;
    TPckgBuf<TVpnDialogOutput>  iDialogResponseDes;
    };


#endif // __EXTENDER__
