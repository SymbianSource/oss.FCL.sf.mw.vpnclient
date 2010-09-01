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
* Description: The container is responsible of showing policy server details. 
*
*/

// INCLUDE FILES
#include <akntextsettingpage.h>
#include <aknnotewrappers.h>
#include <cmmanagerext.h>
#include <cmsettingsui.h>
#include <cmapplicationsettingsui.h>
#include <cmdestinationext.h>
#include <cmpluginvpndef.h>
#include <commdb.h>
#include <e32def.h>

#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h"
#include "vpnmanagementuiserversettingscontainer.h"
#include "vpnmanagementuidefs.h"
#include "serversettingconnectionsettingitem.h"
#include "vpnmanagementui.hrh"
#include "log_vpnmanagementui.h"


#ifdef __SERIES60_HELP
#include <csxhelp/vpn.hlp.hrh> // for help context of VPN Management UI
#endif //__SERIES60_HELP

using namespace CMManager;

// ================= MEMBER FUNCTIONS =======================

CServerSettingsContainer* CServerSettingsContainer::NewL(
    CVpnUiLoader& aLoader, TInt aServerIndex )
    {
    CServerSettingsContainer* self = new(ELeave) CServerSettingsContainer(
        aLoader, aServerIndex );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;
    }

// ---------------------------------------------------------
// CServerSettingsContainer::CServerSettingsContainer()
// ---------------------------------------------------------
//
CServerSettingsContainer::CServerSettingsContainer(
    CVpnUiLoader& aLoader, TInt aServerIndex )
    :iLoader(aLoader), iServerIndex(aServerIndex)
    {
    ASSERT(aServerIndex >= KErrNotFound);
	}

// ---------------------------------------------------------
// CServerSettingsContainer::~CServerSettingsContainer()
// Destructor
// ---------------------------------------------------------
//
CServerSettingsContainer::~CServerSettingsContainer()
    {
    }


// ---------------------------------------------------------------------------
// CreateSettingItemL
// ---------------------------------------------------------------------------
//
CAknSettingItem* CServerSettingsContainer::CreateSettingItemL( 
    TInt aSettingId )
    {
    CAknSettingItem* settingItem(NULL);
    
    switch (aSettingId)
        {
        case EVpnUiSettingServerName:
            settingItem = new (ELeave) CAknTextSettingItem(
                aSettingId, iServerDetails.iServerNameLocal );
            break;
        case EVpnUiSettingServerAddress:
            if (iServerDetails.iServerUrl.Length() > 0)
                {                
                iServerAddressBuffer.Copy(iServerDetails.iServerUrl);
                }
            settingItem = new (ELeave) CAknTextSettingItem(
                aSettingId, iServerAddressBuffer );
            break;
        case EVpnUiSettingIap:              
            if ( iServerDetails.iSelection.iId <1 )
                {
                 RCmManagerExt cmManagerExt;
                 cmManagerExt.OpenL();        
                 CleanupClosePushL( cmManagerExt );     

                 //Makes sure that Internet Destination Exists
                 RArray<TUint32> destinationArray;    
                 cmManagerExt.AllDestinationsL( destinationArray );
                 CleanupClosePushL(destinationArray);    
                            
                 TUint32 internetDestinationId = 0;
                 for (TInt i = 0; i < destinationArray.Count(); ++i)
                     {
                      RCmDestinationExt destination = cmManagerExt.DestinationL( destinationArray[i] );
                      CleanupClosePushL(destination);
                            
                      TUint32 purposeMetaData = destination.MetadataL( ESnapMetadataPurpose );
                      if ( ESnapPurposeInternet ==  purposeMetaData )
                           {
                            internetDestinationId = destinationArray[i];
                            CleanupStack::PopAndDestroy(); //destination
                            break;
                           }                
                      CleanupStack::PopAndDestroy(); //destination
                      }
                              
                iServerDetails.iSelection.iId = internetDestinationId;
                
                iServerDetails.iSelection.iResult=EDestination;
                
                
                settingItem = new (ELeave) CServerSettingConnectionSettingItem(
                        aSettingId, iServerDetails.iSelection);
                CleanupStack::PopAndDestroy(2); //destinationArray,cmManagerExt
                
                }
            else
                settingItem = new (ELeave) CServerSettingConnectionSettingItem(
                        aSettingId, iServerDetails.iSelection);
            break;
        default:
            // Do nothing
            break;
            }
   
    return settingItem;
    }
    
    
