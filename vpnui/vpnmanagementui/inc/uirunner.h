/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* The UI runner interface that applications can use to launch UI 
* implementations.
*
*/

#ifndef __UI_RUNNER_H__
#define __UI_RUNNER_H__

#include <e32base.h>

// The second UID for DLLs that implement
// the UI runner interface.
const TInt KUiRunnerUidValue = 0x10200EC4;
const TUid KUiRunnerUid = { KUiRunnerUidValue };

/**
 * Flags that indicate how the UI launched through
 * the UI runner interface completes. These flags
 * are returned to the calling application in the
 * aUirEvent parameter of the UiComplete call. The
 * flags can be combined if needed.
 */
const TInt KUirEventNone            = 0x00000000; // UI left with the Back button
const TInt KUirEventExitRequested   = 0x00000020; // UI left with Options->Exit

/**
 * UI observation interface. An application that
 * uses the UI runner interface to launch a certain
 * UI must have an object that implements this
 * interface and pass a pointer to this object
 * in the RunUiL method (see below). The UI
 * implementation will call the UiComplete method
 * of this interface when the user leaves the UI.
 */
class MUiRunnerObserver
    {
public:
    virtual void UiComplete(TInt aUirEvent) = 0;
    };

/**
 * The UI runner interface that applications
 * can use to launch UI implementations.
 */
class CUiRunner : public CBase
    {
public:
    /**
     * Second-phase constructor
     */
    virtual void ConstructL() = 0;

    /**
     * Runs the UI, ASYNCHRONOUS.
     * 
     * @param A pointer to an object that implements the
     * MUiRunnerObserver interface. The UI implementation
     * will call the UiComplete method of this interface
     * when the user leaves the UI.
     */
    virtual void RunUiL(MUiRunnerObserver* aObserver) = 0;
    };

#endif // __UI_RUNNER_H__
