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
* Description:  CmManager related utility functionality
*
*/

#ifndef CMUTILS_H
#define CMUTILS_H

#include <e32base.h>
#include <vpnapidefs.h>


/**
 * CmManager related utilities.
 */ 
class CmUtils{
  public:
    /**
     * Gets VPN policy id based on VPN access point id.
     *
     * @param aVpnApId VPN access point id.
     * @param aPolId On return, VPN policy id.
     */
    IMPORT_C static void GetPolicyIdL(
      const TUint32 aVpnApId, TVpnPolicyId& aPolId );

};


#endif  // CMUTILS_H
