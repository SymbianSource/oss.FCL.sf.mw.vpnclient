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
* Description:   VPN Policy view
*
*/



// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>            // for warning & information notes
#include <vpnmanagementuirsc.rsg>
#include <cdblen.h>

#include "vpnuiloader.h" 
#include "vpnmanagementuipolicyview.h"
#include "vpnmanagementuipolicycontainer.h"
#include "vpnmanagementui.hrh"
#ifdef __SERIES60_HELP
#include    <hlplch.h>   // For HlpLauncher 
#endif //__SERIES60_HELP

/** MSK control Id. */
const TInt KVpnMSKControlId = 3;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::CVpnManagementUiPolicyView
// ---------------------------------------------------------
//
CVpnManagementUiPolicyView::CVpnManagementUiPolicyView( 
    CVpnUiLoader& aLoader ) : iLoader(aLoader)
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::~CVpnManagementUiPolicyView()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiPolicyView::~CVpnManagementUiPolicyView()
    {
    if ( iPolicyContainer )
        {
        AppUi()->RemoveFromViewStack(*this, iPolicyContainer);
        delete iPolicyContainer;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::NewL
// ---------------------------------------------------------
//
CVpnManagementUiPolicyView* CVpnManagementUiPolicyView::NewL(
    const TRect& aRect, CVpnUiLoader& aLoader )
    {
    CVpnManagementUiPolicyView* self = NewLC( aRect, aLoader );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::NewLC
// ---------------------------------------------------------
//
CVpnManagementUiPolicyView* CVpnManagementUiPolicyView::NewLC(
    const TRect& /*aRect*/, CVpnUiLoader& aLoader )
    {
    CVpnManagementUiPolicyView* self = 
        new ( ELeave ) CVpnManagementUiPolicyView( aLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::ConstructL() 
    {
    BaseConstructL( R_VPNUI_POLICIES_VIEW );
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::DynInitMenuPaneL(
//      TInt aResourceId,CEikMenuPane* aMenuPane)
// Updates Options list with correct items depending on 
// whether the listbox is empty or if it has any marked items
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if ( aResourceId == R_VPNUI_POLICIES_VIEW_MENU )
        {
        TInt currentItem = iPolicyContainer->iListBox->CurrentItemIndex();

        // the policy list is empty  all except Install and Exit is dimmed
        if ( currentItem == -1 )
            {
            aMenuPane->SetItemDimmed( EVpnUiCmdPolicyDetails, ETrue );
            aMenuPane->SetItemDimmed( EVpnUiCmdDeletePolicy, ETrue );
            aMenuPane->SetItemDimmed( EVpnUiCmdUpdatePolicy, ETrue );
            // NSSM support is discontinued
            aMenuPane->SetItemDimmed( EVpnUiCmdInstallPolicies, ETrue );
            }
        // at least one policy is installed, 
        else 
            {
            aMenuPane->SetItemDimmed( EVpnUiCmdInstallPolicies, ETrue );
            // NSSM support is discontinued
            aMenuPane->SetItemDimmed( EVpnUiCmdUpdatePolicy, ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::Id() const
// Returns Authority view id
// ---------------------------------------------------------
//
TUid CVpnManagementUiPolicyView::Id() const
    {
    return KVpnManagementUiPolicyViewId;
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::HandleCommandL(TInt aCommand)
// Handles Softkey and Options list commands
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::HandleCommandL( TInt aCommand )
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

        case EVpnUiCmdPolicyDetails:
            {
            // Show details
            PolicyDetailsL(iPolicyContainer->iListBox->CurrentItemIndex());
            break;
            }

        case EVpnUiCmdInstallPolicies:
            {
            ffsLow = iLoader.FFSSpaceBelowCriticalLevelL( ETrue, 0 );
    		if(!ffsLow)
    			{
            	iPolicyContainer->InstallPoliciesL();
    			}
            break;
            }
        case EVpnUiCmdUpdatePolicy:
            {
            ffsLow = iLoader.FFSSpaceBelowCriticalLevelL( ETrue, 0 );
    		if(!ffsLow)
    			{
            	iCurrentPosition = iPolicyContainer->iListBox->CurrentItemIndex();
            	//Save policy index for Connecting via note
            	iLoader.iCurrentPolicyIndex = iCurrentPosition;
            	//Save update operation for Connecting via note
            	iLoader.iPolicyUpdate = ETrue;

            	TVpnPolicyInfo policyInfo;
            	policyInfo.iId = iLoader.VpnApiWrapperL().PolicyListL()->At(
            	    iCurrentPosition).iId;
            	iPolicyContainer->UpdatePolicyL(policyInfo.iId);
    			}
            break;
            }

        case EVpnUiCmdDeletePolicy:
            {
            //confirmation query
            HBufC* temp;
           
            TVpnPolicyName policyName;
            iCurrentPosition = iPolicyContainer->iListBox->CurrentItemIndex();
            iTopItem = iPolicyContainer->iListBox->TopItemIndex();

            if (NotAssociatedToIapL(iCurrentPosition, policyName))
                {
                temp = StringLoader::LoadLC( R_VPN_QUEST_DELETE_POLICY, policyName );
                }
            else
                {
                temp = StringLoader::LoadLC( R_VPN_QUEST_DELETE_POLICY_ASSOCIATED );
                }
            CAknQueryDialog* query = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
            TInt retval = query->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
            CleanupStack::PopAndDestroy();  // temp
            if ( retval )
                {
                //Delete policy
                iLoader.VpnApiWrapperL().DeletePolicyL(iPolicyContainer->iListBox->CurrentItemIndex());
                //Update listbox
                iPolicyContainer->UpdateListBoxL(iCurrentPosition);
                //Update iPolicyList
                iLoader.VpnApiWrapperL().PolicyListL();
                iCurrentPosition = iPolicyContainer->iListBox->CurrentItemIndex();
                }
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
// CVpnManagementUiPolicyView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::HandleClientRectChange()
    {
    if ( iPolicyContainer )
        {
        iPolicyContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::DoActivateL
// Updates the view when opening it
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if ( iPolicyContainer )
        {
        ((CAknViewAppUi*)iAvkonAppUi)->RemoveFromViewStack( 
            *this, iPolicyContainer );
		}
    iPolicyContainer = new (ELeave) CVpnManagementUiPolicyContainer( 
        *this, iLoader );

    iPolicyContainer->SetMopParent( this );
    iPolicyContainer->ConstructL( 
        ClientRect(), iCurrentPosition, iTopItem );
    iLoader.ActivateTitleL( KViewTitlePolicyView );
    iLoader.PushDefaultNaviPaneL();
    
    ((CAknViewAppUi*)iAvkonAppUi)->AddToStackL( *this, iPolicyContainer );
    iPolicyContainer->DrawListBoxL( iCurrentPosition, iTopItem );
 
 	SetMiddleSoftKeyL(EFalse);   
	}

void CVpnManagementUiPolicyView::SetMiddleSoftKeyL(TBool aDrawNow)
    {
    if (iPolicyContainer->iListItemCount > 0)
    	{ 
		SetMiddleSoftKeyLabelL(R_MSK_VPN_DETAILS, EVpnUiCmdPolicyDetails);
    	}
    else
    	{
    	SetMiddleSoftKeyLabelL(R_MSK_INSTALL_VPN_POLICIES, EVpnUiCmdInstallPolicies);
    	}

    if (aDrawNow)
    	{
    	if (Cba())
    		Cba()->DrawNow();
    	}
	}
    	

void CVpnManagementUiPolicyView::SetMiddleSoftKeyLabelL(
    TInt aResourceId, TInt aCommandId)
    {
	CEikButtonGroupContainer* cbaGroup = Cba();
	if (cbaGroup)
    	{
	    HBufC* text = HBufC::NewLC(KMaxLengthTextDetailsBody); 
		ReadResourceL(*text, aResourceId);
		cbaGroup->RemoveCommandFromStack(
		    KVpnMSKControlId, EVpnUiCmdPolicyDetails);
		cbaGroup->RemoveCommandFromStack(
		    KVpnMSKControlId, EVpnUiCmdInstallPolicies);
		cbaGroup->AddCommandToStackL(
		    KVpnMSKControlId, aCommandId, text->Des());
		CleanupStack::PopAndDestroy(text);
    	}
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::DoDeactivate()
// Saves focus position when closing view
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::DoDeactivate()
    {
    if ( iPolicyContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iPolicyContainer );
        
        delete iPolicyContainer;
        iPolicyContainer = NULL;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::Container()
// Returns iPolicyContainer
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiPolicyView::Container()
    {
    return iPolicyContainer;
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::PolicyDetailsL()
// Creates the whole of policy details view
// Appends strings from both resources and VPN API to one
// message body text and displays it.
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::PolicyDetailsL(TInt aIndex)
    {
    TVpnPolicyDetails policyDetails;

    iLoader.VpnApiWrapperL().GetPolicyDetailsL(aIndex,policyDetails);
    HBufC* message = HBufC::NewLC(KMaxLengthTextDetailsBody); 

    // Description:
    ReadResourceL(*message, R_VPN_DETAIL_POLICY_DESCRIPTION );
    message->Des().Append(KVpnManagementUiEnter);
    message->Des().Append(policyDetails.iDescription);
    message->Des().Append(KVpnManagementUiEnterTwice);

    // Policy status:
    ReadResourceL(*message, R_VPN_DETAIL_POLICY_STATUS );
    message->Des().Append(KVpnManagementUiEnter);

    if (policyDetails.iUsageStatus == EUsageStatusUnused)
        ReadResourceL(*message, R_VPN_DETAIL_POLICY_NOT_ASSOCIATED );
    else if (policyDetails.iUsageStatus == EUsageStatusAssignedToIap)
        ReadResourceL(*message, R_VPN_DETAIL_POLICY_ASSOCIATED );
    else if (policyDetails.iUsageStatus == EUsageStatusActive)
        ReadResourceL(*message, R_VPN_DETAIL_POLICY_ACTIVE );

    message->Des().Append(KVpnManagementUiEnterTwice);

    // Certificate status:
    ReadResourceL(*message, R_VPN_DETAIL_CERT_STATUS );
    message->Des().Append(KVpnManagementUiEnter);

    if (policyDetails.iPkiStatus == EPkiStatusReady)
        ReadResourceL(*message, R_VPN_DETAIL_CERT_STATUS_OK );
    else if (policyDetails.iPkiStatus == EPkiStatusCertExpired)
        ReadResourceL(*message, R_VPN_DETAIL_CERT_STATUS_EXPIRED );
    else if (policyDetails.iPkiStatus == EPkiStatusNoCert)
        ReadResourceL(*message, R_VPN_DETAIL_CERT_STATUS_MISSING );
    else if (policyDetails.iPkiStatus == EPkiStatusCertNotValidYet)
        ReadResourceL(*message, R_VPN_DETAIL_CERT_STATUS_NOT_YET_VALID );

    message->Des().Append(KVpnManagementUiEnterTwice);

    // Policy name:
    ReadResourceL(*message, R_VPN_DETAIL_POLICY_NAME );
    message->Des().Append(KVpnManagementUiEnter);
    message->Des().Append(policyDetails.iName);
    message->Des().Append(KVpnManagementUiEnterTwice);
    
    // Header for the message query dialog

	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*message);
	dlg->PrepareLC(R_MESSAGE_QUERY_DETAILS_VIEW);
	dlg->QueryHeading()->SetTextL(policyDetails.iName);
	dlg->RunLD();

	CleanupStack::PopAndDestroy(); // message
    }

TBool CVpnManagementUiPolicyView::NotAssociatedToIapL(TInt aIndex, TVpnPolicyName& aPolicyName)
    {
    TVpnPolicyDetails policyDetails;
    iLoader.VpnApiWrapperL().GetPolicyDetailsL(aIndex,policyDetails);
    aPolicyName = policyDetails.iName;
    if (policyDetails.iUsageStatus == EUsageStatusUnused)
        return ETrue;
    else 
        return EFalse;
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyView::ReadResourceL
// Reads text from resource
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyView::ReadResourceL(HBufC& aText, TInt aResource)
	{
	HBufC* text = StringLoader::LoadLC( aResource );
	aText.Des().Append(text->Des());
	CleanupStack::PopAndDestroy();  // text
	}


// End of File

