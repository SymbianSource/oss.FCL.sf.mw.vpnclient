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
* Description:   Management UI Container
*
*/



// INCLUDE FILES
#include <akntitle.h> //for CAknTitlePane
#include <barsread.h> //for TResourceReader
#include <StringLoader.h> 
#include <eiktxlbx.h> //for listbox control
#include <aknlists.h>
#include <eikclbd.h>
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h"
#include "vpnmanagementuicontainer.h"

#ifdef __SERIES60_HELP
#include <csxhelp/vpn.hlp.hrh> // for help context of VPN Management UI
#endif //__SERIES60_HELP


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiContainer::CVpnManagementUiContainer(
// CVpnManagementUiView& aParent, CVpnUiLoader& aLoader)
// Constructor with parent
// ---------------------------------------------------------
//
CVpnManagementUiContainer::CVpnManagementUiContainer(
    CVpnManagementUiView& aParent, CVpnUiLoader& aLoader ) : 
    iParent ( aParent ), iLoader ( aLoader )
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiContainer::~CVpnManagementUiContainer()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiContainer::~CVpnManagementUiContainer()
    {
	delete iListBox;
    }

// ---------------------------------------------------------
// CVpnManagementUiContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    TitlePaneTextsL();

    CreateListBoxL();
    ShowItemsL(); 

    SetRect(aRect);
    ActivateL();
    }


// ---------------------------------------------------------
// CVpnManagementUiContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CVpnManagementUiContainer::SizeChanged()
    {
    if (iListBox)
        {
        iListBox->SetRect(Rect());
        }
    }

// ---------------------------------------------------------
// CVpnManagementUiContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CVpnManagementUiContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CVpnManagementUiContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiContainer::ComponentControl(TInt aIndex) const
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
// CVpnManagementUiContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CVpnManagementUiContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange(aType); 
    
    //Handle change in layout orientation
    if (aType == KEikDynamicLayoutVariantSwitch || 
        aType == KAknsMessageSkinChange )
        {
		TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);
		DrawNow();
		}       
    }	
    
// ---------------------------------------------------------
// CVpnManagementUiContainer::DrawListBoxL()
// Draws listbox
// ---------------------------------------------------------
//
void CVpnManagementUiContainer::DrawListBoxL(
    TInt aCurrentPosition, TInt aTopItem)
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
// CVpnManagementUiContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CVpnManagementUiContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

TKeyResponse CVpnManagementUiContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (aType!=EEventKey)
        {
        return EKeyWasNotConsumed;
        }
    switch (aKeyEvent.iCode)
        {
        case EKeyUpArrow:
            if (iListBox)
                {
                return iListBox->OfferKeyEventL(aKeyEvent, aType);
                }
            break;
        case EKeyDownArrow:
            if (iListBox)
                {
                return iListBox->OfferKeyEventL(aKeyEvent, aType);
                }
            break;
        case EKeyEnter:
        case EKeyDevice3:
            // Select button or enter key pressed, do default action
            if (iListBox)
                {
                return iListBox->OfferKeyEventL(aKeyEvent, aType);
                }
            break;

        default:
            break;
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CVpnManagementUiContainer::TitlePaneTextsL
// Initialized Title pane text
// ---------------------------------------------------------
//
void CVpnManagementUiContainer::TitlePaneTextsL()
    {
    // first get StatusPane. Not owned
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane)
        {
        // then get TitlePane. Not owned
        CAknTitlePane* titlePane = ( CAknTitlePane* ) statusPane->ControlL( 
            TUid::Uid( EEikStatusPaneUidTitle ) );
        
        // set new titlepane text
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC(reader, R_VPN_MANAGEMENT_TITLE);
        titlePane->SetFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader
        }
    }

void CVpnManagementUiContainer::CreateListBoxL()
    {
    iListBox = new(ELeave) CAknSettingStyleListBox;
    iListBox->SetContainerWindowL( *this);

    iListBox->ConstructL( this, EAknListBoxSelectionList);		
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    }


void CVpnManagementUiContainer::ShowItemsL() 
    {
   	CDesCArrayFlat* itemTextArray = 
   	    static_cast<CDesCArrayFlat*>(iListBox->Model()->ItemTextArray());

    itemTextArray->Reset();
    iListBox->Reset();

    HBufC* resourceText = NULL;

    //Create VPN policies setting listbox item
    resourceText = StringLoader::LoadLC( R_VPN_POLICIES );
    itemTextArray->AppendL(*resourceText);
    CleanupStack::PopAndDestroy();  // resourceText
        
    //Create Policy servers setting listbox item
   
    resourceText = StringLoader::LoadLC( R_VPN_POLICY_SERVERS );
     itemTextArray->AppendL(*resourceText);
     CleanupStack::PopAndDestroy();  // resourceText

    //Create Log setting listbox item
    resourceText = StringLoader::LoadLC( R_VPN_LOG );
    itemTextArray->AppendL(*resourceText);
    CleanupStack::PopAndDestroy();  // resourceText
    
    iListBox->HandleItemAdditionL();
    }

// ---------------------------------------------------------
// CVpnManagementUiContainer::GetHelpContext
// This function is called when Help application is launched.  
// ---------------------------------------------------------
//
#ifdef __SERIES60_HELP
void CVpnManagementUiContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
	aContext.iMajor = KUidVpnManagementUi;
    aContext.iContext = KSET_HLP_VPN_CONFIG_MAIN;
    }
#endif //__SERIES60_HELP

// End of File  
