/*
* Copyright (c) 2003 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN management main view
*
*/



// INCLUDE FILES
#include <aknnotewrappers.h> // for warning & information notes
#include <AknWaitDialog.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmpluginvpndef.h>
#include <cmapplicationsettingsui.h>
#include <commdb.h>
#include <sysutil.h>
#include <ErrorUI.h>
#include <bautils.h>
#include <e32def.h>
#include <vpnmanagementuirsc.rsg>
#include "vpnuiloader.h"
#include "vpnmanagementuiview.h"
#include "vpnmanagementuipolicyview.h"
#include "vpnmanagementuiserverview.h"
#include "vpnmanagementuilogview.h"
#include "vpnmanagementuiserversettingsview.h"
#include "version.h"
#include "vpnmanagementuiserversettingscontainer.h"

#include "log_vpnmanagementui.h"

// CONSTANTS


using namespace CMManager;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVpnUiLoader::CVpnUiLoader()
// Constructor with parent
// ---------------------------------------------------------
//
CVpnUiLoader::CVpnUiLoader( CVpnManagementUiView* aUiView ):
    iVpnManagementUiView(aUiView),
    iResourceLoader(*iCoeEnv)
    {
    LOG_("CVpnUiLoader::CVpnUiLoader()");
    ASSERT( aUiView );
    }

// ---------------------------------------------------------
// CVpnUiLoader::~CVpnUiLoader()
// Destructor
// ---------------------------------------------------------
//
CVpnUiLoader::~CVpnUiLoader()
    {
    LOG_("CVpnUiLoader::~CVpnUiLoader()");
    iVpnManagementUiView = NULL;
     
    if(iVersionInfoInNaviPane)
        {
        delete iVersionInfoInNaviPane; 
        }
    delete iVpnApiWrapper;
    delete iWaitDialog;
	iResourceLoader.Close();
    }

