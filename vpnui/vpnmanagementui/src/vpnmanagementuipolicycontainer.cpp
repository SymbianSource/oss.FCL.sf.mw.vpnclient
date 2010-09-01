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
* Description: Policy view container
*
*/

#include <eikclbd.h>
#include <AknIconArray.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <sysutil.h>
#include <ErrorUI.h>
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h"
#include "vpnmanagementuipolicycontainer.h"
#include "vpnmanagementui.hrh"
#include "vpnextapi.h"

#ifdef __SERIES60_HELP
#include <csxhelp/vpn.hlp.hrh> // for help context of VPN Management UI
#endif //__SERIES60_HELP

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::CVpnManagementUiPolicyContainer(
// CVpnManagementUiPolicyView& aParent, CVpnUiLoader& aLoader)
// Constructor with parent
// ---------------------------------------------------------
//
CVpnManagementUiPolicyContainer::CVpnManagementUiPolicyContainer(
    CVpnManagementUiPolicyView& aParent, CVpnUiLoader& aLoader ) : 
    iParent ( aParent ), iLoader ( aLoader )
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::~CVpnManagementUiPolicyContainer()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiPolicyContainer::~CVpnManagementUiPolicyContainer()
    {
	delete iListBox;
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::ConstructL(
// const TRect& aRect, TInt& aCurrentPosition, TInt& aTopItem)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::ConstructL(
    const TRect& aRect, TInt& /*aCurrentPosition*/, TInt& /*aTopItem*/ )
    {
	CreateWindowL();
    CreateListBoxL();
    
    //Initialization

    
    ShowPoliciesL();
	SetRect( aRect );
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::ActivateL()
    {
    CCoeControl::ActivateL();

    }


// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::SizeChanged()
    {
    iListBox->SetRect(Rect());
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::FocusChanged(TDrawNow aDrawNow)
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if ( iListBox ) 
		{
		iListBox->SetFocus( IsFocused(), aDrawNow );
		}
	}

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::HandleResourceChangeL
// ---------------------------------------------------------
//

void CVpnManagementUiPolicyContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange(aType); 
     
    //Handle change in layout orientation
    if (aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange )
        {
        iListBox->HandleResourceChange(aType);
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);
		DrawNow();
		}
    }    
    
// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::HandleListBoxEventL(
// CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
    switch(aEventType)
        {
        case EEventEnterKeyPressed: 
        case EEventItemSingleClicked:
            iParent.HandleCommandL(EVpnUiCmdPolicyDetails);        
            break;
        default:  
            //Do nothing          
            break;
        }        
	}
