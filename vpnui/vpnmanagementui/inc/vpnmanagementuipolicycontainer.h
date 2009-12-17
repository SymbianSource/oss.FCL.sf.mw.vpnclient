/*
* Copyright (c) 2003 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the CVpnManagementUiPolicyContainer class.
*
*/

#ifndef __VPNMANAGEMENTUIPOLICYCONTAINER_H__
#define __VPNMANAGEMENTUIPOLICYCONTAINER_H__

// INCLUDES
#include "vpnmanagementuipolicyview.h"

   
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CVpnManagementUiPolicyContainer  container control class.
*
*  @lib vpnmanagementui.lib
*/
class CVpnManagementUiPolicyContainer : 
    public CCoeControl, 
    public MCoeControlObserver, 
    public MEikListBoxObserver
    {

    /**
    * Policy Container needs to get access to CVpnManagementUiPolicyView's DoActivateL
    */
    friend void CVpnManagementUiPolicyView::DoActivateL(
        const TVwsViewId&, TUid, const TDesC8&);

    public: // functions
        
        /**
        * Overrided Default constructor
        */
        CVpnManagementUiPolicyContainer(
            CVpnManagementUiPolicyView& aParent, CVpnUiLoader& aLoader);

        /**
        * Destructor.
        */
        ~CVpnManagementUiPolicyContainer();

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
        * Draws listbox, fetches graphic icons for markable list
        */
        void DrawListBoxL(TInt aCurrentPosition, TInt aTopItem);

        /**
        * Updates listbox after deletion, sets No VPN policies text 
        * if deleted policy was a last one.
        */
        void UpdateListBoxL(TInt& aCurrentPosition);

        /**
        * Handles listbox events
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox, TListBoxEvent aEventType);

        void InstallPoliciesL();

        void SynchroniseServerL();

        void UpdatePolicyL(TVpnPolicyId aPolicyId);

        // from MAcuApiWrapperCaller
        void NotifyUpdatePolicyCompleteL(TInt aResult);
        void NotifySynchroniseServerCompleteL(TInt aResult);
        void NotifyStepChangedL(TInt aResult);


    public: // data

        /**
        * To handle listbox
        * Cannot be private because CVpnManagementUiPolicyView uses it
        */
        CEikColumnListBox* iListBox;

        /**
        * To get hold of PolicyView
        */
        CVpnManagementUiPolicyView& iParent;

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
        CVpnManagementUiPolicyContainer();

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(
            const TRect& aRect, TInt& aCurrentPosition, TInt& aTopItem);

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
        * Handles the key events OK button, arrow keys, delete key.
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType);
       
        /**
        * Gets list of policies from VpnAPI and displays them in listbox.
        * 
        */
        void ShowPoliciesL(); 

        void PoliciesListEmptyL();



#ifdef __SERIES60_HELP
        /**
        * This function is called when Help application is launched.  
        * (other items were commented in a header).
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
#endif //__SERIES60_HELP

    protected: //data

        /**
        * If ETrue, ConstructL calls PoliciesListEmptyL() method.
        */
        TBool iCallPoliciesListEmpty;
       
    private: //data

    };

#endif // __VPNMANAGEMENTUIPOLICYCONTAINER_H__

// End of File
