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
* Description: Soft Token plugin interface inline.
* 
*/

#ifndef __SOFTTOKENPLUGIN_INL__
#define __SOFTTOKENPLUGIN_INL__

_LIT8(KDefaultImplementation,"*");
_LIT8(KDefaultPluginImpl,"softtokenplugin"); // default_data = "softtokenplugin";

inline CSoftTokenPluginIf::~CSoftTokenPluginIf()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CSoftTokenPluginIf* CSoftTokenPluginIf::NewL()
	{
         return REINTERPRET_CAST(CSoftTokenPluginIf*, 
                REComSession::CreateImplementationL(KCSoftTokenImplUid, 
                _FOFF(CSoftTokenPluginIf,iDtor_ID_Key)));
        }
#endif // __SOFTTOKENPLUGIN_INL__
