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
* Description:   VPN management main view
*
*/

#ifndef  __VPNUILOADER_H__
#define  __VPNUILOADER_H__

// INCLUDES
#include <AknProgressDialog.h>
#include <ConeResLoader.h>
#include "vpnapiwrapper.h"
#include "vpnmanagementuidefs.h"
#include "uirunner.h"

// CONSTANTS
/**
 * Specifies the type of the waitnote text.
 */
enum
    {
    EConnectingVia = 0x0,
    EProcessingStepN = 0x1
    };

const TInt KSecondsToShowConnectingVia = 3;
   
// FORWARD DECLARATIONS

// CLASS DECLARATION
class CAknWaitDialog;
class CVpnManagementUiView;
class CVpnManagementUi;
/**
*  CVpnUiLoader view control class.
*
*  @lib vpnmanagementui.lib
*/
class CVpnUiLoader : public CCoeControl, public MCoeControlObserver, 
    public MProgressDialogCallback
    {
    public: // functions 

        /**
        * Destructor.
        */
        ~CVpnUiLoader();

        /**
         * NewL
         *
         * @since S60 3.0
         * @param aRect ?description
         * @param aViewId ?description
         * @return self
         */
        static class CVpnUiLoader* NewL(
            const TRect& aRect, TUid aViewId, CVpnManagementUiView* aUiView );

        /**
        * NewLC
        */
        static class CVpnUiLoader* NewLC(
            const TRect& aRect, TUid aViewId, CVpnManagementUiView* aUiView );

        /**
        * ChangeViewL
        * @param aNewTab change to given tab
        */
        void ChangeViewL(TInt aNewTab, TInt aSelectionIndex = -1); 
        
        /**
        * Activates the VPN policies, VPN policy servers and
        * VPN log views title
        */
        void ActivateTitleL(TInt aCurrentTitle);

        void ActivateTitleL(TInt aCurrentTitle, TDes& aText);

        /**
        * Activates the VPN log view Navipane text
        */
        void ActivateNaviTextL();

        /**
        * Added here because FocusChanged need to be included
        * in every control derived from CCoeControl that can have listbox 
        */
        void FocusChanged(TDrawNow aDrawNow);

        /**
        * Push the NaviPane to stack.
        */
        void PushNaviPaneL();

        /**
        * Pop the NaviPane from stack.
        */
        void PopNaviPane();

        /**
        * Push the DefaultNaviPane (empty) to stack.
        */
        void PushDefaultNaviPaneL();

        /**
        * Shows the error note 
        */
        void ShowErrorNoteL();

        /**
        * Get selection name to the connecting via wait note 
        */
        void GetSelectionNameL(TDes& aText);

        /**
        * Shows the Connecting via or Processing step wait note 
        */
        void ShowWaitNoteL();

        /**
        * Deletes the wait note 
        */
        void DeleteWaitNoteL();

        /**
        * Set text to wait note 
        */
        void SetTextL();

        //from MProgressDialogCallback
        void DialogDismissedL( TInt aButtonId );

		/**
     	* Check if Disk space goes below critical level.
     	* @param aShowErrorNote If ETrue, a warning note is shown.
     	* @param aBytesToWrite Amount of disk space to be allocated.
     	* @return ETrue if allocation would go below critical level.
     	*/
    	TBool FFSSpaceBelowCriticalLevelL( TBool aShowErrorNote,
                                           TInt aBytesToWrite /*=0*/ );

        /**
        * Returns pointer to main management view 
        */
		CAknView* GetVpnManagementUiView() const;
		
		/**
		*  CVpnApiWrapper
		*/
		CVpnApiWrapper& VpnApiWrapperL();	
		
		/**
		* Returns the id to the GS view that launched us
		*/
        TVwsViewId GSViewId() const;			
		
		/**
		* Sets the id to the GS view that launched us
		* @param aGsViewId The id to the GS view
		*/
        void SetGSViewId( TVwsViewId aGsViewId );			

    public: //data

        /**
        * Added here because FocusChanged need to be included
        * in every control derived from CCoeControl that can have listbox 
        */
        CEikColumnListBox* iListBox;

        /**
        * If ETrue, We are coming back from server parameters view.
        */
        TBool iBackFromServerDefinition;

        /**
        * If ETrue, new Server definition has created.
        */
        TBool iNewServerDefinition;
        /**
        * If ETrue, show Define policy server query when empty list.
        */
        TBool iShowDefineQuery;
 
        MUiRunnerObserver* iObserver;

        CAknWaitDialog*     iWaitDialog;

        TInt iCurrentPolicyIndex;
        TInt iCurrentServerIndex;
        TBool iPolicyUpdate;
        TBuf<KMaxUiSelectionNameLength> iSelectionName;
    
    private: // data
        
        /**
        * Used to wrap asynchronous calls
        */
        CVpnApiWrapper*          iVpnApiWrapper;        
        
    private: // implementation

        void ConstructL(const TRect& aRect, TUid aViewId);
        CVpnUiLoader(CVpnManagementUiView* aUiView);
        void HandleControlEventL( CCoeControl* aControl,
            TCoeEvent aEventType );
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType);
    public:    
        void AddResourceFileL(TBool aKeepOpen = EFalse);
        void ReleaseResource(TBool aForceClose = EFalse);
   
    private: //data

        /**
        * For Navipane
        */
        CAknNavigationDecorator* iVersionInfoInNaviPane;

        /**
        * For Navipane
        */
        CAknNavigationControlContainer* iNaviPane;


        /**
        * To store the previous view id 
        */
        TUid iPreviousViewId;

        /**
        * To store the previous view id  
        */
        TUid iPreviousAppViewId;

        TInt iTextToShow;
        TInt iStateCodeToShow;
        TTime iWaitNoteStartTime;
        
        /**
        * Views to be registered and unregistered
        */
        CAknView* iVpnManagementUiView;
        CAknView* iVpnManagementUiLogView;
        CAknView* iVpnManagementUiPolicyView;
        CAknView* iVpnManagementUiParametersView;
        CAknView* iVpnManagementUiServerView;
        
        CVpnManagementUi* iCVpnManagementUi;
        
        /**
        * To get hold of iResourceLoader
        */
        RConeResourceLoader iResourceLoader;        

        TVwsViewId iGsViewId; // the view id of the view which launched us
        TBool iKeepOpen;
        TBool iPolicyViewVisited;
        TBool iLogViewVisited;
   };

#endif // __VPNUILOADER_H__

// End of File
