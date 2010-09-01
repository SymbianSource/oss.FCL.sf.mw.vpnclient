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
* Description:   Declaration of the CVpnManagementUiContainer class.
*
*/



#ifndef __VPNMANAGEMENTUICONTAINER_H__
#define __VPNMANAGEMENTUICONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include "vpnmanagementuiview.h"
 
// FORWARD DECLARATIONS
class CEikTextListBox;

// CLASS DECLARATION

/**
*  CVpnManagementUiContainer  container control class.
*  
*/
class CVpnManagementUiContainer : public CCoeControl, public MCoeControlObserver
    {
    /**
    * Container needs to get access to CVpnManagementUiView's DoActivateL
    */
    friend void CVpnManagementUiView::DoActivateL(const TVwsViewId&, TUid, const TDesC8&);

    public: // Constructors and destructor
        
        /**
        * Overrided Default constructor
        */
        CVpnManagementUiContainer(CVpnManagementUiView& aParent, CVpnUiLoader& aLoader);

        /**
        * Destructor.
        */
        ~CVpnManagementUiContainer();

        /**
        * Draws listbox
        */
        void DrawListBoxL(TInt aCurrentPosition, TInt aTopItem);
 
 		/**
		* From CCoeControl
		*/
		void HandleResourceChange( TInt aType );
           

    public: // New functions

    public: // Functions from base classes

    private: // Functions from base classes

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * Handles control events
        *
        * event handling section
        * e.g Listbox events
        */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
        /**
        * Handles the key events OK button, arrow keys, delete key.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

        void TitlePaneTextsL();

        /**
        * Creates listbox, sets empty listbox text
        */
        void CreateListBoxL();

        void ShowItemsL(); 

        #ifdef __SERIES60_HELP
        /**
        * This function is called when Help application is launched.  
        * (other items were commented in a header).
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
        #endif //__SERIES60_HELP

    public: //data
        
        CEikTextListBox* iListBox;  

        /**
        * To get hold of View
        */
        CVpnManagementUiView& iParent;

        /**
        * To get hold of CVpnUiLoader
        */
        CVpnUiLoader& iLoader;

        /**
        * To store the list item count
        */
        TInt iListItemCount;

    };

#endif // __VPNUIMANAGEMENTCONTAINER_H__

// End of File
