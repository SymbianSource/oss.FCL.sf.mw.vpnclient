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
* Description:   Declaration of the CVpnManagementUiServerContainer class.
*
*/

#ifndef __VPNMANAGEMENTUISERVERCONTAINER_H__
#define __VPNMANAGEMENTUISERVERCONTAINER_H__

// INCLUDES
#include "vpnmanagementuiserverview.h"

   
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CVpnManagementUiServerContainer  container control class.
*
*  @lib vpnmanagementui.lib
*/
class CVpnManagementUiServerContainer : public CCoeControl, 
                                        public MCoeControlObserver, 
                                        public MEikListBoxObserver
    {
    /**
    * Policy Container needs to get access to CVpnManagementUiServerView's DoActivateL
    */
    friend void CVpnManagementUiServerView::DoActivateL(const TVwsViewId&, TUid, const TDesC8&);

    public: // functions
        
        /**
        * Overrided Default constructor
        */
        CVpnManagementUiServerContainer(CVpnManagementUiServerView& aParent, CVpnUiLoader& aLoader);

        /**
        * Destructor.
        */
        ~CVpnManagementUiServerContainer();

        /**
        * Overrided ActivateL function from base class CCoeControl
        */
        virtual void ActivateL();

        /**
        * Change focus
        */
        void FocusChanged(TDrawNow aDrawNow);

		/**
		* From CCoeControl
		*/
		void HandleResourceChange( TInt aType );

        /**
        * Draws listbox
        */
        void DrawListBoxL(TInt aCurrentPosition, TInt aTopItem);

        /**
        * Refresh listbox after VPN policy server deletion.
        */
        void UpdateListBoxL(TInt& aCurrentPosition);

        /**
        * Ask confirmation to delete server.
        * Calls AcuAgent API's DeleteServer and updates listbox and iServerList
        * Show an information note if server is currently in use and
        * can't be deleted.
        */
        void DeleteServerL(TInt aIndex);

        /**
        * Calls AcuAgent API's SynchroniseServerL
        */
        void SynchroniseServerL(TInt aIndex);

        // from MAcuApiWrapperCaller
        void NotifyUpdatePolicyCompleteL(TInt aResult);
        void NotifySynchroniseServerCompleteL(TInt aResult);
        void NotifyStepChangedL(TInt aResult);

        void  HandleListBoxEventL (CEikListBox *aListBox, TListBoxEvent aEventType);

    public: // data

        /**
        * To handle listbox
        */
        CEikColumnListBox* iListBox;

        /**
        * To get hold of ServerView
        */
        CVpnManagementUiServerView& iParent;

        /**
        * To get hold of CVpnUiLoader
        */
        CVpnUiLoader& iLoader;

        /**
        * To store the list item count
        */
        TInt iListItemCount;

        /**
        * If ETrue, Show Waitnote.
        */
        TBool iShowWaitNote;

    private: // functions

        /**
        * Default constructor
        */
        CVpnManagementUiServerContainer();

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect, TInt& aCurrentPosition, TInt& aTopItem);

       /**
        * Creates listbox, sets empty listbox text
        */
        void CreateListBoxL();

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
        * Handles the key events OK button, arrow keys.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
        
        void ShowServersL(); 


#ifdef __SERIES60_HELP
        /**
        * This function is called when Help application is launched.  
        * (other items were commented in a header).
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
#endif //__SERIES60_HELP

    private: //data

    };

#endif // __VPNMANAGEMENTUISERVERCONTAINER_H__

// End of File
