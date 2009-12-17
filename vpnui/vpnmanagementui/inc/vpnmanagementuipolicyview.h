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
* Description:   Declaration of the CVpnManagementUiPolicyView class.
*
*/


#ifndef __VPNMANAGEMENTUIPOLICYVIEW_H__
#define __VPNMANAGEMENTUIPOLICYVIEW_H__

// INCLUDES
#include <aknview.h>
#include "vpnapiwrapper.h"
#include "vpnmanagementuiviewid.h"

class CVpnManagementUiPolicyContainer;
/**
*  CVpnManagementUiPolicyView view class.
* 
*  @lib vpnmanagementui.dll
*/
class CVpnManagementUiPolicyView : public CAknView
    {
    public: 

        /**
        * Destructor.
        */
        ~CVpnManagementUiPolicyView();

        /**
        * NewL
        */
        static class CVpnManagementUiPolicyView* NewL(const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * NewLC
        */
        static class CVpnManagementUiPolicyView* NewLC(const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * Returns Policy view id
        */
        TUid Id() const;

        /**
        * Handles ClientRectChange
        */
        void HandleClientRectChange();

        /**
        * Returns iPolicyContainer 
        */
        CCoeControl* Container();

        /**
        * Handles Softkey and Options list commands
        */
        void HandleCommandL(TInt aCommand);

        /**
        * Creates the whole of policy details view
        * Appends strings from both resources and VpnAPI to one	
        * message body text and displays it.
        */
        void PolicyDetailsL(TInt aIndex);

        void ReadResourceL(HBufC& aText, TInt aResource);

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
        * To get access to policy list in policy view
        */
        CVpnManagementUiPolicyContainer* iPolicyContainer;

        /**
        * To store the position of the focus in the Policy view
        */
        TInt iCurrentPosition;

    private: // functions

        /**
        * Constructor.
        */
        CVpnManagementUiPolicyView(CVpnUiLoader& aLoader);

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

        TBool NotAssociatedToIapL(TInt aIndex, TVpnPolicyName& aPolicyName);

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


#endif // __VPNMANAGEMENTUIPOLICYVIEW_H__