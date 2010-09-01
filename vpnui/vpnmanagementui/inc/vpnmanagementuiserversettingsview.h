/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the CServerSettingsView class.
*
*/



#ifndef  __VPNMANAGEMENTUISERVERSETTINGSVIEW_H__
#define  __VPNMANAGEMENTUISERVERSETTINGSVIEW_H__

// INCLUDES
#include <aknview.h>
#include "vpnmanagementuiviewid.h"

// FORWARD DECLERATIONS
class CServerSettingsContainer;

// CLASS DECLARATION

/**
*  CServerSettingsView view class.
*
*  @lib vpnmanagementui.dll
*/
class CServerSettingsView : public CAknView
    {
    public: // functions

       /**
        * Destructor
        */
		~CServerSettingsView();

       /**
        * NewL
        */
		static CServerSettingsView* NewL(
		    const TRect& aRect, CVpnUiLoader& aLoader);

       /**
        * NewLC
        */
		static CServerSettingsView* NewLC(
		    const TRect& aRect, CVpnUiLoader& aLoader);

        /**
        * Returns Trust view id
        */
        TUid Id() const;

        /**
        * Handles ClientRectChange
        */
        void HandleClientRectChange();
       

        /**
        * Handles Softkey and Options list commands
        */
        void HandleCommandL(TInt aCommand);

  	public: // data

       /**
        * To get hold of CVpnUiLoader
        */
		CVpnUiLoader&				iLoader;

       /**
        * To get access to ServerSettingsContainer
		*/
        CServerSettingsContainer*		iContainer;

    private: // functions

        /**
        * Constructor.
        */
		CServerSettingsView(CVpnUiLoader& aLoader);

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Updates the view when opening it
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * Closes the view
        */
        void DoDeactivate();


        TInt CompleteSettingsL();

    private: // Data
    };

#endif // __VPNMANAGEMENTUISERVERSETTINGSVIEW_H__

// End of File
