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
* Description:   Declaration of the CServerSettingsContainer class.
*
*/

#ifndef  __VPNMANAGEMENTUISERVERSETTINGSCONTAINER_H__
#define  __VPNMANAGEMENTUISERVERSETTINGSCONTAINER_H__

#include <aknsettingitemlist.h>


/**
 *  CServerSettingsContainer container control class.
 *
 *  @lib vpnmanagementui.lib
 *  @since S60 v3.0
*/
class CServerSettingsContainer : public CAknSettingItemList
    {
public: // functions
    
    /**
     * NewL
     *
     * @since S60 3.2
     * @param aLoader VPN UI loader for common view handling.
     * @param aServerIndex index of selected server or KErrNotFound.     
     * @return self
     */        
	static CServerSettingsContainer* NewL(
	    CVpnUiLoader& aLoader, TInt aServerIndex);
    /**
    * Destructor.
    */
    ~CServerSettingsContainer();
        
    /**
     * From CAknSettingItemList
     */   
    CAknSettingItem* CreateSettingItemL( TInt aSettingId );
    
    /**
     * ServerNameExistsL
     * @return ETrue name already in use
     */
    TBool ServerNameExistsL( const TDesC& aText ) const; 
    /**
     * ChangeSettingValueL
     * Opens Setting page for currently selected setting item
     */
    void ChangeSettingValueL();
    
    /*** NSSM support is discontinued.
         Code is kept in comments temporarily because similar UI functionality
         might be needed for another purpose.
    const TAcuApiServerDetails& ServerDetailsL(); ***/

    static HBufC* GetDestinationNameL( TUint aId );
    static HBufC* GetConnectionMethodNameL( TUint aId );

    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    TInt ServerIndex() const;

private: // functions

    /**
    * Default constructor
    */
	CServerSettingsContainer();
	
    /**
    * Overrided Default constructor
    */
	CServerSettingsContainer(
	    CVpnUiLoader& aLoader, TInt aServerIndex );		

    /**
    * Symbian OS default constructor.
    * @param aRect Frame rectangle for container.
    */
	void ConstructL();

private: // implementation

    void UpdateTitleL( TDes& aText );

#ifdef __SERIES60_HELP
    /**
	* This function is called when Help application is launched.  
	* (other items were commented in a header).
    */
	void GetHelpContext(TCoeHelpContext& aContext) const;
#endif //__SERIES60_HELP

private: // data
    
    /**
    * To get hold of VpnUiLoader
    */
	CVpnUiLoader& iLoader;
	
	/// Server details
    // TAcuApiServerDetails iServerDetails;
    TBuf<KMaxServerUrlLength> iServerAddressBuffer;
    
   /**
    * To get the index of server list
    * -1 when user has select Add new server
	* Must be public so that view can access
    */
	TInt iServerIndex;
    };

#endif // __VPNMANAGEMENTUISERVERSETTINGSCONTAINER_H__

// End of File
