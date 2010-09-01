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
* Description:  VPN Ext API
*
*/


#include "vpnextapi.h"
#include "vpnextapiservantdefs.h"
#include "vpnmanagerserverdefs.h"

EXPORT_C RVpnExtApi::RVpnExtApi() : RVpnServ()
    {    
    }


EXPORT_C TInt RVpnExtApi::CreateServer( const TAgileProvisionApiServerSettings& aServerDetails )
    {
    TPckg<TAgileProvisionApiServerSettings> serverCreatePckg(aServerDetails);
    return SendReceive(EExtCreateProvisionServer, TIpcArgs(&serverCreatePckg));
    }


EXPORT_C TInt RVpnExtApi::ServerDetails( TAgileProvisionApiServerSettings& aServerDetails )
    {
    TPckg<TAgileProvisionApiServerSettings> serverPckg( aServerDetails );
   
    return SendReceive(EExtVPNPolicyServerDetails, TIpcArgs(&serverPckg));
    }

EXPORT_C TInt RVpnExtApi::DeleteServer( )
    {
    return SendReceive(EExtDeletePolicyServer);
    }

EXPORT_C TInt RVpnExtApi::ServerListL(  TAgileProvisionApiServerListElem& aVpnPolicyServerList )
    {
    TPckg<TAgileProvisionApiServerListElem> serverPckg( aVpnPolicyServerList );
    return SendReceive(EExtVPNPolicyServerList, TIpcArgs(&serverPckg));
    }

EXPORT_C void RVpnExtApi::SynchronizePolicyServer( TRequestStatus& aStatus )
    {
    SendReceive(EExtSynchronizePolicyServer, aStatus);
    }

EXPORT_C TInt RVpnExtApi::CancelPolicyProvision( )
    {
    return SendReceive(EExtCancelSynchronize);
    }

EXPORT_C TInt RVpnExtApi::GetPolicyName(TAgileProvisionPolicy& aVpnPolicyName)
    {
    TPckg<TAgileProvisionPolicy> serverPckg( aVpnPolicyName );
    return SendReceive(EExtGetPolicyName, TIpcArgs(&serverPckg));
    }

