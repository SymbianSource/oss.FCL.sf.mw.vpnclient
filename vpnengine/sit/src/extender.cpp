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
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethoddef.h>
#include <centralrepository.h>

#include "sit.h"
#include "extender.h"
#include "log.h"
#include "vpnconnstarter.h"
#include "extenderhelper.h"


CExtender* CExtender::NewL()
    {
    LOG(Log::Printf(_L("CExtender::NewL - begin\n")));
    CExtender* self = new (ELeave) CExtender();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOG(Log::Printf(_L("CExtender::NewL - end\n")));
    return self;
    }
    
CExtender::~CExtender()
    {
    LOG(Log::Printf(_L("CExtender::~CExtender\n")));
    Cancel();
    
    iVpnExtServ.Close();
    iNotifier.Close();
    delete iExtenderHelper;
    }
    
CExtender::CExtender() : CActive(EPriorityNormal)
    {
    
    }

void CExtender::ConstructL()
    {
    CActiveScheduler::Add(this);    
    User::LeaveIfError(iVpnExtServ.Connect());
    User::LeaveIfError(iNotifier.Connect());
    }

void CExtender::DoCancel()
    {
    CancelOngoingOperation();
    }

void CExtender::RunL()
    {
     ChangeStateL();
    }

void CExtender::GotoState(TInt aState)
    {
    SetNextState(aState);
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }
    
void CExtender::SetCurrState(TInt aState)
    {
    iCurrState = aState;
    }

void CExtender::SetNextState(TInt aState)
    {
    iNextState = aState;
    }

TInt CExtender::CurrState()
    {
    return iCurrState;
    }

TInt CExtender::NextState()
    {
    return iNextState;
    }

TInt CExtender::RunError(TInt aError)
    {
    LOG(Log::Printf(_L("CExtender::RunError - error = %d\n"), aError));
    ProcessComplete(aError);
    return KErrNone;
    }

void CExtender::ChangeStateL()
    {
    switch (NextState())
        {
        case KStateCheckPolicyUpdateAvailability:
            StateCheckPolicyUpdateAvailabilityL();
            break;
     
        case KStateAfterAskUpdateConfirmation:
            StateAfterAskUpdateConfirmationL();
            break;

        case KStateUpdatePolicy:
            StateUpdatePolicyL();
            break;
            
        case KStateAfterUpdatePolicy:
            StateAfterUpdatePolicyL();
            break;

        case KStateShowUpdateCompleteNote:
            StateShowUpdateCompleteNote();
            break;

        case KStateAfterShowUpdateCompleteNote:
            StateAfterShowUpdateCompleteNote();
            break;
           
        case KStateShowApActFailureNote:
            StateShowApActFailureNote();
            break;
                    
        case KStateAfterShowApActFailureNote:
            StateAfterShowApActFailureNote();
            break;
   
        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }   
    }

