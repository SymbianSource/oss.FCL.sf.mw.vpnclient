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
* Description: 
* Handles commands given through the UI for maintaining information 
* of single policy server.
*
*/

// INCLUDE FILES
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <vpnmanagementuirsc.rsg>
#include <ErrorUI.h>
#include <sysutil.h>
#include "vpnuiloader.h" 
#include "vpnmanagementuiserversettingsview.h"
#include "vpnmanagementuiserversettingscontainer.h" 
#include "vpnmanagementui.hrh"

#ifdef __SERIES60_HELP
#include    <hlplch.h>   // For HlpLauncher 
#endif //__SERIES60_HELP

/** MSK control Id. */
const TInt KVpnMSKControlId = 3;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CServerSettingsView::CServerSettingsView(CVpnUiLoader& aLoader)
// Constructor
// ---------------------------------------------------------
//
CServerSettingsView::CServerSettingsView( CVpnUiLoader& aLoader )
    : iLoader( aLoader )
    {
	}

// ---------------------------------------------------------
// CServerSettingsView::~CServerSettingsView()
// Destructor
// ---------------------------------------------------------
//
CServerSettingsView::~CServerSettingsView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
	    delete iContainer;
        }
	}

// ---------------------------------------------------------
// CServerSettingsView* CServerSettingsView::NewL(const TRect& /*aRect*/, 
//      CVpnUiLoader& aLoader)
// ---------------------------------------------------------
//
CServerSettingsView* CServerSettingsView::NewL(
    const TRect& /*aRect*/, CVpnUiLoader& aLoader)
    {
    CServerSettingsView* self = new (ELeave) CServerSettingsView(aLoader);
 	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CServerSettingsView* CServerSettingsView::NewLC(const TRect& /*aRect*/, 
//	    CVpnUiLoader& aLoader)
// ---------------------------------------------------------
//
CServerSettingsView* CServerSettingsView::NewLC(const TRect& /*aRect*/, 
    CVpnUiLoader& aLoader)
    {
    CServerSettingsView* self = new (ELeave) CServerSettingsView(aLoader);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CServerSettingsView::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CServerSettingsView::ConstructL()
    {
	BaseConstructL( R_VPNMANAGEMENTUI_SERVER_PARAMETERS_VIEW );
	}


// ---------------------------------------------------------
// CServerSettingsView::Id() const
// Returns Trust view id
// ---------------------------------------------------------
//
TUid CServerSettingsView::Id() const
    {
    return KVpnManagementUiParametersViewId;
    }

// ---------------------------------------------------------
// CServerSettingsView::HandleCommandL(TInt aCommand)
// Handles Softkey and Options list commands
// ---------------------------------------------------------
//
void CServerSettingsView::HandleCommandL( TInt aCommand)
    {
   
    TBool ffs(EFalse);
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {            
            //GET CURRENT ADDRESS
            ASSERT( iContainer );
            
            const TAgileProvisionApiServerSettings& serverDetails = iContainer->ServerDetailsL();

            ffs = iLoader.FFSSpaceBelowCriticalLevelL( ETrue, 0 );
            if (ffs)
    	        {
                iLoader.iBackFromServerDefinition = ETrue;
   			    iLoader.ChangeViewL( KChangeViewPrevious );
		        }
            else if(serverDetails.iServerUrl.Length()==0 || serverDetails.iSelection.iId==0)
                {
                TInt ret = CompleteSettingsL();
                if (ret)
                    {
                    iLoader.iShowDefineQuery = EFalse;
                    iLoader.iBackFromServerDefinition = ETrue;
       			    iLoader.ChangeViewL( KChangeViewPrevious );
                    }
                }
            else
                {
              
	                        int aResult = iLoader.VpnApiWrapperL().CreateServer(serverDetails);
	                        if (aResult == KErrNone)
		                        {
		                    
		                        
		                        iLoader.iNewServerDefinition = ETrue;
		                        }
		                    else
		                    	{
			                    iLoader.iShowDefineQuery = EFalse;
			                    iLoader.iBackFromServerDefinition = ETrue;
	                    		iLoader.iNewServerDefinition = EFalse;

						    	CErrorUI* errorUi = CErrorUI::NewLC( *(CCoeEnv::Static()) );
						        errorUi->ShowGlobalErrorNoteL( aResult );
						        CleanupStack::PopAndDestroy();  // errorUi
		                    	}
           
                    }

			    iLoader.ChangeViewL( KChangeViewPrevious );
                break;
            }
		case EAknCmdExit:
			{
			((CAknViewAppUi*)iAvkonAppUi)->HandleCommandL( EAknCmdExit );
			break;
			}
        case EVpnUiCmdChange:
            {
            ASSERT(iContainer);
            iContainer->ChangeSettingValueL();
            break;
            }
        #ifdef __SERIES60_HELP
        case EAknCmdHelp: 
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                AppUi()->AppHelpContextL() );
            break;
            }
        #endif //__SERIES60_HELP

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CServerSettingsView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CServerSettingsView::HandleClientRectChange()
    {
	if ( iContainer )
		{
		iContainer->SetRect( ClientRect() );
		}
    }

// ---------------------------------------------------------
// CServerSettingsView::DoActivateL()
// Updates the view when opening it
// ---------------------------------------------------------
//
void CServerSettingsView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, 
    TUid aCustomMessageId, const TDesC8& /*aCustomMessage*/)
    {    
    __ASSERT_DEBUG(iContainer == NULL, User::Invariant());
            	    
    // aCustomMessageId contains index of the policy servers listbox
	iContainer = CServerSettingsContainer::NewL( iLoader,
	    static_cast<TInt> ( aCustomMessageId.iUid ) );
	iContainer->SetMopParent( this );
    	
    iLoader.PushDefaultNaviPaneL();

	((CAknViewAppUi*)iAvkonAppUi)->AddToStackL( *this, iContainer ); 
		
    // Add MiddleSoftKey
   	CEikButtonGroupContainer* cbaGroup = Cba();
   	if (cbaGroup)
    	{
	    HBufC* text = StringLoader::LoadLC(R_MSK_CHANGE); 
   		cbaGroup->AddCommandToStackL(KVpnMSKControlId, EVpnUiCmdChange, text->Des());
   		CleanupStack::PopAndDestroy(text);
    	}
    iContainer->ActivateL();	
	}

// ---------------------------------------------------------
// CServerSettingsView::DoDeactivate()
// ---------------------------------------------------------
//
void CServerSettingsView::DoDeactivate()
    {
    if ( iContainer )
        {
		((CAknViewAppUi*)iAvkonAppUi)->RemoveFromViewStack( *this, iContainer );
	    
	    delete iContainer;
		iContainer = NULL;
		}
	}


TInt CServerSettingsView::CompleteSettingsL()
    {
    // compulsory field is empty, confirmation query
    HBufC* temp;
    temp = StringLoader::LoadLC( R_VPN_QUEST_INCOMPLETE_SERVER_DEF_DELETE );
    CAknQueryDialog* query = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
    TInt retval = query->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
    CleanupStack::PopAndDestroy();  // temp
    return retval;
    }

// End of File

