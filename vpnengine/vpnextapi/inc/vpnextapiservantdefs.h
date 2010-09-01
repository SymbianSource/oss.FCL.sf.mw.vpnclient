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
* Description:  VPN External API servant definitions
*
*/

#ifndef VPNEXTAPISERVANTDEFS_H
#define VPNEXTAPISERVANTDEFS_H

const TInt KMaxProvisionServerUrlLength = 100;
const TInt KMaxProvisionServerNameLength = 100;
const TInt KMaxPolicyServerFileNameLength = 100;

// VPN policy server settings file definition constants 
const TInt KPolicyServerAddressLine = 1; //Address of the policy server
const TInt KPolicyServerNameLine = 2; //Name of the server. Not has to be equal to real host name of server 
const TInt KPolicyServerIapIdLine = 3; // Used IAP for policy provision operation
const TInt KPolicyServerIapModeLine = 4; //Iap mode for used IAP (SNAP, AP)
const TInt KPolicyFileLine = 5; //real file name of the imported policy in private dir
const TInt KAgileProvisionVpnIapId = 6; //ID of created IAP for provisioned policy.


_LIT(KProvisionServerSettings, "vpnpolicyserver");
_LIT(KNullDesc, "");
_LIT8(KNullDesc8, "");


class TAgileProvisionServerLocals
{
public:
    TCmSettingSelection                        iSelection;
    TBuf8<KMaxProvisionServerUrlLength>        iServerAddress;
    TBuf<KMaxProvisionServerNameLength>        iServerNameLocal;
};


#endif // VPNEXTAPISERVANTDEFS_H
