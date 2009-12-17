/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: The container is responsible of showing log viewer.
*
*/



// INCLUDE FILES
#include <eikclbd.h>
#include <AknIconArray.h>
#include <aknlists.h>
#include <aknmessagequerydialog.h> //for LOG details message query
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h"
#include "vpnmanagementuilogcontainer.h"
#include "vpnmanagementuidefs.h"
#include "vpnmanagementui.hrh"
#ifdef __SCALABLE_ICONS
#include <AknsUtils.h>
#include <vpnmanagementui.mbg>
#endif //__SCALABLE_ICONS


#ifdef __SERIES60_HELP
#include <csxhelp/vpn.hlp.hrh> // for help context of VPN Management UI
#endif //__SERIES60_HELP

//CONSTANTS
const TInt KVpnLogNumberOfIcons = 3;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::CVpnManagementUiLogContainer(
// CVpnManagementUiLogView& aParent, CVpnUiLoader& aLoader)
// Constructor with parent
// ---------------------------------------------------------
//
CVpnManagementUiLogContainer::CVpnManagementUiLogContainer(
    CVpnManagementUiLogView& aParent, CVpnUiLoader& aLoader ) : 
    iParent ( aParent ), iLoader ( aLoader )
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::~CVpnManagementUiLogContainer()
// Destructor
// ---------------------------------------------------------
//
CVpnManagementUiLogContainer::~CVpnManagementUiLogContainer()
    {
    if(iEventViewer)
        {
        delete iEventViewer;
        iEventViewer = NULL;
        }
    if(iLogList)
        {
        delete iLogList;
        iLogList = NULL;
        }
	delete iListBox;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::ConstructL(
// const TRect& aRect, TInt& aCurrentPosition, TInt& aTopItem)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::ConstructL(
    const TRect& aRect, TInt& /*aCurrentPosition*/ )
    {
    CreateWindowL();
	CreateListBoxL();

    ShowLogsL();
	SetRect( aRect );
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::SizeChanged()
    {
    iListBox->SetRect(Rect());
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::FocusChanged(TDrawNow aDrawNow)
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if ( iListBox ) 
		{
		iListBox->SetFocus( IsFocused(), aDrawNow );
		}
	}
	
// ---------------------------------------------------------
// CVpnManagementUiLogContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::HandleResourceChange( TInt aType )
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
// CVpnManagementUiLogContainer::HandleListBoxEventL(
// CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
    switch(aEventType)
        {
        case EEventEnterKeyPressed: 
        case EEventItemSingleClicked:
            iParent.HandleCommandL(EVpnUiCmdLogDetails);        
            break;
        default:  
            //Do nothing          
            break;
        }        
	}
enum { EListBoxControl, ENumberOfControls };

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CVpnManagementUiLogContainer::CountComponentControls() const
    {
	// return number of controls inside this container
    return ENumberOfControls; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CVpnManagementUiLogContainer::ComponentControl(TInt aIndex) const
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
// CVpnManagementUiLogContainer::HandleControlEventL(
//    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
// Cannot be changed to non-leaving function. 
// L-function is required by the class definition, even if empty.
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::OfferKeyEventL(
// const TKeyEvent& aKeyEvent, TEventCode aType)
// Handles the key events OK button, arrow keys, delete key.
// OK button show VPN log details,
// if list is empty then arrow keys and OK inactive.
// Delete active only if list not empty.
// ---------------------------------------------------------
//
TKeyResponse CVpnManagementUiLogContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
	{
    TKeyResponse retval = EKeyWasNotConsumed;
	if ( aType==EEventKey )
		{
		if ( (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter) && // select or enter
             iListBox->CurrentItemIndex() > -1) //This is for empty list
            {
	        // Selection key pressed.
            LogDetailsL(iListBox->CurrentItemIndex());

			retval = EKeyWasConsumed;	
			}

        else if ( aKeyEvent.iCode == EKeyUpArrow )
			{
			retval = iListBox->OfferKeyEventL( aKeyEvent, aType );
    		}
		else if ( aKeyEvent.iCode == EKeyDownArrow )
			{
            TEventProperties eventProperties;
            HBufC* eventText = NULL;
            TInt currentPosition = iListBox->CurrentItemIndex();
            if((iEventViewer != NULL) && (iLogList->Count() > 0))
                {
                iEventViewer->GetEventUsingEventNumber(eventText, iLogList->At(iLogList->Count() - 1).iEventNumber, eventProperties);
                delete eventText;
                }
            UpdateListBoxL(currentPosition);
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
// CVpnManagementUiLogContainer::CreateListBoxL()
// Creates listbox, sets empty listbox text.
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::CreateListBoxL()
	{
	iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox;
	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxSelectionList ); 
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->
        SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto );
	iListBox->SetListBoxObserver(this);

#ifdef __SCALABLE_ICONS

    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( KVpnLogNumberOfIcons );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    
    TFileName fileName = GetIconFilenameL();

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                                            KAknsIIDQgnPropVpnLogInfo,
                                            fileName, 
                                            EMbmVpnmanagementuiQgn_prop_vpn_log_info, 
                                            EMbmVpnmanagementuiQgn_prop_vpn_log_info_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                                            KAknsIIDQgnPropVpnLogWarn,
                                            fileName, 
                                            EMbmVpnmanagementuiQgn_prop_vpn_log_warn, 
                                            EMbmVpnmanagementuiQgn_prop_vpn_log_warn_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                                            KAknsIIDQgnPropVpnLogError,
                                            fileName, 
                                            EMbmVpnmanagementuiQgn_prop_vpn_log_error, 
                                            EMbmVpnmanagementuiQgn_prop_vpn_log_error_mask ) );
  
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    CleanupStack::Pop(); // icons
#else
    CAknIconArray* icons = new(ELeave) CAknIconArray(KVpnLogNumberOfIcons);
    icons->ConstructFromResourceL(R_VPN_LOG_ICON_ARRAY);

 	iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);

#endif //__SCALABLE_ICONS

    EmptyViewTextL();
	}

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::EmptyViewTextL()
// Reads text from resource to empty log view
// and put text to the listbox.
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::EmptyViewTextL()
    {
    HBufC* stringHolder = StringLoader::LoadLC( R_VPN_LOG_EMPTY );
	iListBox->View()->SetListEmptyTextL( *stringHolder );
	CleanupStack::PopAndDestroy();  // stringHolder
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::RemoveAllFromListBoxL()
// Removes all from listbox.
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::RemoveAllFromListBoxL()
    {
    MDesCArray* itemList = iListBox->Model()->ItemTextArray();
	CDesCArray* itemArray = (CDesCArray*) itemList;
    itemArray->Reset();
    iListBox->HandleItemRemovalL();
    iListItemCount = 0;
    DrawNow();
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::DrawListBoxL(TInt aCurrentPosition, TInt aTopItem)
// Draws listbox, fetches graphic icons for markable list
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::DrawListBoxL( 
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

void CVpnManagementUiLogContainer::ShowLogsL() 
    {
    iListItemCount = 0;

    //Create CEventViewer object
    delete iEventViewer;
    iEventViewer = NULL;
    TRAPD(error, iEventViewer = CEventViewer::NewL());
    if (error != KErrNone)
        return;

    delete iLogList;
    iLogList = NULL;
    iLogList = new (ELeave) CArrayFixFlat<TEventProperties>(3);

   	CDesCArrayFlat* itemTextArray = static_cast<CDesCArrayFlat*>(iListBox->Model()->ItemTextArray());

	itemTextArray->Reset();
	iListBox->Reset();

    TEventProperties eventProperties;
    HBufC* eventText = NULL;
    
    //First item
    TBuf<100> itemName;
    TInt ret = iEventViewer->GetMostRecentEvent(eventText, eventProperties);
    if(ret==KErrNone)
        {
        CleanupStack::PushL(eventText);

        iLogList->AppendL(eventProperties);

        //append index of icon array
        itemName.AppendNum(GetIconArrayIndex(eventProperties.iCategory)); 
        
        itemName.Append(_L("\t"));

        TPtr eventTextDesc(eventText->Des());
        TInt length = eventTextDesc.Length();
        if(length > KMaxLogListLength)
            {
            eventTextDesc.Delete(KMaxLogListLength, length-KMaxLogListLength);
            }
        itemName.Append(eventTextDesc);
        CleanupStack::PopAndDestroy(); //eventText
        itemName.Append(_L("\t"));
        
        //Format time stamp for LOG view
        TBuf<2 * KMaxLengthTextDateString + 3> logEntry;
        TimeLogEntryL(logEntry, eventProperties.iTimeStamp);

        itemName.Append(logEntry);

        itemTextArray->AppendL(itemName);
	    iListBox->HandleItemAdditionL();
        iListItemCount++;
        //Get rest of events    
        while (ret==KErrNone)
            {
            TInt currentPosition = iListBox->CurrentItemIndex();
            ret = UpdateListBoxL(currentPosition);
            }

        }

    }

TInt CVpnManagementUiLogContainer::GetIconArrayIndex(TLogCategory2& aCategory)
    {
    //index of icon array
    if (aCategory == ELogInfo)
        return 0;     //index of icon array
    else if (aCategory == ELogWarning)
        return 1;    //index of icon array
    else if (aCategory == ELogError)
        return 2;    //index of icon array
    else
        return 2;
    }


void CVpnManagementUiLogContainer::TimeLogEntryL(TDes& aDes, TTime aTime)
    {
     // Date
    HBufC* dateFormat = StringLoader::LoadLC(R_QTN_DATE_USUAL_WITH_ZERO);
    TBuf<KMaxLengthTextDateString> dateString;
    aTime.FormatL(dateString, *dateFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(dateString);
    CleanupStack::PopAndDestroy(); //dateFormat

    // Time
    dateFormat = StringLoader::LoadLC(R_QTN_TIME_USUAL_WITH_ZERO);
    TBuf<KMaxLengthTextDateString> timeString;
    aTime.FormatL(timeString, *dateFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(timeString);
    CleanupStack::PopAndDestroy(); //dateFormat
      
//    TBuf<KMaxShortDateFormatSpec*2> dateBuffer; // KMaxShortDateFormatSpec is defined in e32std.h
//    TBuf<KMaxTimeFormatSpec*2> timeBuffer; // KMaxTimeFormatSpec is defined in e32std.h

    TBuf<2 * KMaxLengthTextDateString + 3> logEntry;
    
    TTime now;
	now.HomeTime();
    if ( (now.YearsFrom( aTime ).Int() > 0) ||
        (aTime.DayNoInYear() < now.DayNoInYear()) )
        {
        logEntry.Append(dateString);
        logEntry.Append(KVpnManagementUiSpaceHyphenSpace);
        }

    logEntry.Append(timeString);
    aDes = logEntry;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::UpdateListBoxL(TInt& aCurrentPosition)
// 
// ---------------------------------------------------------
//
TInt CVpnManagementUiLogContainer::UpdateListBoxL(TInt& /*aCurrentPosition*/)
	{ 
	MDesCArray* itemList = iListBox->Model()->ItemTextArray();
	CDesCArray* itemArray = ( CDesCArray* )itemList;

    TEventProperties eventProperties;
    HBufC* eventText = NULL;
    TBuf<100> itemName;
    
    //next item
    if(iEventViewer != NULL)
        {
        TInt ret = iEventViewer->GetPreviousEvent(eventText, eventProperties);
        if(ret==KErrNone)
            {
            CleanupStack::PushL(eventText);
            iLogList->AppendL(eventProperties);

            //append index of icon array       
            itemName.AppendNum(GetIconArrayIndex(eventProperties.iCategory)); 

            itemName.Append(_L("\t"));

            TPtr eventTextDesc(eventText->Des());
            TInt length = eventTextDesc.Length();
            if(length > KMaxLogListLength)
                eventTextDesc.Delete(KMaxLogListLength, length-KMaxLogListLength);
            itemName.Append(eventTextDesc);
            CleanupStack::PopAndDestroy(); //eventText
            itemName.Append(_L("\t"));

            //Format time stamp for LOG view
            TBuf<2 * KMaxLengthTextDateString + 3> logEntry;
            TimeLogEntryL(logEntry, eventProperties.iTimeStamp);

            itemName.Append(logEntry);

            itemArray->AppendL(itemName);
            iListBox->HandleItemAdditionL();
            iListItemCount++;
            }

        iParent.SetMiddleSoftKeyL(ETrue);
        return ret;
 
        }
        
    iParent.SetMiddleSoftKeyL(ETrue);
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::LogDetailsL()
// Creates the log details message query
// Appends strings from resource and eventviewer to one
// message body text and displays it.
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::LogDetailsL(TInt aIndex)
    {
    TEventProperties eventProperties;
    eventProperties = iLogList->At(aIndex);

    HBufC* message = NULL;
    HBufC* eventText = NULL;
    iEventViewer->GetEventUsingEventNumber(eventText, eventProperties.iEventNumber, eventProperties);
  	CleanupStack::PushL( eventText );

    if(eventProperties.iCategory == ELogInfo)
        {
        // Info:
        message = StringLoader::LoadLC( R_VPN_DETAIL_LOG_ENTRY_INFO );
        }

    if(eventProperties.iCategory == ELogWarning)
        {
        // Warning:
        message = StringLoader::LoadLC( R_VPN_DETAIL_LOG_ENTRY_WARNING );
        }

    if(eventProperties.iCategory == ELogError)
        {
        // Error:
        message = StringLoader::LoadLC( R_VPN_DETAIL_LOG_ENTRY_ERROR );
        }

    HBufC* messageText = HBufC::NewLC(message->Length() + eventText->Length() + 2);

    messageText->Des().Append(*message); //Append category
    messageText->Des().Append(KVpnManagementUiSpace); 
    messageText->Des().Append(*eventText); //Append log entry message

    // Heading is the date and the time of the log entry
    //Format time stamp for LOG view
    TBuf<2 * KMaxLengthTextDateString + 3> logEntry;
    TimeLogEntryL(logEntry, eventProperties.iTimeStamp);

	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*messageText);

  	CleanupStack::PopAndDestroy(3); // message Text, message, eventText

    dlg->PrepareLC(R_MESSAGE_QUERY_DETAILS_VIEW);
	dlg->QueryHeading()->SetTextL(logEntry);
	dlg->RunLD();
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::ReadResourceL(HBufC& aText, TInt aResource);
// Reads text from resource
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::ReadResourceL(HBufC& aText, TInt aResource)
	{
	HBufC* text = StringLoader::LoadLC( aResource );
	aText.Des().Append(text->Des());
	CleanupStack::PopAndDestroy();  // text
	}

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::ClearLogL()
// Asks confirmation from user.
// Clears VPN log view.
// Calls eventviewer to clear log.
// ---------------------------------------------------------
//
void CVpnManagementUiLogContainer::ClearLogL()
    {
    // Read query text from resource
    HBufC* temp;
    temp = StringLoader::LoadLC( R_VPN_QUEST_CLEAR_LOG );

    // Asks confirmation from user
    CAknQueryDialog* query = CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
    TInt retval = query->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
    CleanupStack::PopAndDestroy();  // temp
    if ( retval )
        {
        // Clear VPN log listbox
        RemoveAllFromListBoxL();

        // Calls eventviewer to clear log.
        iEventViewer->DeleteLogFile();
        }
        
        iParent.SetMiddleSoftKeyL(ETrue);
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::IconArrayResourceId
// Loads Mark/Unmark icons from resource
// ---------------------------------------------------------
//
TInt CVpnManagementUiLogContainer::IconArrayResourceId() const
    {
    return 0; //TVA
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::CreateIconArrayL
// Creates Icon Array
// ---------------------------------------------------------
//
CAknIconArray* CVpnManagementUiLogContainer::CreateIconArrayL() const
    {
	CAknIconArray* icons = new (ELeave) CAknIconArray( KGranularity );
	CleanupStack::PushL( icons );
    icons->ConstructFromResourceL( IconArrayResourceId() );
	CleanupStack::Pop(); // icons
    return icons;
    }

TFileName CVpnManagementUiLogContainer::GetIconFilenameL()
    {
    RFs& fsSession = CCoeEnv::Static()->FsSession(); 

    TDriveList driveList;
    User::LeaveIfError(fsSession.DriveList(driveList));

    TInt driveNumber;
    TDriveName drive = _L("a:");

    TFileName iconName;
    
    for (driveNumber = EDriveA, drive[0] = 'a';
         driveNumber <= EDriveZ;
         driveNumber++, drive[0]++)
        {
        if (!driveList[driveNumber])
            {
            continue;
            }

        TParse parse;
        parse.Set(drive, &KVpnLogIcons, NULL);
        iconName.Copy(parse.DriveAndPath()); 
        iconName.Append(parse.Name()); 
        iconName.Append(_L(".mif"));
                
        TEntry entry;
        if (fsSession.Entry(iconName, entry) == KErrNone)
            {
            //icon founded
            break;
            }
        }

    return iconName;
    }

// ---------------------------------------------------------
// CVpnManagementUiLogContainer::GetHelpContext
// This function is called when Help application is launched.  
// (other items were commented in a header).
// ---------------------------------------------------------
//
#ifdef __SERIES60_HELP
void CVpnManagementUiLogContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
	aContext.iMajor = KUidVpnManagementUi;
    aContext.iContext = KSET_HLP_VPN_LOG_VIEW;
    }
#endif //__SERIES60_HELP

// End of File  