// ---------------------------------------------------------------------------
// CServerSettingsContainer::ConstructL()
// ---------------------------------------------------------------------------
//
void CServerSettingsContainer::ConstructL()
    {
	// Server settings view sets iServerIndex to -1 when creating a new 
	// server
	if ( iServerIndex >= 0 )
	    {
	    iLoader.VpnApiWrapperL().GetServerDetailsL( iServerDetails );
	    UpdateTitleL( iServerDetails.iServerNameLocal );    
	    }    
	ConstructFromResourceL(R_VPN_SERVER_SETTING_LIST);	
	CAknSettingItem* item = SettingItemArray()->At(EVpnUiSettingIap); 
	
	using namespace CMManager;
	    
	RCmManagerExt cmManagerExt;
	cmManagerExt.OpenL();        
	CleanupClosePushL( cmManagerExt );     

	//Makes sure that Internet Destination Exists
	RArray<TUint32> destinationArray;    
	cmManagerExt.AllDestinationsL( destinationArray );
	CleanupClosePushL(destinationArray);    
	TBool internetIapExist=EFalse;       
	TUint32 internetDestinationId = 0;
	for (TInt i = 0; i < destinationArray.Count(); ++i)
	        {
	        RCmDestinationExt destination = cmManagerExt.DestinationL( destinationArray[i] );
	        CleanupClosePushL(destination);
	        
	        TUint32 purposeMetaData = destination.MetadataL( ESnapMetadataPurpose );
	        if ( ESnapPurposeInternet ==  purposeMetaData )
	            {
	            internetDestinationId = destinationArray[i];
	            CleanupStack::PopAndDestroy(); //destination
	            internetIapExist=ETrue;
	            break;
	            }                
	        CleanupStack::PopAndDestroy(); //destination
	        }
	
	if ( internetIapExist != EFalse)
	    {
        HBufC* defaultConn = GetDestinationNameL(internetDestinationId);
        CleanupStack::PushL(defaultConn);
        item->SetEmptyItemTextL(*defaultConn);
        item->LoadL();
        item->UpdateListBoxTextL();
        CleanupStack::PopAndDestroy(defaultConn); 
	    }
	
	CleanupStack::PopAndDestroy(); //destinationArray 
	CleanupStack::PopAndDestroy(); //cmManagerExt 
	}
	
	
// ---------------------------------------------------------------------------
// UpdateTitleL
// ---------------------------------------------------------------------------
//
void CServerSettingsContainer::UpdateTitleL( TDes& aText )
    {
    iLoader.ActivateTitleL(KViewTitleParametersView,aText);
    } 


void CServerSettingsContainer::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed: 
        case EEventItemSingleClicked:
            ChangeSettingValueL();            
            break;
        default:
            CAknSettingItemList::HandleListBoxEventL(aListBox, aEventType);
        }    
    }


