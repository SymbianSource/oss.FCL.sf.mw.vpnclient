/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Soft Token plugin interface.
* 
*/

#ifndef __SOFTTOKENPLUGIN_H
#define __SOFTTOKENPLUGIN_H

#include <e32base.h>
#include <ecom/ecom.h>
 
const TUid KCSoftTokenImplUid = {0x20031614};
/**
 *  Soft Token plugin interface.
 *
 *  Soft Token plugin interface provides functionality for generating OTPs
 *  (One Time Passwords).
 *
 */

class CSoftTokenPluginIf : public CBase
    {
    public:
        /**
         * Instantiates an object of this type .
         */
        static CSoftTokenPluginIf* NewL();
        /**
         * Destructor.
         */
        virtual ~CSoftTokenPluginIf();
        /**
         * Checks if default token is found.
         * @return True if there is a token available.
         */
        virtual TBool DefaultFoundL() = 0;
        /**
         * Gets code from default token.
         * @param aPin PIN of token.
         * @param aOTP One Time Password.
         * @param aNextCode True if next code is requested.
         * @return KErrNone if the OTP received or a system-wide error code.
         */
        virtual TInt CodeL(const TDesC8& aPin, HBufC8*& aOTP, TBool aNextCode = EFalse) = 0;
        
    private:
        TUid iDtor_ID_Key;
    };

#include "softtokenpluginif.inl"

#endif // __SOFTTOKENPLUGIN_H
