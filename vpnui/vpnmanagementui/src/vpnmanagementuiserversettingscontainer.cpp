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
    TInt /* aSettingId */)
    {
    CAknSettingItem* settingItem(NULL);
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
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
            settingItem = new (ELeave) CServerSettingConnectionSettingItem(
                aSettingId, iServerDetails.iSelection);
            break;
        default:
            // Do nothing
            break;
            }
    ***/
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
	/*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
	if ( iServerIndex >= 0 )
	    {
	    iLoader.AcuApiWrapperL().GetServerDetailsL( 
	        iServerIndex, iServerDetails );
	    UpdateTitleL( iServerDetails.iServerNameLocal );    
	    }    
	***/  
    ConstructFromResourceL(R_VPN_SERVER_SETTING_LIST);	    
	}
	
	
// ---------------------------------------------------------------------------
// UpdateTitleL
// ---------------------------------------------------------------------------
//
void CServerSettingsContainer::UpdateTitleL( TDes& aText )
    {
    iLoader.ActivateTitleL(KViewTitleParametersView,aText);
    } 


// ---------------------------------------------------------------------------
// ServerNameExistsL
// ---------------------------------------------------------------------------
//
TBool CServerSettingsContainer::ServerNameExistsL( const TDesC& /* aText */) const
    {
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    const CArrayFix<TAcuApiServerListElem>* serverList = iLoader.AcuApiWrapperL().ServerListL();
    TInt count = serverList->Count();

    for (TInt i = 0; i < count; ++i)
        {
        //If we are editing name, we don't want to compare itself
        if(i != iServerIndex)
            {
            if (serverList->At(i).iServerNameLocal.Compare(aText)==0)
                {
                //Name is already in use
                return ETrue;
                }
            }
        } ***/
    return EFalse;
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
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    TInt currentItem( ListBox()->CurrentItemIndex() );
           
    switch(currentItem)
        {        
        case EVpnUiSettingServerName:
            {
            TBool alreadyInUse = ETrue;
            while(alreadyInUse)
                {
                EditItemL(EVpnUiSettingServerName, ETrue);   
                SettingItemArray()->At(EVpnUiSettingServerName)->StoreL(); 
                alreadyInUse = ServerNameExistsL( iServerDetails.iServerNameLocal );
                if(alreadyInUse)
                    {
                    //Show an information note that server is currently in use
                    HBufC* noteText;
                    noteText = StringLoader::LoadLC( R_FLDR_NAME_ALREADY_USED, iServerDetails.iServerNameLocal );
                    CAknInformationNote* note = new(ELeave)CAknInformationNote(ETrue);
                    note->SetTimeout(CAknNoteDialog::ELongTimeout); //3sec
                    note->ExecuteLD(noteText->Des());
                    CleanupStack::PopAndDestroy();  // noteText                                
                    }

                }
            if (iServerDetails.iServerNameLocal.Length() > 0)
                {                
                UpdateTitleL( iServerDetails.iServerNameLocal );
                }
            }
            break;            
        case EVpnUiSettingServerAddress:
            {
            if( iServerDetails.iServerUrlReadOnly )
                {
                //Show an information note that server cannot be
                //modified
                HBufC* noteText;
                noteText = StringLoader::LoadLC( 
                    R_VPN_INFO_CANNOT_MODIFY_SERVER_DEF );
                CAknInformationNote* note = 
                    new(ELeave) CAknInformationNote(ETrue);
                note->SetTimeout(CAknNoteDialog::ELongTimeout); //3sec
                note->ExecuteLD(noteText->Des());
                CleanupStack::PopAndDestroy();  // noteText
                }
            else
                {            
                EditItemL(EVpnUiSettingServerAddress, ETrue);    
                }
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
        } ***/
    }


// ---------------------------------------------------------------------------
// ServerDetails
// ---------------------------------------------------------------------------
//
/*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
const TAcuApiServerDetails& CServerSettingsContainer::ServerDetailsL()
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
        do
            {
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
                           
            }while(ServerNameExistsL( iServerDetails.iServerNameLocal) );                                                          
        
        CAknSettingItem* item = SettingItemArray()->At(EVpnUiSettingServerName); 
        item->LoadL();
        item->UpdateListBoxTextL();        
        }
    return iServerDetails;
    }
***/

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