void CExtender::CancelOngoingOperation()
    {
    switch (CurrState())
        {
        case KStateShowApActFailureNote:
        case KStateAskUpdateConfirmation:
        case KStateShowUpdateCompleteNote:
            LOG(Log::Printf(_L("CExtender::CancelOngoingOperation - cancelling notifier\n")));
            iNotifier.CancelNotifier(KUidVpnDialogNotifier);
            break;
            
        case KStateCheckPolicyUpdateAvailability:
        case KStateUpdatePolicy:
        case KStateAfterUpdatePolicy:    
            LOG(Log::Printf(_L("CExtender::CancelOngoingOperation - cancelling Policy Provision operation\n")));
            iVpnExtServ.CancelPolicyProvision();
            iExtenderHelper->Cancel();
            break;
            
        case KStateAfterShowApActFailureNote:
        case KStateAfterAskUpdateConfirmation:
        case KStateAfterCheckPolicyUpdateAvailability:
        case KStateAfterShowUpdateCompleteNote:
            LOG(Log::Printf(_L("CExtender::CancelOngoingOperation - cancelling nothing\n")));
            // Nothing to cancel
            break;
            
        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }

void CExtender::OnVpnApActStart(CVpnConnStarter* aVpnConnStarter)
    {
    LOG(Log::Printf(_L("CExtender::OnVpnApActStart\n")));
    iVpnConnStarter = aVpnConnStarter;

    // Prevent the deletion of the iVpnConnStarter task
    // handler (that owns us) as long as we are running
    iVpnConnStarter->SetDelayedTaskEnd(ETrue);
    }

void CExtender::OnVpnApActCancel()
    {
    LOG(Log::Printf(_L("CExtender::OnVpnApActCancel\n")));

    // Allow the deletion of the iVpnConnStarter task handler
    // (the activation has been cancelled in which case
    // automatic policy updating process will not be
    // performed and thus the task handler can be deleted
    // right away after cancel)
    iVpnConnStarter->SetDelayedTaskEnd(EFalse);
    }

void CExtender::OnVpnApActEnd(const TVpnPolicyId* aPolicyId, TInt aStatus, TUint32 aRealIapId)
    {
    LOG(Log::Printf(_L("CExtender::OnVpnApActEnd\n")));
    iPolicyId.Copy(*aPolicyId);
    iRealIapId = aRealIapId;

    iVpnActStatus = aStatus;
    
    if (iVpnActStatus != KErrNone && iVpnActStatus != KErrCancel)
        {
        // Report the failure to the system
        // (management session runner if such is present)
        // First show a VPN connection activation failure note
        GotoState(KStateShowApActFailureNote);
        }
    else if (iVpnActStatus == KErrCancel)
        {
        // The user has cancelled the VPN connection activation,
        // in that case the automatic policy update operation is
        // not performed
        EndTask();
        }
    else
        {
        // Begin the combined policy update and
        // certificate enrollment process right away
        //GotoState(KStateCheckPolicyUpdateAvailability);
        EndTask();
        }
    }

void CExtender::StateShowApActFailureNote()
    {
    LOG(Log::Printf(_L("CExtender::StateShowApActFailureNote\n")));
    SetCurrState(KStateShowApActFailureNote);
    iStatus = KRequestPending;
    SetActive();
        
    TRequestStatus* ownStatus = &iStatus;
    User::RequestComplete(ownStatus, KErrNone);

    SetNextState(KStateAfterShowApActFailureNote);
    }

void CExtender::StateAfterShowApActFailureNote()
    {
    LOG(Log::Printf(_L("CExtender::StateAfterShowApActFailureNote\n")));
    SetCurrState(KStateAfterShowApActFailureNote);

    iNotifier.CancelNotifier(KUidVpnDialogNotifier);
    
    // Begin the combined policy update
    // and certificate enrollment process
    GotoState(KStateCheckPolicyUpdateAvailability);
    }


void CExtender::StateCheckPolicyUpdateAvailabilityL()
    {
    LOG(Log::Printf(_L("CExtender::StateCheckPolicyUpdateAvailabilityL\n")));
    SetCurrState(KStateCheckPolicyUpdateAvailability);

    LOG(Log::Printf(_L(" Doing the policy update.\n")));
 
       
    TAgileProvisionPolicy vpnPolicyName;
    User::LeaveIfError(iVpnExtServ.GetPolicyName(vpnPolicyName));
   
    if ( iPolicyId.Compare(vpnPolicyName.iPolicyName) == 0 )
       {
        SetCurrState(KStateAskUpdateConfirmation);

        TVpnDialogInfo dialogInfo(TVpnDialog::EPolicyUpdateConfirmation, 0);
        iDialogInfoDes() = dialogInfo;
       
        iNotifier.StartNotifierAndGetResponse(iStatus, KUidVpnDialogNotifier,
                                             iDialogInfoDes, iDialogResponseDes);

        SetActive();
        SetNextState(KStateAfterAskUpdateConfirmation);
        }
    else
        {
        EndTask();
        }
    }


void CExtender::StateAfterAskUpdateConfirmationL()
    {
    LOG(Log::Printf(_L("CExtender::StateAfterAskUpdateConfirmationL\n")));        
    SetCurrState(KStateAfterAskUpdateConfirmation);
    
    iNotifier.CancelNotifier(KUidVpnDialogNotifier);

    // If the user pressed Cancel or an
    // error occurred, we stop here
    User::LeaveIfError(iStatus.Int());
    
    iShowCompletionNote = ETrue;
    GotoState(KStateUpdatePolicy);
    }
    
void CExtender::StateUpdatePolicyL()
    {
    LOG(Log::Printf(_L("CExtender::StateUpdatePolicy\n")));
    SetCurrState(KStateUpdatePolicy);
    
   if ( iExtenderHelper == NULL)
       iExtenderHelper=CExtenderHelper::NewL();
   
    iExtenderHelper->StartNotifierL(this);
   
        
    iVpnExtServ.SynchronizePolicyServer(iStatus);
    
    SetActive();
    SetNextState(KStateAfterUpdatePolicy);
    }

void CExtender::StateAfterUpdatePolicyL()
    {
    LOG(Log::Printf(_L("CExtender::StateAfterUpdatePolicyL\n")));    
  
    
    if (iStatus != KErrNone)
        {
        User::Leave(iStatus.Int());
        }

    ProcessComplete(KErrNone);
    }

void CExtender::ProcessComplete(TInt aStatus)
    {
    LOG(Log::Printf(_L("CExtender::ProcessComplete - status = %d\n"), aStatus));        
    iFinalStatus = aStatus;
    GotoState(KStateShowUpdateCompleteNote);
    }

void CExtender::StateShowUpdateCompleteNote()
    {
    LOG(Log::Printf(_L("CExtender::StateShowUpdateCompleteNote\n")));    
    SetCurrState(KStateShowUpdateCompleteNote);

    if (!iShowCompletionNote || iFinalStatus == KErrCancel)
        {
         EndTask();
        }
    else if ( iFinalStatus == KErrNone )
        {
               
        TVpnDialogInfo dialogInfo(TNoteDialog::EInfo, TVpnNoteDialog::EPolicyUpdateSucceeded);
                                iDialogInfoDes() = dialogInfo;
                                
        iNotifier.StartNotifierAndGetResponse(iStatus, KUidVpnDialogNotifier,
                                           iDialogInfoDes, iDialogResponseDes);

        SetActive();
        SetNextState(KStateAfterShowUpdateCompleteNote);
        
        }
    else
        {
        TVpnDialogInfo dialogInfo(TNoteDialog::EInfo, TVpnNoteDialog::EPolicyUpdateFailed);
                iDialogInfoDes() = dialogInfo;
        
        iNotifier.StartNotifierAndGetResponse(iStatus, KUidVpnDialogNotifier,
                                          iDialogInfoDes, iDialogResponseDes);

        SetActive();
        SetNextState(KStateAfterShowUpdateCompleteNote);
        }
    
    
    }

void CExtender::StateAfterShowUpdateCompleteNote()
    {
    LOG(Log::Printf(_L("CExtender::StateAfterShowUpdateCompleteNote\n")));    
    SetCurrState(KStateAfterShowUpdateCompleteNote);
    
    iNotifier.CancelNotifier(KUidVpnDialogNotifier);
    
    EndTask();
    }

TCmSettingSelection CExtender::GetPolicyServerSelectionL()
    {

   // Get policy server details
    // from the Policy Provision server configuration
    TAgileProvisionApiServerSettings vpnPolicyServerDetails;
    User::LeaveIfError(iVpnExtServ.ServerDetails(vpnPolicyServerDetails));

    
    // Return the server selection
    return vpnPolicyServerDetails.iSelection;
    }
    
void CExtender::EndTask()
    {
    LOG(Log::Printf(_L("CExtender::EndTask\n")));    
    // Allow the deletion of the CVpnConStarter task handler
    iVpnConnStarter->SetDelayedTaskEnd(EFalse);

    // We're done and the enclosing connection starter
    // object is now free to go (causing the deletion
    // of this extender object as well)
    iVpnConnStarter->TaskDone();
    }
