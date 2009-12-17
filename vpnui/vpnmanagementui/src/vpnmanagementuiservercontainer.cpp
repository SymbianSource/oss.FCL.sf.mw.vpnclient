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
#include <eikclbd.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <ErrorUI.h>
#include <sysutil.h>
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h"
#include "vpnmanagementuiservercontainer.h"
#include "vpnmanagementuidefs.h"
#include "vpnmanagementui.hrh"
#include "log_vpnmanagementui.h"

#ifdef __SERIES60_HELP
#include <csxhelp/vpn.hlp.hrh> // for help context of VPN Management UI
#endif //__SERIES60_HELP

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::CVpnManagementUiServerContainer(
// CVpnManagementUiServerView& aParent, CVpnUiLoader& aLoader)
// Constructor with parent
// ---------------------------------------------------------
//
CVpnManagementUiServerContainer::CVpnManagementUiServerContainer(
    CVpnManagementUiServerView& aParent, CVpnUiLoader& aLoader ) : 
    iParent ( aParent ), iLoader ( aLoader )
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::~CVpnManagementUiServerContainer()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiServerContainer::~CVpnManagementUiServerContainer()
    {
	delete iListBox;
    }

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::ConstructL(
// const TRect& aRect, TInt& aCurrentPosition, TInt& aTopItem)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::ConstructL(
    const TRect& aRect, TInt& /*aCurrentPosition*/, TInt& /*aTopItem*/ )
    {
 	CreateWindowL();
	CreateListBoxL();
    ShowServersL();
    if ((iListItemCount == 0) && (!iLoader.iBackFromServerDefinition))
        {
        iLoader.iShowDefineQuery = ETrue;
        }
    iLoader.iBackFromServerDefinition = EFalse;
    iShowWaitNote = EFalse;
	SetRect( aRect );
    }

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::ActivateL()
    {
    CCoeControl::ActivateL();
    // put empty list checking & required actions here.
    if ((iListItemCount == 0) && (iLoader.iShowDefineQuery))
        {
        HBufC* temp;
        temp = StringLoader::LoadLC( R_VPN_QUEST_DEFINE_NEW_SERVER );
        CAknQueryDialog* query = CAknQueryDialog::NewL( 
            CAknQueryDialog::EConfirmationTone );
        TInt retval = query->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
        CleanupStack::PopAndDestroy();  // temp
        iLoader.iShowDefineQuery = EFalse;
        if ( retval )
            {
            if (!SysUtil::FFSSpaceBelowCriticalLevelL(0, 0))
		        {
    	        iLoader.ChangeViewL(KChangeViewSettings);
		        }
            else
	            {
		    	CErrorUI* errorUi = CErrorUI::NewLC( *(CCoeEnv::Static()) );
		        errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
		        CleanupStack::PopAndDestroy();  // errorUi
				iLoader.iShowDefineQuery = EFalse;
				iLoader.iNewServerDefinition = EFalse;
	            }
            }
        }
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    if (iLoader.iNewServerDefinition)
        {//Synchronise server query
            HBufC* temp;
            temp = StringLoader::LoadLC( R_VPN_QUEST_SYNCHRONISE_SERVER );
            CAknQueryDialog* query = CAknQueryDialog::NewL( 
                CAknQueryDialog::EConfirmationTone );
            TInt retval = query->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
            CleanupStack::PopAndDestroy();  // temp
            if ( retval )
                {
                //Show wait dialog 
                //Save server index for Connecting via note
		        iLoader.iCurrentServerIndex = iLoader.AcuApiWrapperL().GetLastCreatedServerIndexL(); 
                //ShowWaitNoteL();
                iShowWaitNote = ETrue;
                // Get selection name for connecting via note
                // and save it to the member variable of iLoader
                iLoader.GetSelectionNameL( iLoader.iSelectionName );
                
                //Synchronise server
                iLoader.AcuApiWrapperL().SynchroniseLastCreatedServerL(this);
   
                //Set iNewServerDefinition to false, because 
                //we are now synchronised server
                iLoader.iNewServerDefinition = EFalse;
                }
        } ***/
    }


// ---------------------------------------------------------
// CVpnManagementUiServerContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::SizeChanged()
    {
    iListBox->SetRect(Rect());
    }

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::FocusChanged(TDrawNow aDrawNow)
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if ( iListBox ) 
		{
		iListBox->SetFocus( IsFocused(), aDrawNow );
		}
	}
	
