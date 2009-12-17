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
* Description: Handles commands given through the UI for maintaining policies.
*
*/



// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>            // for warning & information notes
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h" 
#include "vpnmanagementuilogview.h"
#include "vpnmanagementuilogcontainer.h" 
#include "vpnmanagementui.hrh"
#ifdef __SERIES60_HELP
#include    <hlplch.h>   // For HlpLauncher 
#endif //__SERIES60_HELP

/** MSK control Id. */
const TInt KVpnMSKControlId = 3;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiLogView::CVpnManagementUiLogView(CVpnUiLoader& aLoader)
// : iLoader(aLoader)
// Constructor
// ---------------------------------------------------------
//
CVpnManagementUiLogView::CVpnManagementUiLogView( CVpnUiLoader& aLoader )
: iLoader(aLoader)
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::~CVpnManagementUiLogView()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiLogView::~CVpnManagementUiLogView()
    {
    if ( iLogContainer )
        {
        AppUi()->RemoveFromViewStack(*this, iLogContainer);
        delete iLogContainer;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView* CVpnManagementUiLogView::NewL(
//      const TRect& /*aRect*/, CVpnUiLoader& aLoader)
// ---------------------------------------------------------
//
CVpnManagementUiLogView* CVpnManagementUiLogView::NewL(
    const TRect& /*aRect*/, CVpnUiLoader& aLoader )
    {
    CVpnManagementUiLogView* self = 
        new ( ELeave ) CVpnManagementUiLogView( aLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView* CVpnManagementUiLogView::NewLC(
//  const TRect& /*aRect*/, CVpnUiLoader& aLoader)
// ---------------------------------------------------------
//
CVpnManagementUiLogView* CVpnManagementUiLogView::NewLC(
    const TRect& /*aRect*/, CVpnUiLoader& aLoader )
    {
    CVpnManagementUiLogView* self = 
        new ( ELeave ) CVpnManagementUiLogView( aLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::ConstructL()
// CVpnUiView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiLogView::ConstructL() 
    {
    BaseConstructL( R_VPNUI_LOG_VIEW );
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::DynInitMenuPaneL(
//      TInt aResourceId,CEikMenuPane* aMenuPane)
// Updates Options list with correct items depending on 
// whether the listbox is empty or if it has any marked items
// ---------------------------------------------------------
//
void CVpnManagementUiLogView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
  
    if ( aResourceId == R_VPNUI_LOG_VIEW_MENU )
        {
        TInt currentItem = iLogContainer->iListBox->CurrentItemIndex();

        // the VPN log list is empty  all except Refresh, Help and Exit is dimmed
        if ( currentItem == -1 )
            {
            aMenuPane->SetItemDimmed( EVpnUiCmdLogDetails, ETrue );
            aMenuPane->SetItemDimmed( EVpnUiCmdClearLog, ETrue );
//            aMenuPane->SetItemDimmed( EVpnUiCmdRefreshLog, ETrue );
            }

        }
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::Id() const
// Returns Authority view id
// ---------------------------------------------------------
//
TUid CVpnManagementUiLogView::Id() const
    {
    return KVpnManagementUiLogViewId;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::HandleCommandL(TInt aCommand)
// Handles Softkey and Options list commands
// ---------------------------------------------------------
//
void CVpnManagementUiLogView::HandleCommandL( TInt aCommand )
    {
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
        
        case EVpnUiCmdRefreshLog:
            {
            iLogContainer->ShowLogsL();
            iLogContainer->DrawListBoxL( iCurrentPosition, iTopItem );
            break;
            }

        case EVpnUiCmdLogDetails:
            {
            iLogContainer->LogDetailsL(iLogContainer->iListBox->CurrentItemIndex());
            break;
            }

        case EVpnUiCmdClearLog:
            {
            iLogContainer->ClearLogL();
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
// CVpnManagementUiLogView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CVpnManagementUiLogView::HandleClientRectChange()
    {
    if ( iLogContainer )
        {
        iLogContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
//      TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
// Updates the view when opening it
// ---------------------------------------------------------
//
void CVpnManagementUiLogView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if ( iLogContainer )
        {
        ((CAknViewAppUi*)iAvkonAppUi)->RemoveFromViewStack( 
            *this, iLogContainer );
	    }
    iLogContainer = new (ELeave) CVpnManagementUiLogContainer( 
        *this, iLoader );

    iLogContainer->SetMopParent( this );
    iLogContainer->ConstructL( ClientRect(), iCurrentPosition );
    iLoader.ActivateTitleL( KViewTitleLogView );
 
    iLoader.ActivateNaviTextL();
    iLoader.PushNaviPaneL();

    ((CAknViewAppUi*)iAvkonAppUi)->AddToStackL( *this, iLogContainer );
    iLogContainer->DrawListBoxL( iCurrentPosition, iTopItem );
    
    // Add MiddleSoftKey
 	SetMiddleSoftKeyL(EFalse);       
   }


void CVpnManagementUiLogView::SetMiddleSoftKeyL(TBool aDrawNow)
    {
    if (iLogContainer->iListItemCount > 0)
    	{ 
		SetMiddleSoftKeyLabelL(R_MSK_VPN_DETAILS, EVpnUiCmdLogDetails);
    	}
    else
    	{
    	SetMiddleSoftKeyLabelL(R_MSK_REFRESH_VPN_LOG, EVpnUiCmdRefreshLog);
    	}

    if (aDrawNow)
    	{
    	if (Cba())
    		Cba()->DrawNow();
    	}
	}
    	

void CVpnManagementUiLogView::SetMiddleSoftKeyLabelL(TInt aResourceId, TInt aCommandId)
    {
   	CEikButtonGroupContainer* cbaGroup = Cba();
   	if (cbaGroup)
    	{
			cbaGroup->RemoveCommandFromStack(KVpnMSKControlId, EVpnUiCmdLogDetails);
			cbaGroup->RemoveCommandFromStack(KVpnMSKControlId, EVpnUiCmdRefreshLog);
		    HBufC* text = StringLoader::LoadLC(aResourceId); 
    		cbaGroup->AddCommandToStackL(KVpnMSKControlId, aCommandId, text->Des());
   		CleanupStack::PopAndDestroy(text);
    	}
    
   }

// ---------------------------------------------------------
// CVpnManagementUiLogView::DoDeactivate()
// Saves focus position when closing view
// ---------------------------------------------------------
//
void CVpnManagementUiLogView::DoDeactivate()
    {
    if ( iLogContainer )
        {
        iLoader.PopNaviPane();
        AppUi()->RemoveFromViewStack( *this, iLogContainer );
        
        delete iLogContainer;
        iLogContainer = NULL;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiLogView::Container()
// Returns iLogContainer
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiLogView::Container()
    {
    return iLogContainer;
    }

// End of File