enum { EListBoxControl, ENumberOfControls };

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CVpnManagementUiPolicyContainer::CountComponentControls() const
    {
	// return number of controls inside this container
    return ENumberOfControls; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiPolicyContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
			return iListBox;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::HandleControlEventL(
//    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
// Cannot be changed to non-leaving function. 
// L-function is required by the class definition, even if empty.
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::OfferKeyEventL(
// const TKeyEvent& aKeyEvent, TEventCode aType)
// Handles the key events OK button, arrow keys, delete key.
// OK button functionality is different depending on the number of marked items
// in the list; zero marked -> show details view, one or more marked -> show
// OKOptions list, list empty -> OK inactive.
// Delete active only if list not empty.
// ---------------------------------------------------------
//
TKeyResponse CVpnManagementUiPolicyContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
	{

    // If operation is currenly onway, do not continue.
	TKeyResponse retval = EKeyWasNotConsumed;
	if ( aType==EEventKey )
		{
		if ( (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter) && // select or enter
            iListBox->CurrentItemIndex() > -1) //This is for empty list
            {
	        // Selection key pressed.
            iParent.PolicyDetailsL(iListBox->CurrentItemIndex());

			retval = EKeyWasConsumed;	
			}

		else if ( aKeyEvent.iCode == EKeyDelete 
                  || aKeyEvent.iCode == EKeyBackspace )
			{
			if ( iListBox->CurrentItemIndex() > -1 )
				{
				iParent.HandleCommandL( EVpnUiCmdDeletePolicy );
				retval = EKeyWasConsumed; 
				}
			}
		else
			{
			retval = iListBox->OfferKeyEventL( aKeyEvent, aType );
			}
		}
	else
		{
		retval = iListBox->OfferKeyEventL( aKeyEvent, aType );
		}


    return retval;
	}

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::CreateListBoxL()
// Creates listbox, sets empty listbox text.
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::CreateListBoxL()
	{

	iListBox = new( ELeave ) CAknSingleStyleListBox;
	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxMarkableList ); 
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->
        SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto );
	iListBox->SetListBoxObserver(this);

	HBufC* stringHolder = StringLoader::LoadLC( R_VPN_POLICIES_EMPTY );
	iListBox->View()->SetListEmptyTextL( *stringHolder );
	CleanupStack::PopAndDestroy();  // stringHolder

	}

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::DrawListBoxL(TInt aCurrentPosition, TInt aTopItem)
// Draws listbox, fetches graphic icons for markable list
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::DrawListBoxL( 
    TInt aCurrentPosition, TInt aTopItem )
	{

	if (aCurrentPosition >= 0)
		{
		iListBox->SetTopItemIndex( aTopItem );
		iListBox->SetCurrentItemIndex( aCurrentPosition );
		}
	ActivateL();
	DrawNow();
	}

// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::UpdateListBoxL(TInt& aCurrentPosition)
// Updates listbox after deletion, sets No VPN policies text 
// if deleted policy was a last one.
// ---------------------------------------------------------
//
void CVpnManagementUiPolicyContainer::UpdateListBoxL( TInt& aCurrentPosition )
	{ 
	MDesCArray* itemList = iListBox->Model()->ItemTextArray();
	CDesCArray* itemArray = ( CDesCArray* )itemList;

    itemArray->Delete(aCurrentPosition);
    itemArray->Compress();

    iListBox->HandleItemRemovalL();
    if(aCurrentPosition > 0)
        {
        iListBox->SetCurrentItemIndexAndDraw(aCurrentPosition-1);
        }
    if(aCurrentPosition == 0 )
        {
        if (iListItemCount > 1 )
            {
            iListBox->SetCurrentItemIndexAndDraw(aCurrentPosition);
            }
        else
            {
            HBufC* stringHolder = StringLoader::LoadLC( R_VPN_POLICIES_EMPTY );
	        iListBox->View()->SetListEmptyTextL( *stringHolder );
	        CleanupStack::PopAndDestroy();  // stringHolder
            }
        }
    iListItemCount = iListItemCount -1;
    iParent.SetMiddleSoftKeyL(ETrue);
    DrawNow();
	}

void CVpnManagementUiPolicyContainer::ShowPoliciesL() 
    {
    CArrayFixFlat<TVpnPolicyInfo>* policyList = 
        iLoader.VpnApiWrapperL().PolicyListL();

   	CDesCArrayFlat* itemTextArray = 
   	    static_cast<CDesCArrayFlat*>(iListBox->Model()->ItemTextArray());

	itemTextArray->Reset();
	iListBox->Reset();

    iListItemCount = policyList->Count();

    for (TInt i = 0; i < iListItemCount; ++i)
        {
        TBuf<KMaxNameLength+2> policyName;
        policyName.Append(_L("\t"));
        policyName.Append(policyList->At(i).iName);
        itemTextArray->AppendL(policyName);
        }

	iListBox->HandleItemAdditionL();
    }



// ---------------------------------------------------------
// CVpnManagementUiPolicyContainer::GetHelpContext
// This function is called when Help application is launched.  
// (other items were commented in a header).
// ---------------------------------------------------------
//
#ifdef __SERIES60_HELP
void CVpnManagementUiPolicyContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
	aContext.iMajor = KUidVpnManagementUi;
    aContext.iContext = KSET_HLP_VPN_POLICY_VIEW;
    }
#endif //__SERIES60_HELP

// End of File  
