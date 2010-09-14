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
* Description:   Combined task arrival observer and task handler manager
*
*/



// @file taskarrivalobserver.h

#ifndef __TASK_ARRIVAL_OBSERVER_H__
#define __TASK_ARRIVAL_OBSERVER_H__

#include "sit.h"
#include "taskhandler.h"

const TInt KUnfoundIndex = -1;

class CAsyncCleaner;

/**
 * Combined task handler arrival observer and task handler manager.
 */
NONSHARABLE_CLASS(CTaskArrivalObserver) : public CBase,
                                          public MEventObserver, 
                                          public MTaskHandlerManager
    {
public:
    static CTaskArrivalObserver* NewL();
    ~CTaskArrivalObserver();
    
public:
    void Start();
    void Cancel();
    void DeleteTaskHandler(CTaskHandler* aTaskHandler);
    void DeleteAsyncCleaner(CAsyncCleaner* aAsyncCleaner);

private: // From MEventObserver
    void EventOccured(TInt aStatus, TEventType aType, TDesC8* aData);

private: // From MTaskHandlerManager
    void TaskHandlerComplete(CTaskHandler* handler);
    void TaskHandlerFatalError(CTaskHandler* handler, TInt aError);
    
private: // Construction
    CTaskArrivalObserver();
    void ConstructL();

private: // Implementation
    void LaunchTaskHandlerL(const TTaskArrivedEventData& aEventSpec);    

private:
    REventMediator iEventMediator;
    CArrayPtrFlat<CTaskHandler>* iTaskHandlerList;
    CArrayPtrFlat<CAsyncCleaner>* iAsyncCleanerList;
    };

NONSHARABLE_CLASS(CAsyncCleaner) : public CAsyncOneShot
    {
public:
    CAsyncCleaner(CTaskArrivalObserver* aTaskArrivalObserver,
                  CTaskHandler* aTaskHandlerToDelete);
    void Start();        
    TBool IsMatchingCleaner(const CTaskHandler& aTaskHandler) const;
private: // From CAsyncOneShot
    void RunL();

private:
    CTaskArrivalObserver* iTaskArrivalObserver;
    CTaskHandler* iTaskHandlerToDelete;
    };

#endif // __TASK_ARRIVAL_OBSERVER_H__
