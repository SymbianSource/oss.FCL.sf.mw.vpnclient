/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Custom setting item for selectin used  connection
*
*/



#ifndef SERVER_SETTING_CONNECTION_SETTING_ITEM_H
#define SERVER_SETTING_CONNECTION_SETTING_ITEM_H

#include <e32base.h>
#include <aknsettingitemlist.h>
#include <cmapplicationsettingsui.h>

/**
 *  A custom setting for selecting the used destination or connection method.
 */
class CServerSettingConnectionSettingItem : public CAknSettingItem
    {
public:

    CServerSettingConnectionSettingItem(TInt aIdentifier, TCmSettingSelection& aSelection);
    virtual ~CServerSettingConnectionSettingItem();


    void StoreL();
    void LoadL(); 
    void EditItemL( TBool aCalledFromMenu );
    
    const TDesC& SettingTextL();

private:

    void UpdateSettingTextL();

    TCmSettingSelection& iExternalValue;
    TCmSettingSelection  iInternalValue;
    
    HBufC* iSettingText;
    };

#endif // SERVER_SETTING_CONNECTION_SETTING_ITEM_H
