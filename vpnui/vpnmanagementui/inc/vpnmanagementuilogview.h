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
* Description:   Declaration of the CVpnManagementUiLogView class.
*
*/


#ifndef __VPNMANAGEMENTUILOGVIEW_H__
#define __VPNMANAGEMENTUILOGVIEW_H__

// INCLUDES
#include <aknview.h>
#include "vpnmanagementuiviewid.h"

class CVpnManagementUiLogContainer;
class CVpnUiLoader;
/**
*  CVpnManagementUiLogView view class.
* 
*  @lib vpnmanagementui.dll
*/
class CVpnManagementUiLogView : public CAknView
    {
    public: 

        /**
        * Destructor.
        */
        ~CVpnManagementUiLogView();

        /**
        * NewL
        */
        static class CVpnManagementUiLogView* NewL(const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * NewLC
        */
        static class CVpnManagementUiLogView* NewLC(const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * Returns Log view id
        */
        TUid Id() const;

        /**
        * Handles ClientRectChange
        */
        void HandleClientRectChange();

        /**
        * Returns iLogContainer 
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

        /**
        * To get access to log list in log view
        */
        CVpnManagementUiLogContainer*	iLogContainer;

        /**
        * To store the position of the focus in the Log view
        */
        TInt iCurrentPosition;

    private: // functions

        /**
        * Constructor.
        */
        CVpnManagementUiLogView(CVpnUiLoader& aLoader);

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
        * Stores top item in the listbox
        */
        TInt iTopItem;
    };


#endif // __VPNMANAGEMENTUILOGVIEW_H__