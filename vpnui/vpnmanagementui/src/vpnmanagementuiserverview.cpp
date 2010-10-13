/*
* Copyright (c) 2003 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Handles commands given through the UI for maintaining policy servers.
*
*/



// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>            // for warning & information notes
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h" 
#include "vpnmanagementuiserverview.h"
#include "vpnmanagementuiservercontainer.h" 
#include "vpnmanagementui.hrh"
#ifdef __SERIES60_HELP
#include    <hlplch.h>   // For HlpLauncher 
#endif //__SERIES60_HELP

/** MSK control Id. */
const TInt KVpnMSKControlId = 3;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiServerView::CVpnManagementUiServerView(CVpnUiLoader& aLoader)
// : iLoader(aLoader)
// Constructor
// ---------------------------------------------------------
//
CVpnManagementUiServerView::CVpnManagementUiServerView( CVpnUiLoader& aLoader )
: iLoader(aLoader)
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::~CVpnManagementUiServerView()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiServerView::~CVpnManagementUiServerView()
    {
    if ( iServerContainer )
        {
        AppUi()->RemoveFromViewStack(*this, iServerContainer);
        delete iServerContainer;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView* CVpnManagementUiServerView::NewL(
//      const TRect& /*aRect*/, CVpnUiLoader& aLoader)
// ---------------------------------------------------------
//
CVpnManagementUiServerView* CVpnManagementUiServerView::NewL(
    const TRect& /*aRect*/, CVpnUiLoader& aLoader )
    {
    CVpnManagementUiServerView* self = 
        new ( ELeave ) CVpnManagementUiServerView( aLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView* CVpnManagementUiServerView::NewLC(
//  const TRect& /*aRect*/, CVpnUiLoader& aLoader)
// ---------------------------------------------------------
//
CVpnManagementUiServerView* CVpnManagementUiServerView::NewLC(
    const TRect& /*aRect*/, CVpnUiLoader& aLoader )
    {
    CVpnManagementUiServerView* self = 
        new ( ELeave ) CVpnManagementUiServerView( aLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::ConstructL()
// CVpnUiView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiServerView::ConstructL() 
    {
    BaseConstructL( R_VPNUI_POLICY_SERVERS_VIEW );
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::DynInitMenuPaneL(
//      TInt aResourceId,CEikMenuPane* aMenuPane)
// Updates Options list with correct items depending on 
// whether the listbox is empty or if it has any marked items
// ---------------------------------------------------------
//
void CVpnManagementUiServerView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
  
    if ( aResourceId == R_VPNUI_POLICY_SERVERS_VIEW_MENU )
        {
        TInt currentItem = iServerContainer->iListBox->CurrentItemIndex();

        // the policy server list is empty  all except Add new server, Help and Exit is dimmed
        if ( currentItem == -1 )
            {
            aMenuPane->SetItemDimmed( EVpnUiCmdEditServer, ETrue );
            aMenuPane->SetItemDimmed( EVpnUiCmdSynchronise, ETrue );
            aMenuPane->SetItemDimmed( EVpnUiCmdDeleteServer, ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::Id() const
// Returns Authority view id
// ---------------------------------------------------------
//
TUid CVpnManagementUiServerView::Id() const
    {
    return KVpnManagementUiServerViewId;
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::HandleCommandL(TInt aCommand)
// Handles Softkey and Options list commands
// ---------------------------------------------------------
//
void CVpnManagementUiServerView::HandleCommandL( TInt aCommand )
    {
    TBool ffsLow;
    
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            iLoader.ChangeViewL( KChangeViewPrevious );
            break;
            }
        case EAknCmdExit:
            {
            ((CAknViewAppUi*)iAvkonAppUi)->HandleCommandL( EAknCmdExit );
            break;
            }
        
        case EVpnUiCmdEditServer:
            {
            ffsLow = iLoader.FFSSpaceBelowCriticalLevelL( ETrue, 0 );
            if (!ffsLow) 
                {
                iCurrentPosition = iServerContainer->iListBox->CurrentItemIndex();
                iLoader.ChangeViewL(KChangeViewSettings, iCurrentPosition);
                }
            break;
            }

        case EVpnUiCmdAddServer:
            {
            ffsLow = iLoader.FFSSpaceBelowCriticalLevelL( ETrue, 0 );
    		if(!ffsLow)
    			{
            	iLoader.ChangeViewL(KChangeViewSettings);
    			}
            break;
            }
        case EVpnUiCmdSynchronise:
            {
            ffsLow = iLoader.FFSSpaceBelowCriticalLevelL( ETrue, 0 );
    		if(!ffsLow)
    			{
    			iCurrentPosition = iServerContainer->iListBox->CurrentItemIndex();
            	iServerContainer->SynchroniseServerL(iCurrentPosition);
    			}
    		break;
            }

        case EVpnUiCmdDeleteServer:
            {
            iCurrentPosition = iServerContainer->iListBox->CurrentItemIndex();
            iServerContainer->DeleteServerL(iCurrentPosition);
            iCurrentPosition = iServerContainer->iListBox->CurrentItemIndex();
            break;
            }

#ifdef __SERIES60_HELP
        case EAknCmdHelp: 
            {
            HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
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
// CVpnManagementUiServerView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CVpnManagementUiServerView::HandleClientRectChange()
    {
    if ( iServerContainer )
        {
        iServerContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
//      TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
// Updates the view when opening it
// ---------------------------------------------------------
//
void CVpnManagementUiServerView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if ( iServerContainer )
        {
        ((CAknViewAppUi*)iAvkonAppUi)->RemoveFromViewStack( 
            *this, iServerContainer );
        }
    iServerContainer = new (ELeave) CVpnManagementUiServerContainer( 
        *this, iLoader );

    iServerContainer->SetMopParent( this );
    iServerContainer->ConstructL( 
        ClientRect(), iCurrentPosition, iTopItem );
    iLoader.ActivateTitleL( KViewTitleServerView );
    iLoader.PushDefaultNaviPaneL();

    ((CAknViewAppUi*)iAvkonAppUi)->AddToStackL( *this, iServerContainer );
    iServerContainer->DrawListBoxL( iCurrentPosition, iTopItem );
    
    // Add MiddleSoftKey
 	SetMiddleSoftKeyL(EFalse);   
    }

void CVpnManagementUiServerView::SetMiddleSoftKeyL(TBool aDrawNow)
    {
    if (iServerContainer->iListItemCount > 0)
    	{ 
		SetMiddleSoftKeyLabelL(R_MSK_EDIT, EVpnUiCmdEditServer);
    	}
    else
    	{
    	SetMiddleSoftKeyLabelL(R_MSK_ADD_VPN_SERVER, EVpnUiCmdAddServer);
    	}

    if (aDrawNow)
    	{
    	if (Cba())
    		Cba()->DrawNow();
    	}
	}
    	

void CVpnManagementUiServerView::SetMiddleSoftKeyLabelL(TInt aResourceId, TInt aCommandId)
    {
    	CEikButtonGroupContainer* cbaGroup = Cba();
    	if (cbaGroup)
	    	{
			cbaGroup->RemoveCommandFromStack(KVpnMSKControlId, EVpnUiCmdEditServer);
			cbaGroup->RemoveCommandFromStack(KVpnMSKControlId, EVpnUiCmdAddServer);
		    HBufC* text = StringLoader::LoadLC(aResourceId); 
    		cbaGroup->AddCommandToStackL(KVpnMSKControlId, aCommandId, text->Des());
    		CleanupStack::PopAndDestroy(text);
	    	}
    }


// ---------------------------------------------------------
// CVpnManagementUiServerView::DoDeactivate()
// Saves focus position when closing view
// ---------------------------------------------------------
//
void CVpnManagementUiServerView::DoDeactivate()
    {
    if ( iServerContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iServerContainer );
        
        delete iServerContainer;
        iServerContainer = NULL;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiServerView::Container()
// Returns iServerContainer
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiServerView::Container()
    {
    return iServerContainer;
    }

// End of File

