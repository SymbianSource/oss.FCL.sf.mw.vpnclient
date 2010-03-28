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
* Description:   Declaration of the CVpnManagementUiLogContainer class.
*
*/




#ifndef __VPNMANAGEMENTUILOGCONTAINER_H__
#define __VPNMANAGEMENTUILOGCONTAINER_H__
// INCLUDES
#include "vpnmanagementuilogview.h"
#include "eventviewer2.h"

   
// FORWARD DECLARATIONS
class CAknIconArray;

// CLASS DECLARATION

/**
*  CVpnManagementUiLogContainer  container control class.
*
*  @lib vpnmanagementui.lib
*/
class CVpnManagementUiLogContainer : public CCoeControl, MCoeControlObserver, MEikListBoxObserver
    {

    /**
    * Policy Container needs to get access to CVpnManagementUiLogView's DoActivateL
    */
    friend void CVpnManagementUiLogView::DoActivateL(const TVwsViewId&, TUid, const TDesC8&);

    public: // functions
        
        /**
        * Overrided Default constructor
        */
        CVpnManagementUiLogContainer(CVpnManagementUiLogView& aParent, CVpnUiLoader& aLoader);

        /**
        * Destructor.
        */
        ~CVpnManagementUiLogContainer();

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
        * Gets previous log event and updates listbox.
        * @return KErrNone or KErrNotFound.
        */
        TInt UpdateListBoxL(TInt& aCurrentPosition);

        /**
        * Handles listbox events
        */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

        /**
        * Gets list of Log events and displays them in listbox.
        */
        void ShowLogsL(); 

        /**
        * Creates the log details message query 
        * Appends strings from resource and eventviewer to one
        * message body text and displays it.
        */
        void LogDetailsL(TInt aIndex);

        /**
        * Asks confirmation from user.
        * Clears VPN log view.
        * Calls eventviewer to clear log.
        */
        void ClearLogL();

    public: // data

        /**
        * To handle listbox
        * Cannot be private because CVpnManagementUiLogView uses it
        */
        CAknDoubleGraphicStyleListBox* iListBox;

        /**
        * To get hold of PolicyView
        */
        CVpnManagementUiLogView& iParent;

        /**
        * To get hold of CVpnUiLoader
        */
        CVpnUiLoader& iLoader;

        /**
        * To store the list item count
        */
        TInt iListItemCount;

    private: // functions

        /**
        * Default constructor
        */
        CVpnManagementUiLogContainer();

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect, TInt& aCurrentPosition);

        /**
        * Creates listbox, sets empty listbox text
        */
        void CreateListBoxL();

        /**
        * Reads text from resource to empty log view
        * and put text to the listbox.
        */
        void EmptyViewTextL();

        /**
        * Reads text from resource
        */
        void ReadResourceL(HBufC& aText, TInt aResource);

        /**
        * Removes all from listbox.
        */
        void RemoveAllFromListBoxL();

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
        
        /**
        * Creates the iconarray. 0th icon is info icon,
        * 1st icon is warning icon and 2nd icon is error icon.
        * @return The created iconarray. Owner is the caller.
        */
        CAknIconArray* CreateIconArrayL() const;

        /**
        * Get resource id for the iconarray.
        * @return The resource id for the icon array.
        */
        TInt IconArrayResourceId() const;

        /**
        * Get corresponding icon of category
        * @param aCategory category of logtext 
        * @return The index of icon array.
        */
        TInt GetIconArrayIndex(TLogCategory2& aCategory);

        /**
        * Formats date and time string for Log entry.
        */
        void TimeLogEntryL(TDes& aDes, TTime aTime);
        
        /**
        * Searches all drive letters and tries
        * to find vpnmanagementui.mif file.
        * @return The full name and path of the MIF file
        * containing icons for VPN Log view
        */
        TFileName GetIconFilenameL();



#ifdef __SERIES60_HELP
        /**
        * This function is called when Help application is launched.  
        * (other items were commented in a header).
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
#endif //__SERIES60_HELP

    private: //data

        CEventViewer*   iEventViewer;
        CArrayFixFlat<TEventProperties>* iLogList;
    };

#endif // __VPNMANAGEMENTUILOGCONTAINER_H__

// End of File