// ---------------------------------------------------------
// CVpnManagementUiServerContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::HandleResourceChange( TInt aType )
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
    
enum { EListBoxControl, ENumberOfControls };

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CVpnManagementUiServerContainer::CountComponentControls() const
    {
	// return number of controls inside this container
    return ENumberOfControls; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiServerContainer::ComponentControl(TInt aIndex) const
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
// CVpnManagementUiServerContainer::HandleControlEventL(
//    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
// Cannot be changed to non-leaving function. 
// L-function is required by the class definition, even if empty.
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::OfferKeyEventL(
// const TKeyEvent& aKeyEvent, TEventCode aType)
// Handles the key events OK button, arrow keys, delete key.
// OK button functionality is different depending on the number of marked items
// in the list; zero marked -> show details view, one or more marked -> show
// OKOptions list, list empty -> OK inactive.
// Delete active only if list not empty.
// ---------------------------------------------------------
//
TKeyResponse CVpnManagementUiServerContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
	{
    TKeyResponse retval = EKeyWasNotConsumed;
	if ( aType==EEventKey )
		{
		if ( (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter) && //select or enter 
            iListBox->CurrentItemIndex() > -1 ) //This is for empty list
			{
	        // Selection key pressed 
            iParent.HandleCommandL( EVpnUiCmdEditServer );
			retval = EKeyWasConsumed;	
			}

        else if ( aKeyEvent.iCode == EKeyDelete 
                  || aKeyEvent.iCode == EKeyBackspace )
            {
			if ( iListBox->CurrentItemIndex() > -1 )
				{
				iParent.HandleCommandL( EVpnUiCmdDeleteServer );
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
// CVpnManagementUiServerContainer::CreateListBoxL()
// Creates listbox, sets empty listbox text.
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::CreateListBoxL()
	{
	iListBox = new( ELeave ) CAknSingleStyleListBox;
	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxMarkableList ); 
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->
        SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto );

	HBufC* stringHolder = StringLoader::LoadLC( R_VPN_POLICY_SERVERS_EMPTY );
	iListBox->View()->SetListEmptyTextL( *stringHolder );
	CleanupStack::PopAndDestroy();  // stringHolder
	
	iListBox->SetListBoxObserver (this);
	}

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::DrawListBoxL(TInt aCurrentPosition, TInt aTopItem)
// Draws listbox, fetches graphic icons for markable list
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::DrawListBoxL( TInt aCurrentPosition, TInt aTopItem )
	{
    if (aCurrentPosition >= 0)
		{
		iListBox->SetTopItemIndex( aTopItem );
		iListBox->SetCurrentItemIndex( aCurrentPosition );
		}
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    if(iLoader.iNewServerDefinition)
        {
        iListBox->SetCurrentItemIndex(iLoader.AcuApiWrapperL().GetLastCreatedServerIndexL());
        } ***/
    
	ActivateL();
	DrawNow();
	}

// ---------------------------------------------------------
// CVpnManagementUiServerContainer::UpdateListBoxL(TInt& aCurrentPosition)
// Refresh listbox after VPN policy server deletion.
// ---------------------------------------------------------
//
void CVpnManagementUiServerContainer::UpdateListBoxL(TInt& aCurrentPosition)
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
            HBufC* stringHolder = StringLoader::LoadLC( R_VPN_POLICY_SERVERS_EMPTY );
	        iListBox->View()->SetListEmptyTextL( *stringHolder );
	        CleanupStack::PopAndDestroy();  // stringHolder
            }
        }
    iListItemCount = iListItemCount -1;
    iParent.SetMiddleSoftKeyL(ETrue);
    DrawNow();
	}


void CVpnManagementUiServerContainer::ShowServersL() 
    {
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    CArrayFix<TAcuApiServerListElem>* serverList = iLoader.AcuApiWrapperL().ServerListL();

   	CDesCArrayFlat* itemTextArray = static_cast<CDesCArrayFlat*>(iListBox->Model()->ItemTextArray());

	itemTextArray->Reset();
	iListBox->Reset();

    iListItemCount = serverList->Count();

    for (TInt i = 0; i < iListItemCount; ++i)
        {
        TBuf<KAcuMaxServerNameLocalLth+2> serverName;
        serverName.Append(_L("\t"));
        serverName.Append(serverList->At(i).iServerNameLocal);
        itemTextArray->AppendL(serverName);
       }


	iListBox->HandleItemAdditionL();
	***/
    }


void CVpnManagementUiServerContainer::DeleteServerL(TInt /* aIndex */)
    {
  	LOG(Log::Printf(_L("CVpnManagementUiServerContainer::DeleteServerL\n")));

    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.

    TBuf<KAcuMaxServerNameLocalLth> serverName;
    serverName.Append(
        iLoader.AcuApiWrapperL().ServerListL()->At(aIndex).iServerNameLocal);
   	LOG(Log::Printf(_L("CVpnManagementUiServerContainer::DeleteServerL - server name %S\n"), &serverName));
    HBufC* temp;

    TAcuServerId serverId;
    serverId = iLoader.AcuApiWrapperL().ServerListL()->At(aIndex).iServerId;

    //Get server policy count from vpnapiext
    TInt policyCount = 0;
    TInt ret = iLoader.VpnApiWrapperL().GetServerPolicyCount(
        serverId, policyCount);

  	LOG(Log::Printf(_L("CVpnManagementUiServerContainer::DeleteServerL - server policy count %d\n"), policyCount));

    if ((ret== KErrNone) && (policyCount > 0)) //at least one policy related to given ACU server is found
        {
        // policies related to the server, confirmation query
        temp = StringLoader::LoadLC( R_VPN_QUEST_DELETE_SERVER_ASSOCIATED );
        }
    else
        {
        // no policies related to the server, confirmation query
        temp = StringLoader::LoadLC( R_VPN_QUEST_DELETE_SERVER, serverName );
        }

    CAknQueryDialog* query = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
    TInt retval = query->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
    CleanupStack::PopAndDestroy();  // temp
    if ( retval )
        {
        TInt ret = iLoader.AcuApiWrapperL().DeleteServer(aIndex);
        if(ret == KErrNone)
            {
            //Update listbox
            UpdateListBoxL( aIndex );
            //Update iServerList
            iLoader.AcuApiWrapperL().ServerListL();
            
            //Set iNewServerDefinition to false
            iLoader.iNewServerDefinition = EFalse;
            }
        else if (ret == KErrServerBusy)
            {
            //Show an information note that server is currently in use
            HBufC* noteText;
            noteText = StringLoader::LoadLC( R_VPN_INFO_POLICY_SERVER_IN_USE );
            CAknInformationNote* note = new(ELeave)CAknInformationNote(ETrue);
            note->SetTimeout(CAknNoteDialog::ELongTimeout); //3sec
            note->ExecuteLD(noteText->Des());
            CleanupStack::PopAndDestroy();  // noteText
            }
        }
    ***/
    }

void CVpnManagementUiServerContainer::SynchroniseServerL(TInt /* aIndex */)
    {
  	LOG_("CVpnManagementUiServerContainer::SynchroniseServerL");

    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.

    TBool selectionExists = iLoader.SelectionExistsL( aIndex );
    if( selectionExists )
        {
        //Save server index for Connecting via note
        iLoader.iCurrentServerIndex = aIndex;
        // Get selection name for connecting via note
        // and save it to the member variable of iLoader
        iLoader.GetSelectionNameL( iLoader.iSelectionName );
        //Show wait dialog 
        iLoader.ShowWaitNoteL();

        iLoader.AcuApiWrapperL().SynchroniseServerL( aIndex,this );
        //Set iNewServerDefinition to false, because when
        //we go to the policy view we don't want to show note
        //"new server defined synchronise now"
        iLoader.iNewServerDefinition = EFalse;
        } ***/
    }


void CVpnManagementUiServerContainer::NotifyUpdatePolicyCompleteL(TInt /*aResult*/)
    {
    }

void CVpnManagementUiServerContainer::NotifySynchroniseServerCompleteL(TInt aResult)
    {
   	LOG(Log::Printf(_L("CVpnManagementUiServerContainer::NotifySynchroniseServerCompleteL - Result %d\n"), aResult));

    if ( iLoader.iWaitDialog ) 
        {
        iLoader.DeleteWaitNoteL();//->iWaitDialog->ProcessFinishedL(); // deletes the wait dialog
        }


    if (aResult == KErrNone)
        {
        //Confirmation note
        HBufC* string = StringLoader::LoadLC( R_VPN_CONF_POLICY_SERVER_SYNC_OK );
        CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string );
        }
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    else if (aResult == KAcuErrServerCertExpiredByPkiService)
        {
        //Show an information note
        HBufC* noteText;
        noteText = StringLoader::LoadLC( R_VPN_INFO_SERVER_CERT_EXPIRED );
        CAknInformationNote* note = new(ELeave)CAknInformationNote(ETrue);
        note->SetTimeout(CAknNoteDialog::ELongTimeout); //3sec
        note->ExecuteLD(noteText->Des());
        CleanupStack::PopAndDestroy();  // noteText
        } ***/
    else if (aResult == KErrDiskFull)
    	{
    	CErrorUI* errorUi = CErrorUI::NewLC( *(CCoeEnv::Static()) );
        errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
        CleanupStack::PopAndDestroy();  // errorUi
    	}    
    else if (aResult == KErrCancel)
        {
        // No operation, i.e. don't show anything
        }
    else
        {
        //Show an information note
        HBufC* noteText;
        noteText = StringLoader::LoadLC( R_VPN_INFO_POLICY_SERVER_SYNC_ERROR );
        CAknInformationNote* note = new(ELeave)CAknInformationNote(ETrue);
        note->SetTimeout(CAknNoteDialog::ELongTimeout); //3sec
        note->ExecuteLD(noteText->Des());
        CleanupStack::PopAndDestroy();  // noteText
        }

    iParent.SetMiddleSoftKeyL(ETrue);
    }

void CVpnManagementUiServerContainer::NotifyStepChangedL(TInt /*aResult*/)
    {
    if (iShowWaitNote)
        {
        iLoader.ShowWaitNoteL();
        iShowWaitNote = EFalse;
        }
    if ( iLoader.iWaitDialog ) 
        {
        iLoader.SetTextL();
        }
    }


void CVpnManagementUiServerContainer::HandleListBoxEventL (CEikListBox* /*aListBox*/, 
                                                           TListBoxEvent aEventType)
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed: 
        case EEventItemSingleClicked:
            iParent.HandleCommandL(EVpnUiCmdEditServer);        
            break;
        default:  
            //Do nothing          
            break;
        }        
    }


// ---------------------------------------------------------
// CVpnManagementUiServerContainer::GetHelpContext
// This function is called when Help application is launched.  
// (other items were commented in a header).
// ---------------------------------------------------------
//
#ifdef __SERIES60_HELP
void CVpnManagementUiServerContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
	aContext.iMajor = KUidVpnManagementUi;
    aContext.iContext = KSET_HLP_VPN_POLICY_SERVERS;
    }
#endif //__SERIES60_HELP

// End of File  
