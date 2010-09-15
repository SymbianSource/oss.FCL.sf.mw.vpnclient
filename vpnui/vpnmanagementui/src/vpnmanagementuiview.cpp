/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknViewAppUi.h>
#include <avkon.hrh>
#include <vpnmanagementuirsc.rsg>
#include <settingsinternalcrkeys.h>
#include <centralrepository.h>
#include <bautils.h>
#include <vpnmanagementui.mbg>
#include "vpnmanagementui.hrh"
#include "vpnuiloader.h"
#include "vpnmanagementuiview.h"
#include "vpnmanagementuicontainer.h"
#include "vpnmanagementuiviewid.h"
#include "log_vpnmanagementui.h"

/** MSK control Id. */
const TInt KVpnMSKControlId = 3;


// ROM folder

// Name of the MBM file containing icons
_LIT( KFileIcons, "vpnmanagementui.mbm");

// Constants
enum TVpnUiCommands 
    {
    KVpnUiPolicyViewListItemId,
    KVpnUiServerViewListItemId,
    KVpnUiLogViewListItemId
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CVpnManagementUiView::CVpnManagementUiView()
// ---------------------------------------------------------------------------
//
CVpnManagementUiView::CVpnManagementUiView()
    {
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::~CVpnManagementUiView()
// Destructor
// ---------------------------------------------------------------------------
//
CVpnManagementUiView::~CVpnManagementUiView()
    {
    LOG_("CVpnManagementUiView::~CVpnManagementUiView entered");
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        delete iContainer;
        }
    delete iLoader;    
    delete iCaption;
    LOG_("CVpnManagementUiView::~CVpnManagementUiView() exited");
    }


// ---------------------------------------------------------------------------
// CVpnManagementUiView::NewL()
// ---------------------------------------------------------------------------
//
CVpnManagementUiView* CVpnManagementUiView::NewL()
    {
    LOG_("CVpnManagementUiView::NewL() entered");
    CVpnManagementUiView* self = NewLC();
    CleanupStack::Pop();
    LOG_("CVpnManagementUiView::NewL() exited");
    return self;
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::NewLC()
// ---------------------------------------------------------------------------
//
CVpnManagementUiView* CVpnManagementUiView::NewLC()
    {
    LOG_("CVpnManagementUiView::NewLC() entered"); 
    CVpnManagementUiView* self = new ( ELeave ) CVpnManagementUiView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CVpnManagementUiView::ConstructL()
// ---------------------------------------------------------------------------
//
void CVpnManagementUiView::ConstructL()
    {
    LOG_("CVpnManagementUiView::ConstructL() entered");
    iLoader = CVpnUiLoader::NewL(iAvkonViewAppUi->ClientRect(), 
                KVpnManagementPluginUid, this );
    
    iLoader->AddResourceFileL();
    BaseConstructL( R_VPNUI_MANAGEMENT_VIEW );    
    iCaption = StringLoader::LoadL( R_VPN_MANAGEMENT_TITLE_BUF );    
    iLoader->ReleaseResource();
    
    LOG_("CVpnManagementUiView::ConstructL() exited");
    }

// ---------------------------------------------------------------------------
// void CVpnManagementUiView::GetCaptionL( TDes& aCaption ) const
// ---------------------------------------------------------------------------
void CVpnManagementUiView::GetCaptionL( TDes& aCaption ) const
    {
    LOG_("CVpnManagementUiView::GetCaptionL() entered");
    
    __ASSERT_DEBUG(iCaption != NULL, User::Invariant());
    
    if (aCaption.MaxLength() < iCaption->Length())
        {
        aCaption = iCaption->Left(aCaption.MaxLength());
        }
    else
        {
        aCaption = *iCaption;
        }
    LOG_("CVpnManagementUiView::GetCaptionL() exited");
    }

// ---------------------------------------------------------------------------
// TUid CVpnManagementUiView::Id()
// ---------------------------------------------------------------------------
//
TUid CVpnManagementUiView::Id() const
    {
    LOG_("CVpnManagementUiView::Id called");
    return KVpnManagementPluginUid; 
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::HandleCommandL()
// Handles commands directed to this class.
// ---------------------------------------------------------------------------
//
void CVpnManagementUiView::HandleCommandL(TInt aCommand)
    {   
    LOG_1("CVpnManagementUiView::HandleCommandL():%d", aCommand);
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            iLoader->ChangeViewL( KChangeViewBack );
            iLoader->ReleaseResource(ETrue);
            break;
            }
        case EAknCmdExit:
            {
            ((CAknViewAppUi*)iAvkonAppUi)->HandleCommandL( EAknCmdExit );
            break;
            }
        case EVpnUiCmdOpen:
            {
            HandleListBoxSelectionL();
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
       


// ---------------------------------------------------------------------------
// CVpnManagementUiView::HandleClientRectChange()
// ---------------------------------------------------------------------------
//
void CVpnManagementUiView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::DoActivateL()
// ---------------------------------------------------------------------------
//
void CVpnManagementUiView::DoActivateL(
   const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG_("CVpnManagementUiView::DoActivateL() entered");
    iLoader->AddResourceFileL(ETrue);
    if ( iLoader->GSViewId().iViewUid.iUid == 0 )
        {
        //must be prepared to application switch, so it stores
        //only the parent view (General Settings Main View)
        iLoader->SetGSViewId( aPrevViewId );
        
        }
    if ( iContainer )
        {
        ((CAknViewAppUi*)iAvkonAppUi)->RemoveFromViewStack( 
            *this, iContainer );
        }
    iContainer = new (ELeave) CVpnManagementUiContainer( 
        *this, *iLoader );

    iContainer->SetMopParent( this );
    iContainer->ConstructL( ClientRect() );
    iLoader->ActivateTitleL( KViewTitleManagementView );
    iLoader->PushDefaultNaviPaneL();

    ((CAknViewAppUi*)iAvkonAppUi)->AddToStackL( *this, iContainer );
    iContainer->DrawListBoxL( iCurrentItem, iTopItemIndex );

    //Listbox observer is set here, instead create listbox
    iContainer->iListBox->SetListBoxObserver(this);
    
    // Add MiddleSoftKey
   	CEikButtonGroupContainer* cbaGroup = Cba();
   	if (cbaGroup)
    	{
	    HBufC* text = StringLoader::LoadLC(R_MSK_OPEN); 
   		cbaGroup->AddCommandToStackL(
   		    KVpnMSKControlId, EVpnUiCmdOpen, text->Des());
   		CleanupStack::PopAndDestroy(text);
    	}
    LOG_("CVpnManagementUiView::DoActivateL() exited");
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CVpnManagementUiView::DoDeactivate()
    {
    LOG_("CVpnManagementUiView::DoDeactivate() entered");
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        
        delete iContainer;
    	iContainer = NULL;
        }
    LOG_("CVpnManagementUiView::DoDeactivate() exited");
    }

// ---------------------------------------------------------------------------
//  CVpnManagementUiView::HandleListBoxEventL
// ---------------------------------------------------------------------------

void CVpnManagementUiView::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType)
    {
    switch (aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleListBoxSelectionL();
            break;
        default:
           break;
        }
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::HandleListBoxSelectionL()
// ---------------------------------------------------------------------------
//
void CVpnManagementUiView::HandleListBoxSelectionL()
    {
    ASSERT(iContainer && iContainer->iListBox && iLoader);
    iCurrentItem = iContainer->iListBox->CurrentItemIndex();
    iTopItemIndex = iContainer->iListBox->TopItemIndex();
       switch (iCurrentItem)
        {
        case KVpnUiPolicyViewListItemId:
            iLoader->ChangeViewL(KChangeViewPolicy);            
            break;

        
        case KVpnUiServerViewListItemId:
            iLoader->ChangeViewL(KChangeViewServer);
            break;
        

        case KVpnUiLogViewListItemId:
            iLoader->ChangeViewL(KChangeViewLog);
            break;
        
        default:
            LOG_("CVpnManagementUiView::HandleListBoxSelectionL default");
            break;
        }
    }



void CVpnManagementUiView::NotifySynchroniseServerCompleteL(TInt aResult)
    {
    if ( aResult != KErrNone)
           LOG_1("CVpnManagementUiView::NotifySynchroniseServerCompleteL:%d", aResult);
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::HasBitmap
// ---------------------------------------------------------------------------
TBool CVpnManagementUiView::HasBitmap() const
    {
    LOG_("CVpnManagementUiView::HasBitmap() called");
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::GetBitmapL
// ---------------------------------------------------------------------------
void CVpnManagementUiView::GetBitmapL( CFbsBitmap* /*aBitmap*/, 
                                     CFbsBitmap* /*aMask*/ ) const
    {
    LOG_("CVpnManagementUiView::GetBitmapL() entered");
    //no bitmap
    User::Leave( KErrNotFound );
    LOG_("CVpnManagementUiView::GetBitmapL() exited");
    }
    
// ---------------------------------------------------------------------------
// CVpnManagementUiView::PluginProviderCategory
// ---------------------------------------------------------------------------
TInt CVpnManagementUiView::PluginProviderCategory() const
    {
    LOG_("CVpnManagementUiView::PluginProviderCategory() called");
    return EGSPluginProviderOEM;
    }    

// ---------------------------------------------------------------------------
// CVpnManagementUiView::CreateIconL
// ---------------------------------------------------------------------------    
CGulIcon* CVpnManagementUiView::CreateIconL( const TUid aIconType )
    {
    LOG_("CVpnManagementUiView::CreateIconL() entered");
    
    TFileName iconsFileName;
    TFileName dllName;
    Dll::FileName(dllName);
    TBuf<2> drive = dllName.Left(2);
    iconsFileName.Insert(0, drive);
        

    iconsFileName.Append( KDC_APP_BITMAP_DIR );
    iconsFileName.Append( KFileIcons );
    
	CGulIcon* icon;
       
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetConnVpn, 
        iconsFileName,
        EMbmVpnmanagementuiQgn_prop_set_conn_vpn,
        EMbmVpnmanagementuiQgn_prop_set_conn_vpn_mask );
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }
    LOG_("CVpnManagementUiView::CreateIconL() exited");
	return icon;
	} 

// ---------------------------------------------------------------------------
// CVpnManagementUiView::Visible
// ---------------------------------------------------------------------------    
TBool CVpnManagementUiView::Visible() const
    {
    LOG_("CVpnManagementUiView::Visible() entered");
    TInt vpnSupported( 0 );
    TInt readError(0);
    TRAPD(loadError,
        {        
        CRepository* repository = CRepository::NewL(KCRUidCommunicationSettings);
        readError = repository->Get(KSettingsVPNSupported, vpnSupported);
        delete repository;
        });
    LOG_1("CVpnManagementUiView::Visible():%d exited", vpnSupported);
    return (vpnSupported && !readError && !loadError) ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// CVpnManagementUiView::PluginUid
// ---------------------------------------------------------------------------    
TUid CVpnManagementUiView::PluginUid() const
    {
    LOG_("CVpnManagementUiView::PluginUid() called");
    return KVpnManagementPluginUid;
    }

// End of File