// ---------------------------------------------------------------------------
// ChangeSettingValueL
// ---------------------------------------------------------------------------
//
void CServerSettingsContainer::ChangeSettingValueL()
    {
  
    TInt currentItem( ListBox()->CurrentItemIndex() );
           
    switch(currentItem)
        {        
        case EVpnUiSettingServerName:
            {
            EditItemL(EVpnUiSettingServerName, ETrue);   
            SettingItemArray()->At(EVpnUiSettingServerName)->StoreL(); 
            }
            if (iServerDetails.iServerNameLocal.Length() > 0)
            {                
            UpdateTitleL( iServerDetails.iServerNameLocal );
            }
            
            break;            
        case EVpnUiSettingServerAddress:
            {
            EditItemL(EVpnUiSettingServerAddress, ETrue);    
            }
            break;
        case EVpnUiSettingIap:
            {
            EditItemL(EVpnUiSettingIap, ETrue);    
            }
            break;
        default:            
            User::Invariant();
            break;
        }
    }



const TAgileProvisionApiServerSettings& CServerSettingsContainer::ServerDetailsL()
    {
    StoreSettingsL();
    if (iServerAddressBuffer.Length() > 0)
        {
        iServerDetails.iServerUrl.Copy(iServerAddressBuffer);
        }

    //If server name is left empty, but we have an address,
    //set address as a server name.        
    if (iServerDetails.iServerNameLocal.Length() == 0 &&
        iServerDetails.iServerUrl.Length() > 0)
        {         

        //If the address is already in use as server name, generate
        //a new unique name.                
        TUint16 i = 1;
            static const TInt KSuffixLength = 10;
            _LIT(KSuffixFormat, "(%d)");
                        
            TBuf<KSuffixLength> suffix;                            
            suffix.Zero();
            if (i > 1)
                {
                suffix.Format(KSuffixFormat, i);                
                }
            i++;
            
            //First assumes that the address is too long to fit
            TInt numberOfCharactersCopiedFromAddress =
                          iServerDetails.iServerNameLocal.MaxLength() - suffix.Length();         

            //If this is not the case adjust the length                           
            if (numberOfCharactersCopiedFromAddress > iServerDetails.iServerUrl.Length())
                {
                numberOfCharactersCopiedFromAddress = iServerDetails.iServerUrl.Length();
                }
            
            TPtrC8 serverNameAddress = iServerDetails.iServerUrl.Left(numberOfCharactersCopiedFromAddress);
            iServerDetails.iServerNameLocal.Copy(serverNameAddress);             
            iServerDetails.iServerNameLocal.Append(suffix);                                                             
   
        CAknSettingItem* item = SettingItemArray()->At(EVpnUiSettingServerName); 
        
        item->LoadL();
        item->UpdateListBoxTextL();        
        }
    return iServerDetails;
    }


// ---------------------------------------------------------------------------
// GetIapNameL
// ---------------------------------------------------------------------------
//
HBufC* CServerSettingsContainer::GetDestinationNameL( TUint aId )
	{
    RCmManagerExt cmManager;    
	cmManager.OpenLC();
	RCmDestinationExt dest = cmManager.DestinationL( aId );
	CleanupClosePushL( dest );
	HBufC* name = dest.NameLC();   
	CleanupStack::Pop(name);
	CleanupStack::PopAndDestroy( 2 ); // dest, cmManager
	return name;
	}


HBufC* CServerSettingsContainer::GetConnectionMethodNameL( TUint aId )
	{
	RCmManagerExt cmManager;    
	cmManager.OpenLC();
	RCmConnectionMethodExt conn = cmManager.ConnectionMethodL( aId );
	CleanupClosePushL( conn );	
	HBufC* name = conn.GetStringAttributeL( ECmName );
	CleanupStack::PopAndDestroy( 2 ); // conn, cmManager
	return name;
	}


TInt CServerSettingsContainer::ServerIndex() const
    {
    return iServerIndex;
    }


// ---------------------------------------------------------
// CServerSettingsContainer::GetHelpContext
// ---------------------------------------------------------

#ifdef __SERIES60_HELP
void CServerSettingsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
	aContext.iMajor = KUidVpnManagementUi;
    aContext.iContext = KSET_HLP_VPN_POLICY_SERVER_SET;
    }
#endif //__SERIES60_HELP


// End of File  
