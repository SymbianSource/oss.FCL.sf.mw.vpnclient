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
* Description: Wrapper for vpnapi
*
*/

#include "vpnapiwrapper.h"
#include "vpnapi.h"
#include "log_vpnmanagementui.h"


CVpnApiWrapper* CVpnApiWrapper::NewL()
    {
    CVpnApiWrapper* self = new (ELeave) CVpnApiWrapper();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CVpnApiWrapper::CVpnApiWrapper() : 
    CActive(EPriorityNormal), iOngoingTask(ETaskNone)
    {
    }

CVpnApiWrapper::~CVpnApiWrapper()
    {
    delete iPolicyList;
    Cancel();
    iVpnApi.Close();
    }

void CVpnApiWrapper::DoCancel()
    {
    LOG_("CVpnApiWrapper::DoCancel() called\n");

    if (iOngoingTask == ETaskImportPolicies)
        {
        iVpnApi.CancelImport();
        }
    
    iOngoingTask = ETaskNone;
    }

void CVpnApiWrapper::RunL()
    {
    LOG_1("CVpnApiWrapper::RunL():%d", iStatus.Int());
       
    if (iOngoingTask == ETaskImportPolicies)
        {
        iCaller->NotifyPolicyImportComplete(iStatus.Int());
        }
    
    iOngoingTask = ETaskNone;
    }

void CVpnApiWrapper::ConstructL()
    {
    LOG_("CVpnApiWrapper::ConstructL() - begin");

    CActiveScheduler::Add(this);
    LOG_("CVpnApiWrapper::ConstructL() - begin 2");
    User::LeaveIfError(iVpnApi.Connect());

    LOG_("CVpnApiWrapper::ConstructL() - end");
    }

CArrayFixFlat<TVpnPolicyInfo>* CVpnApiWrapper::PolicyListL()
    {
    BuildPolicyListL();
    return iPolicyList;
    }

void CVpnApiWrapper::BuildPolicyListL()
    {
    LOG_("CVpnApiWrapper::BuildPolicyListL() - begin");

    delete iPolicyList;
    iPolicyList = NULL;
    
    iPolicyList = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(2);

    User::LeaveIfError(iVpnApi.GetPolicyInfoList(iPolicyList));
    
    TKeyArrayFix Key( _FOFF(TVpnPolicyInfo,iName), ECmpCollated );
    Key.SetPtr( iPolicyList );
    iPolicyList->Sort( Key );
    
    LOG_("CVpnApiWrapper::BuildPolicyListL() - end");
	}

void CVpnApiWrapper::DeletePolicyL(TInt aPolicyIndex)
    {
    LOG_("CVpnApiWrapper::DeletePolicyL() - begin");

    TVpnPolicyId& policyToDelete = iPolicyList->At(aPolicyIndex).iId;
    User::LeaveIfError(iVpnApi.DeletePolicy(policyToDelete));
    
    LOG_("CVpnApiWrapper::DeletePolicyL() - end");
    }

void CVpnApiWrapper::GetPolicyDetailsL(TInt aPolicyIndex, TVpnPolicyDetails& aPolicyDetails)
    {
    LOG_("CVpnApiWrapper::GetPolicyDetailsL() - begin");

    TVpnPolicyId& policyOfInterest = iPolicyList->At(aPolicyIndex).iId;
    User::LeaveIfError(iVpnApi.GetPolicyDetails(policyOfInterest, aPolicyDetails));

    LOG_("CVpnApiWrapper::GetPolicyDetailsL() - end");
    }

void CVpnApiWrapper::ImportPolicyL(const TDesC& aImportDir, MVpnApiWrapperCaller* aCaller)
    {
    LOG_("CVpnApiWrapper::ImportPolicyL() - begin");

    if (iOngoingTask != ETaskNone)
        {
        User::Leave(KErrInUse);
        }

    iCaller = aCaller;
    iImportDir.Copy(aImportDir);

    iVpnApi.ImportPolicy(iImportDir, iStatus);

    iOngoingTask = ETaskImportPolicies;
    SetActive();
    LOG_("CVpnApiWrapper::ImportPolicyL() - end");
    }


/***/
