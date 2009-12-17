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
* Description:   Declaration of the CVpnManagementUiServerView class.
*
*/



#ifndef __VPNMANAGEMENTUISERVERVIEW_H__
#define __VPNMANAGEMENTUISERVERVIEW_H__

// INCLUDES
#include <aknview.h>
#include "vpnmanagementuiviewid.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CVpnManagementUiServerContainer;

// CLASS DECLARATION

/**
*  CVpnManagementUiServerView view class.
* 
*  @lib vpnmanagementui.lib
*/
class CVpnManagementUiServerView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CVpnManagementUiServerView();

        /**
        * NewL
        */
        static class CVpnManagementUiServerView* NewL(const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * NewLC
        */
        static class CVpnManagementUiServerView* NewLC(const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * Returns server view id
        */
        TUid Id() const;

        /**
        * Handles ClientRectChange
        */
        void HandleClientRectChange();

        /**
        * Returns iServerContainer
        */
        CCoeControl* Container();

        /**
        * Handles Softkey and Options list commands
        */
        void HandleCommandL(TInt aCommand);

        /**
        * Sets appropriate MSK for view
        */
		void SetMiddleSoftKeyL(TBool aDrawNow);


    public: // data

        /**
        * To get hold of VpnUiLoader
        */
        CVpnUiLoader& iLoader;

        

    private: // functions

        /**
        * Constructor.
        */
        CVpnManagementUiServerView(CVpnUiLoader& aLoader);

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * From AknView, 
        * Updates the view when opening it
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From AknView,
        * Saves focus position when closing view
        */
        void DoDeactivate();

        /**
        * Updates Options list with correct items depending on 
        * whether the listbox is empty or if it has any marked items
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        
        /**
        * Sets MSK  
        */
		void SetMiddleSoftKeyLabelL(TInt aResourceId, TInt aCommandId);
        

    private: // Data

       /**
        * To get access to policy server list in Policy server view
        */
        CVpnManagementUiServerContainer* iServerContainer;

       /**
        * To store the position of the focus in the Policy server view
        */
        TInt iCurrentPosition;

       /**
        * Stores top item in the listbox
        */
        TInt iTopItem;

    };


#endif // __VPNMANAGEMENTUISERVERVIEW_H__