// ---------------------------------------------------------
// CVpnUiLoader* CVpnUiLoader::NewL()
// ---------------------------------------------------------
//
CVpnUiLoader* CVpnUiLoader::NewL(
    const TRect& aRect, TUid aViewId,
    CVpnManagementUiView* aUiView )
    {
    CVpnUiLoader* self = NewLC( aRect, aViewId, aUiView );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CVpnUiLoader* CVpnUiLoader::NewLC()
// ---------------------------------------------------------
//
CVpnUiLoader* CVpnUiLoader::NewLC(
    const TRect& aRect, TUid aViewId,
    CVpnManagementUiView* aUiView )
    {
    CVpnUiLoader* self = new ( ELeave ) CVpnUiLoader( aUiView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aViewId );
    return self;
    }

// ---------------------------------------------------------
// CVpnUiLoader::ConstructL()
// ---------------------------------------------------------
//
void CVpnUiLoader::ConstructL( const TRect& aRect, TUid aViewId )
    {
    LOG_("CVpnUiLoader::ConstructL() - begin");
    
    AddResourceFileL();
    //Initialization
    iBackFromServerDefinition = EFalse;
    iNewServerDefinition = EFalse;
    iPolicyUpdate = EFalse;

    CEikStatusPane* statusPane = STATIC_CAST(
        CAknAppUi*, iEikonEnv->EikAppUi())->StatusPane();

    // Fetch pointer to the default navi pane control
    iNaviPane =  static_cast<CAknNavigationControlContainer*> (
        statusPane->ControlL( TUid::Uid(EEikStatusPaneUidNavi) ) );
    
	iPreviousAppViewId = aViewId;
	CreateWindowL();
    
    User::LeaveIfError( iVpnExtApi.Connect() );
	SetRect(aRect);

    LOG(Log::Printf(_L("CVpnUiLoader::ConstructL() - end\n")));
    }

// ---------------------------------------------------------
// CVpnUiLoader::FocusChanged(TDrawNow aDrawNow)
// Added here because FocusChanged need to be included
// in every control derived from CCoeControl that can have listbox 
// ---------------------------------------------------------
//
void CVpnUiLoader::FocusChanged(TDrawNow aDrawNow)
	{
	if (iListBox) 
		{
		iListBox->SetFocus(IsFocused(), aDrawNow);
		}
	}

// ---------------------------------------------------------
// CVpnUiLoader::ChangeViewL(TInt aNewTab, TInt aSelectionIndex)
// Called when the view tab is changed
// ---------------------------------------------------------
//
void CVpnUiLoader::ChangeViewL(TInt aNewTab, TInt aSelectionIndex)
    {
    LOG_1("CVpnUiLoader::ChangeViewL():%d", aNewTab);
	switch (aNewTab)
		{

		case KChangeViewPrevious:
            {
            TVwsViewId currentViewId;
            ((CAknViewAppUi*)iAvkonAppUi)->GetActiveViewId(currentViewId);
            
            if(iPreviousViewId == currentViewId.iViewUid)
                {
                ((CAknViewAppUi*)iAvkonAppUi)->RegisterViewL(*iVpnManagementUiView);
				((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL( KVpnManagementPluginUid );
                }
            else
                {
                ((CAknViewAppUi*)iAvkonAppUi)->RegisterViewL(*iVpnManagementUiView);
				((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL( 
                iPreviousViewId );
                }
			break;
            }
		case KChangeViewBack:
		    ((CAknViewAppUi*)iAvkonAppUi)->RemoveView(KVpnManagementUiPolicyViewId);
		    iPolicyViewVisited = EFalse;
			((CAknViewAppUi*)iAvkonAppUi)->RemoveView(KVpnManagementUiLogViewId);
			iLogViewVisited = EFalse;
			((CAknViewAppUi*)iAvkonAppUi)->RemoveView(KVpnManagementUiParametersViewId);
			iServerSettingsViewVisited = EFalse;
			((CAknViewAppUi*)iAvkonAppUi)->RemoveView(KVpnManagementUiServerViewId);
			iServerViewVisited = EFalse;
			((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL( iGsViewId.iViewUid );
            if(iObserver)
                {
                iObserver->UiComplete(KUirEventNone);
                }
			break;

		case KChangeViewPolicy: //Policy view
            {
            // the current view is saved so we 
            // know where to come back
            TVwsViewId localCurrentViewId;
			((CAknViewAppUi*)iAvkonAppUi)->GetActiveViewId( 
                localCurrentViewId );

			iPreviousViewId = localCurrentViewId.iViewUid;
			if ( iPolicyViewVisited == EFalse)
			    {
                TRect rect;
                iVpnManagementUiPolicyView = CVpnManagementUiPolicyView::NewL( 
		            rect, *this);
			    ((CAknViewAppUi*)iAvkonAppUi)->AddViewL(iVpnManagementUiPolicyView);
			    }
			iPolicyViewVisited=ETrue;
			((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL(
                  KVpnManagementUiPolicyViewId );
			break;
            }
		case KChangeViewServer:  //Policy server view  
            {
            // the current view is saved so we 
            // know where to come back
            TVwsViewId localCurrentViewId;
			((CAknViewAppUi*)iAvkonAppUi)->GetActiveViewId( 
                localCurrentViewId );

			iPreviousViewId = localCurrentViewId.iViewUid;
			if ( iServerViewVisited == EFalse)
			    {
			    TRect rect;
			    iVpnManagementUiServerView = CVpnManagementUiServerView::NewL( rect, *this);
			    ((CAknViewAppUi*)iAvkonAppUi)->AddViewL(iVpnManagementUiServerView);
			    } 
			iServerViewVisited = ETrue;
			((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL( 
                  KVpnManagementUiServerViewId ); 
			break;
            }
        case KChangeViewLog: //Log view
            {
            // the current view is saved so we 
            // know where to come back
            TVwsViewId localCurrentViewId;
			((CAknViewAppUi*)iAvkonAppUi)->GetActiveViewId( 
                localCurrentViewId );

			iPreviousViewId = localCurrentViewId.iViewUid;
			if ( iLogViewVisited == EFalse)
			    {
                TRect rect;
                iVpnManagementUiLogView = CVpnManagementUiLogView::NewL( rect, *this);
			    ((CAknViewAppUi*)iAvkonAppUi)->AddViewL(iVpnManagementUiLogView);
			    }
			iLogViewVisited=ETrue;
			((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL(
                  KVpnManagementUiLogViewId );
            break;
			}

        case KChangeViewSettings: //VPN policy server parameters view
			{
			// the current view is saved so that close settings 
            // knows where to come back
			TVwsViewId localCurrentViewId;
			((CAknViewAppUi*)iAvkonAppUi)->GetActiveViewId( 
                localCurrentViewId );

			iPreviousViewId = localCurrentViewId.iViewUid;
            //Put selected server to CustomMessageId
			if ( iServerSettingsViewVisited == EFalse)
			   {
			    TRect rect;
			    iVpnManagementUiParametersView = CServerSettingsView::NewL( rect, *this);
			    ((CAknViewAppUi*)iAvkonAppUi)->AddViewL(iVpnManagementUiParametersView);
			   }
			iServerSettingsViewVisited = ETrue;
			((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL(
                  KVpnManagementUiParametersViewId,
                TUid::Uid( aSelectionIndex), KNullDesC8 );
			break;
			}

		default:
			break;	
		}
    }

// ----------------------------------------------------
// CVpnUiLoader::ActivateTitleL(TInt aCurrentTitle)
// Activates the VPN policies, VPN policy servers and
// VPN log views title
// ----------------------------------------------------
//
void CVpnUiLoader::ActivateTitleL(TInt aCurrentTitle)
    {
	CEikStatusPane* sp = STATIC_CAST( 
        CAknAppUi*, iEikonEnv->EikAppUi())->StatusPane();
	// Fetch pointer to the default title pane control
	CAknTitlePane* title = STATIC_CAST( 
        CAknTitlePane*, sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)) );
	TResourceReader reader;

    if ( aCurrentTitle == KViewTitleManagementView )
	    {
		iCoeEnv->CreateResourceReaderLC( reader, R_VPN_MANAGEMENT_TITLE );
		}
	if ( aCurrentTitle == KViewTitlePolicyView )
		{
		iCoeEnv->CreateResourceReaderLC( reader, R_VPN_POLICIES_TITLE );
		}
    if ( aCurrentTitle == KViewTitleServerView )
		{
		iCoeEnv->CreateResourceReaderLC( reader, R_VPN_POLICY_SERVERS_TITLE );
		}
    if ( aCurrentTitle == KViewTitleLogView )
		{
		iCoeEnv->CreateResourceReaderLC( reader, R_VPN_LOG_TITLE );
		}

	title->SetFromResourceL( reader );
	CleanupStack::PopAndDestroy();  // reader
    }

void CVpnUiLoader::ActivateTitleL(TInt aCurrentTitle, TDes& aText)
    {
	CEikStatusPane* sp = STATIC_CAST( 
        CAknAppUi*, iEikonEnv->EikAppUi())->StatusPane();
	// Fetch pointer to the default title pane control
	CAknTitlePane* title = STATIC_CAST( 
        CAknTitlePane*, sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)) );

    if ( aCurrentTitle == KViewTitleParametersView )
        {
        title->SetTextL(aText);
        }
    }
// ----------------------------------------------------
// CVpnUiLoader::ActivateNaviTextL()
// Activates the VPN log view Navipane text (Version information)
// ----------------------------------------------------
//
void CVpnUiLoader::ActivateNaviTextL()
    {
  	// version information in navi pane text shown in VPN Log view
    //TBuf<KVersionLine> version(KVersion);
    HBufC* naviText = StringLoader::LoadLC( 
        R_VPN_NAVI_CLIENT_VERSION, KVersion/*version*/ );
    TPtr naviDes=naviText->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(naviDes);
    iVersionInfoInNaviPane = iNaviPane->CreateMessageLabelL(*naviText);
    CleanupStack::PopAndDestroy(); //naviText
    }

// ---------------------------------------------------------
// CVpnUiLoader::PushNaviPaneL
// ---------------------------------------------------------
//
void CVpnUiLoader::PushNaviPaneL()
    {
    iNaviPane->PushL( *iVersionInfoInNaviPane );
    }

// ---------------------------------------------------------
// CVpnUiLoader::PopNaviPane
// ---------------------------------------------------------
//
void CVpnUiLoader::PopNaviPane()
    {
    if(iVersionInfoInNaviPane)
        {
        delete iVersionInfoInNaviPane; //A control is popped from stack
        iVersionInfoInNaviPane = NULL;
        }
    }

// ---------------------------------------------------------
// CVpnUiLoader::PushDefaultNaviPaneL
// ---------------------------------------------------------
//
void CVpnUiLoader::PushDefaultNaviPaneL()
    {
    iNaviPane->PushDefaultL();
    }

// ---------------------------------------------------------
// CVpnUiLoader::HandleControlEventL(
//      CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
// Cannot be changed to non-leaving function. 
// L-function is required by the class definition, even if empty.
// ---------------------------------------------------------
//
void CVpnUiLoader::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// ---------------------------------------------------------
// CVpnUiLoader::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CVpnUiLoader::OfferKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
	{
	// Listbox takes all event even if it doesn't use them
	return EKeyWasNotConsumed; 
	}

// ----------------------------------------------------------
// AddResourceFileL()
// ----------------------------------------------------------
//
void CVpnUiLoader::AddResourceFileL(TBool aKeepOpen)
	{
	if (!iKeepOpen)
	    {
    	iKeepOpen = aKeepOpen;    
        _LIT(KResourceFile, "\\resource\\vpnmanagementuirsc.rsc");	
	TFileName resourceFileName(KResourceFile);
    TFileName dllName;
    Dll::FileName(dllName);
    TBuf<2> drive = dllName.Left(2);
    resourceFileName.Insert(0, drive);

    // To enable loading of e.g. vpnmanagementuirsc.r**
    BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(), resourceFileName);
	iResourceLoader.OpenL(resourceFileName);
        LOG_("CVpnUiLoader::AddResourceFileL(");
	    }
	}
// ----------------------------------------------------------
// ReleaseResource()
// ----------------------------------------------------------
//
void CVpnUiLoader::ReleaseResource(TBool aForceClose)
    {
    if(!iKeepOpen || aForceClose) 
        {
        LOG_("CVpnUiLoader::ReleaseResource()");
        iKeepOpen = EFalse;
        iResourceLoader.Close();
        }
    }	
// ---------------------------------------------------------
// CVpnUiLoader::DialogDismissedL
// ---------------------------------------------------------
//
void CVpnUiLoader::DialogDismissedL( TInt /*aButtonId*/ )
    {
    // The UI is left in an inactive state if the progress dialog is cancelled
    // very quickly. Thus, we ensure that the UI ends up in an active
    // state by activating the view that should be active.
    TVwsViewId activeViewId;
    ((CAknViewAppUi*)iAvkonAppUi)->GetActiveViewId(activeViewId);
    ((CAknViewAppUi*)iAvkonAppUi)->ActivateLocalViewL(activeViewId.iViewUid);
    
    if ( !iShowWaitNote )
        {
        iVpnApiWrapper->CancelSynchronise( );
        }
    }

// ---------------------------------------------------------
// CVpnUiLoader::ShowWaitNoteL
// ---------------------------------------------------------
//
void CVpnUiLoader::ShowWaitNoteL()
    {
    // Initialization (before the progress dialog is shown)
    iWaitNoteStartTime.UniversalTime();
            
    if ( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
    iWaitDialog = new ( ELeave ) CAknWaitDialog
                ( REINTERPRET_CAST(CEikDialog**,&iWaitDialog),ETrue );
    iWaitDialog->PrepareLC( R_VPN_WAIT_NOTE );
    iWaitDialog->SetTone( CAknNoteDialog::ENoTone );
    iWaitDialog->SetCallback( this );

    SetTextL();

    iWaitDialog->RunLD();
    iWaitDialog->DrawNow();
    }

void CVpnUiLoader::DeleteWaitNoteL()
    {
    TInt error;
    // Dismiss wait dialog
    TRAP(error, iWaitDialog->ProcessFinishedL()); // deletes the wait dialog
    if (error != KErrNone)
        {
        // on error destroy wait note by force.
        delete iWaitDialog;
        }
    iWaitDialog = NULL;
    iShowWaitNote = EFalse;
    }

void CVpnUiLoader::SetTextL()
    {
	LOG(Log::Printf(_L("CVpnUiLoader::SetTextL()\n")));

       
        HBufC* string = StringLoader::LoadLC( R_VPN_WAIT_IMPORTING_POLICY );
        iWaitDialog->SetTextL( *string ); 
        CleanupStack::PopAndDestroy( string );
   
    }


void CVpnUiLoader::GetSelectionNameL( TDes& aText )
    {
    TCmSettingSelection selection = TCmSettingSelection();
   
    //Reset update operation for Connecting via note
    iPolicyUpdate = EFalse;

    HBufC* name = NULL;
    TInt err = KErrNone;    
	if ( selection.iResult ==  EDestination )
		{
		TRAP( err, (name = CServerSettingsContainer::GetDestinationNameL( selection.iId )));
		}
	else if ( selection.iResult == EConnectionMethod )
		{
		TRAP( err, (name = CServerSettingsContainer::GetConnectionMethodNameL( selection.iId )));
		}
	
	if ( name )
		{
		aText.Copy(*name);
		}	
	delete name;
	name = NULL;
    }

    
// ---------------------------------------------------------
// CVpnUiLoader::FFSSpaceBelowCriticalLevelL
// ---------------------------------------------------------
//
TBool CVpnUiLoader::FFSSpaceBelowCriticalLevelL
        ( TBool aShowErrorNote, TInt aBytesToWrite /*=0*/ )
    {
    LOG_("CVpnUiLoader::FFSSpaceBelowCriticalLevelL() - begin");

    TBool ret( EFalse );
    if ( SysUtil::FFSSpaceBelowCriticalLevelL
                ( &(CCoeEnv::Static()->FsSession()), aBytesToWrite ) )
        {
        ret = ETrue;
        if ( aShowErrorNote )
            {
            CErrorUI* errorUi = CErrorUI::NewLC( *(CCoeEnv::Static()) );
            errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
            CleanupStack::PopAndDestroy();  // errorUi
            }
        }
    
    LOG( Log::Printf(_L("CVpnUiLoader::FFSSpaceBelowCriticalLevelL() - end\n")));

    return ret;
    }
    
// ---------------------------------------------------------
// CVpnUiLoader::GetVpnManagementUiView()
// 
// ---------------------------------------------------------
//
CAknView* CVpnUiLoader::GetVpnManagementUiView() const
    {
	return iVpnManagementUiView;
    }

// ---------------------------------------------------------
// CVpnUiLoader::VpnApiWrapperL()
// ---------------------------------------------------------
//
CVpnApiWrapper& CVpnUiLoader::VpnApiWrapperL()
    {
    LOG_("CVpnUiLoader::VpnApiWrapperL()");
    if (!iVpnApiWrapper)
        iVpnApiWrapper = CVpnApiWrapper::NewL();
    return *iVpnApiWrapper;
    }
    
// ---------------------------------------------------------
// CVpnUiLoader::GsViewId()
// Called when the view tab is changed
// ---------------------------------------------------------
//
TVwsViewId CVpnUiLoader::GSViewId() const
    {
    return iGsViewId;
    }

// ---------------------------------------------------------
// CVpnUiLoader::SetGsViewId( TVwsViewId )
// Called when the view tab is changed
// ---------------------------------------------------------
//
void CVpnUiLoader::SetGSViewId( TVwsViewId aGsViewUid )
    {
    iGsViewId = aGsViewUid;
    }
    
// End of File  
