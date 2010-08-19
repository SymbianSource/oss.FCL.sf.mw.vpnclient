/*
* Copyright (c) 2003 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the CVpnManagementUiView class.
*
*/



#ifndef __VPNMANAGEMENTUIVIEW_H__
#define __VPNMANAGEMENTUIVIEW_H__

// INCLUDES
#include <aknview.h>
#include <eikclb.h>
#include <ConeResLoader.h>
#include <gsplugininterface.h>
#include "vpnmanagementuiviewid.h"
#include "vpnapiwrapper.h"
#include "vpnuiloader.h"

#ifdef __SERIES60_HELP
#include    <hlplch.h>   // For HlpLauncher 
#endif //__SERIES60_HELP


// FORWARD DECLARATIONS
class CVpnManagementUiContainer;

// CLASS DECLARATION

/**
 *  CVpnManagementUiView view class.
 *
 *  @lib (internal) vpnmanagementui.lib
 *  @since S60 v3.0
 */
class CVpnManagementUiView : public CGSPluginInterface, public MEikListBoxObserver, public MVpnApiWrapperCaller
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CVpnManagementUiView();

        /**
        * NewL
        */
        static class CVpnManagementUiView* NewL();

        /**
        * NewLC
        */
        static class CVpnManagementUiView* NewLC();

       

    public: // Functions from base classes

        /**
        * Method for getting caption of this plugin. This should be the
        * localized name of the settings view to be shown in parent view.
        *
        * @param aCaption pointer to Caption variable
        */
        virtual void GetCaptionL( TDes& aCaption ) const;    
        
        /**
        * Returns Management view id
        */
        TUid Id() const;

        /**
        * Handles ClientRectChange
        */
        void HandleClientRectChange();

        /**
        * Returns iContainer 
        */
        CCoeControl* Container();

        /**
        * Handles Softkey and Options list commands
        */
        void HandleCommandL(TInt aCommand);

        
        void NotifySynchroniseServerCompleteL(TInt aResult);
    public:
    
        /**
        * from CGSPluginInterface
        */
        
        /**
        * Method for checking plugin's Uid. Uid identifies this GS plugin. Use
        * same Uid as the ECOM plugin implementation Uid.
        *
        * @return PluginUid
        */
        TUid PluginUid() const;

        /**
        * Method for checking, if item has bitmap icon to be shown in list
        *
        * @return ETrue if plugin has icon bitmap
        * @return EFalse if plugin does not have a bitmap
        */
        TBool HasBitmap() const;

        /**
        * Method for reading bitmap icon.
        *
        * @param aBitmap plugin bitmap
        * @param aMask plugin icon bitmap mask
        */
        void GetBitmapL( CFbsBitmap* aBitmap, CFbsBitmap* aMask ) const;

        /**
        * Method for reading the ID of the plugin provider category. See 
        *
        * @return Plugin provider category ID defined by 
        *         
        */
        TInt PluginProviderCategory() const;
        
        /**
        * Method for checking, if plugin should be visible and used in GS FW.
        * (for example shown in listbox of the parent view).
        *
        * On default plugin is visible. Overwrite this function to enable or
        * disable your plugin dynamically.
        *
        * @return ETrue if plugin should be visible in GS.
        * @return EFalse if plugin should not be visible in GS.
        */
        virtual TBool Visible() const;
        
        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used. Ownership of the created icon
        * is transferred to the caller.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem  -   ListBox item icon.
        * KGSIconTypeTab      -   Tab icon.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon. NOTE: Ownership of this icon is
        *         transferred to the caller.
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        /**
        * From MUiRunnerObserver
        */                    
        void UiComplete( TInt aUirEvent );
                                  
        /**
        * Handles the actual selected list box item
        */
        void HandleListBoxSelectionL(); 
        
        /**
        * Shows a information note.
        * @param aResourceId Resource id of the showed text .
        */
        void ShowInfoNoteL( TInt aResourceId );        

    public: // data

        /**
        * To get hold of VpnUiLoader
        */
        CVpnUiLoader* iLoader;

        /**
        * To get access to list in management view
        */
        CVpnManagementUiContainer* iContainer;

        /**
        * To store the position of the focus in the management view
        */
        TInt iCurrentPosition;
     
    private: // functions

        /**
        * Constructor.
        */
        CVpnManagementUiView();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * From AknView, 
        * Updates the view when opening it
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From AknView,
        * Saves focus position when closing view
        */
        void DoDeactivate();

        void HandleListBoxEventL(CEikListBox* aListBox,
            TListBoxEvent aEventType);

    private: // Data
        
        TInt iCurrentItem; // currently selected listbox item
        TInt iTopItemIndex; // first item in the listbox
    };

#endif // __VPNMANAGEMENTUIVIEW_H__

// End of File
