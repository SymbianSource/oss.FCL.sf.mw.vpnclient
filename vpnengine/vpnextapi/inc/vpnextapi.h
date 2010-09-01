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

#ifndef R_VPNEXTAPI_H
#define R_VPNEXTAPI_H

// INCLUDES
#include <e32base.h>
#include <vpnapi.h>
#include <cmapplicationsettingsui.h>
#include "vpnextapiservantdefs.h"



class TAgileProvisionApiServerSettings

   {
   public:
       TCmSettingSelection             iSelection;
       TBuf8<KMaxProvisionServerUrlLength>   iServerUrl;
       TBuf<KMaxProvisionServerNameLength> iServerNameLocal;
   };

   class TAgileProvisionApiServerListElem

   {
   public:
       TBuf8<KMaxProvisionServerUrlLength>   iServerUrl;
       TBuf<KMaxProvisionServerNameLength> iServerNameLocal;
   };

   class TAgileProvisionPolicy
   
   {
   public:
       TBuf<KMaxNameLength> iPolicyName;
   };
  
/**
 * VPN Ext API.
 *
 * VPN Ext API allows clients to manage VPN policies. VPN Ext API provides
 * extended functionality to VPN API.
 */
class RVpnExtApi : public RVpnServ
    {    
public:
    
   
    
    
    /**
     * Constructor
     */
    IMPORT_C RVpnExtApi();
    

    
    IMPORT_C TInt CreateServer( const TAgileProvisionApiServerSettings& aServerDetails );
    IMPORT_C TInt ServerDetails( TAgileProvisionApiServerSettings& aServerDetails );
    IMPORT_C TInt ServerListL( TAgileProvisionApiServerListElem& aVpnPolicyServerList );
    IMPORT_C TInt DeleteServer( );
    IMPORT_C void SynchronizePolicyServer( TRequestStatus& aStatus ); 
    IMPORT_C TInt CancelPolicyProvision();
    IMPORT_C TInt GetPolicyName(TAgileProvisionPolicy& aVpnPolicyName);
private:
        
    };

#endif // R_VPNEXTAPI_H
