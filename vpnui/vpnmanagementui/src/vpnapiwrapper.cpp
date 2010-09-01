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

#include <cmmanager.h>
#include <cmdestination.h>
#include <cmpluginvpndef.h>
#include "vpnapiwrapper.h"

#include "log_vpnmanagementui.h"

using namespace CMManager;

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
    LOG_("CVpnApiWrapper::~CVpnApiWrapper() called\n");
    delete iPolicyList;
    Cancel();

    iVpnExtApi.Close();
    }

void CVpnApiWrapper::DoCancel()
    {
    LOG_("CVpnApiWrapper::DoCancel() called\n");

    TInt i=iVpnExtApi.CancelPolicyProvision();

    iOngoingTask = ETaskNone;
    }

void CVpnApiWrapper::RunL()
    {
    LOG_1("CVpnApiWrapper::RunL():%d", iStatus.Int());
       
    if (iOngoingTask == ETaskSynchroniseServer)
       {
        iCaller->NotifySynchroniseServerCompleteL(iStatus.Int());
       }
    
    iOngoingTask = ETaskNone;
    }

void CVpnApiWrapper::ConstructL()
    {
    LOG_("CVpnApiWrapper::ConstructL() - begin");

    CActiveScheduler::Add(this);

    User::LeaveIfError(iVpnExtApi.Connect());
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

    User::LeaveIfError(iVpnExtApi.GetPolicyInfoList(iPolicyList));
    
    TKeyArrayFix Key( _FOFF(TVpnPolicyInfo,iName), ECmpCollated );
    Key.SetPtr( iPolicyList );
    iPolicyList->Sort( Key );
    
    LOG_("CVpnApiWrapper::BuildPolicyListL() - end");
	}

void CVpnApiWrapper::DeletePolicyL(TInt aPolicyIndex)
    {
    LOG_("CVpnApiWrapper::DeletePolicyL() - begin");

    TVpnPolicyId& policyToDelete = iPolicyList->At(aPolicyIndex).iId;
    User::LeaveIfError(iVpnExtApi.DeletePolicy(policyToDelete));
    
    //Delete all VPN APs pointing deleted policy
    DeleteReferringVpnAps(policyToDelete);
    
    LOG_("CVpnApiWrapper::DeletePolicyL() - end");
    }

void CVpnApiWrapper::GetPolicyDetailsL(TInt aPolicyIndex, TVpnPolicyDetails& aPolicyDetails)
    {
    LOG_("CVpnApiWrapper::GetPolicyDetailsL() - begin");

    TVpnPolicyId& policyOfInterest = iPolicyList->At(aPolicyIndex).iId;
    User::LeaveIfError(iVpnExtApi.GetPolicyDetails(policyOfInterest, aPolicyDetails));

    LOG_("CVpnApiWrapper::GetPolicyDetailsL() - end");
    }



TInt CVpnApiWrapper::CreateServer( const TAgileProvisionApiServerSettings& aServerDetails)
    {
    
    iPolicyServer.iServerNameLocal = aServerDetails.iServerNameLocal;
    iPolicyServer.iServerUrl = aServerDetails.iServerUrl;
    iPolicyServer.iSelection = aServerDetails.iSelection;
  
    return iVpnExtApi.CreateServer(iPolicyServer);
    }

TAgileProvisionApiServerListElem& CVpnApiWrapper::ServerListL()
    {
    TInt err(iVpnExtApi.ServerListL( iVpnPolicyServerList ));
    User::LeaveIfError( err );    
    
    return iVpnPolicyServerList;
    
    }

void CVpnApiWrapper::GetServerDetailsL( TAgileProvisionApiServerSettings& aServerDetails )
    {
    User::LeaveIfError( iVpnExtApi.ServerDetails(aServerDetails));
    }

TInt CVpnApiWrapper::DeleteServer()
    {
 
    return iVpnExtApi.DeleteServer();

    }

void CVpnApiWrapper::SynchroniseServerL( MVpnApiWrapperCaller* aCaller )
    {
    iCaller=aCaller;
    iVpnExtApi.SynchronizePolicyServer(iStatus);
    SetActive();
    iOngoingTask = ETaskSynchroniseServer;
    }

void CVpnApiWrapper::CancelSynchronise()
    {
   
    Cancel();
  
    }

void CVpnApiWrapper::DeleteReferringVpnAps(const TVpnPolicyId& aPolicyId) const
    {
    TRAP_IGNORE(DeleteReferringVpnApsL(aPolicyId));
    }

void CVpnApiWrapper::DeleteReferringVpnApsL(const TVpnPolicyId& aPolicyId) const
    {
    RCmManager cmManager;
    cmManager.OpenLC();
                                      
    //First collect all VPN connection methods from destinations
    RArray<TUint32> destinationArray;    
    cmManager.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);    
    
    for (TInt i = 0; i < destinationArray.Count(); ++i)
        {
        RCmDestination destination = cmManager.DestinationL( destinationArray[i] );
        CleanupClosePushL(destination);
        
        TInt connectionMethodCount = destination.ConnectionMethodCount();
        for (TInt j = connectionMethodCount - 1; j >= 0; --j)
            {
            RCmConnectionMethod connectionMethod = destination.ConnectionMethodL( j );  
            CleanupClosePushL(connectionMethod);
            
            if ( connectionMethod.GetBoolAttributeL(ECmVirtual) &&
                 connectionMethod.GetIntAttributeL( ECmBearerType ) == KPluginVPNBearerTypeUid)
                {
                HBufC* policyId = connectionMethod.GetStringAttributeL( EVpnServicePolicy );
                CleanupStack::PushL(policyId);
                if (policyId->Compare(aPolicyId) == 0)
                    {
                    destination.DeleteConnectionMethodL( connectionMethod );
                    destination.UpdateL();
                    }                    
                CleanupStack::PopAndDestroy(policyId);
                }
            CleanupStack::PopAndDestroy(); //connectionMethod       
            }
        
        CleanupStack::PopAndDestroy(); //destination
        }
    CleanupStack::PopAndDestroy(); //destinationArray    
    
    //Second collect VPN connection methods, which are not inside a destination.    
    RArray<TUint32> connectionMethodArray;    
    cmManager.ConnectionMethodL( connectionMethodArray );
    CleanupClosePushL(connectionMethodArray);
    
    for ( TInt i = 0; i < connectionMethodArray.Count(); ++i)
        {
        RCmConnectionMethod connectionMethod = 
                cmManager.ConnectionMethodL( connectionMethodArray[i] );
        CleanupClosePushL(connectionMethod);
        if ( connectionMethod.GetBoolAttributeL(ECmVirtual) &&
             connectionMethod.GetIntAttributeL( ECmBearerType ) == KPluginVPNBearerTypeUid)
            {
            HBufC* policyId = connectionMethod.GetStringAttributeL( EVpnServicePolicy );
            CleanupStack::PushL(policyId);
            if (policyId->Compare(aPolicyId) == 0)
                {
                connectionMethod.DeleteL();
                connectionMethod.UpdateL();
                }                    
            CleanupStack::PopAndDestroy(policyId);
            }

        CleanupStack::PopAndDestroy(); //connectionMethod               
        }    
    CleanupStack::PopAndDestroy(); //connectionMethodArray
    
    CleanupStack::PopAndDestroy(); //cmManager
    }
/***/
