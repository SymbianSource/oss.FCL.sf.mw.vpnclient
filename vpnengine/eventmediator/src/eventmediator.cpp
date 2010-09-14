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
* Description:   This module contains eventmediator and the services of it.
*
*/



/**  
 * @file eventmediator.cpp
 *
 * This module contains eventmediator and the services of it.
 *
 */
#include "eventmediator.h"
#include "eventmediatorsession.h"
#include "eventmediatorclientservercommon.h"
#include "eventlogger.h"
#include "sit.h"
#include "log_em.h"


#define FIRST_ARGUMENT 0
#define SECOND_ARGUMENT 1
#define THIRD_ARGUMENT 2
#define FOURTH_ARGUMENT 3


// ============================= CEventMediatorServer =============================

const TUint CEventMediatorServer::iRangeCount = 2;
    
const TInt CEventMediatorServer::iRanges[iRangeCount] = 
    {
    KEventMediatorListen,
    KEventMediatorClearEventLog+1
    };

const TUint8 CEventMediatorServer::iElementIndex[iRangeCount] = 
    {
    0,
    CPolicyServer::ENotSupported
    };

const CPolicyServer::TPolicyElement CEventMediatorServer::iElements[] =
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy CEventMediatorServer::iPolicy =
    {
    0,              // All connect attempts are checked
    iRangeCount,    // Count of ranges
    iRanges,        // 0...9, 9...
    iElementIndex,  // Only range 1000-1008 are checked
    iElements       // The list of policy elements
    };


CEventMediatorServer::CEventMediatorServer(void)
    : CPolicyServer(EPriorityNormal,iPolicy), iStoredEvents(1)
    {
    }

CEventMediatorServer* CEventMediatorServer::NewL()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::NewL - begin\n")));
    
    CEventMediatorServer* self = CEventMediatorServer::NewLC();
    CleanupStack::Pop(self); // server
    LOG(Log::Printf(_L("CEventMediatorServer::NewL - end\n")));
    
    return self;
    }

CEventMediatorServer* CEventMediatorServer::NewLC()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::NewLC - begin\n")));
    
    CEventMediatorServer* self = new (ELeave) CEventMediatorServer();
    CleanupStack::PushL(self); 
    self->ConstructL();
    
    LOG(Log::Printf(_L("CEventMediatorServer::NewLC - end\n")));
    return self;
    }

void CEventMediatorServer::ConstructL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    fs.CreatePrivatePath(EDriveC);
    
    TPath privateDir;
    User::LeaveIfError(fs.PrivatePath(privateDir));
    iEventLogFileName.Copy(privateDir);
    iEventLogFileName.Append(KEventLogFile);
    
    CleanupStack::PopAndDestroy(); // fs
    
    iLogger = CEventLogger::NewL(this);
    iSit = new (ELeave) CSit(this);
    StartL(KEventMediatorServer);
    }

CEventMediatorServer::~CEventMediatorServer(void)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::~CEventMediatorServer\n")));
    // Delete stored events
    TInt nEvents = this->iStoredEvents.Count();
    for (TInt i = 0; i < nEvents; i++)  
        {
        delete iStoredEvents.At(i)->iData;
        delete iStoredEvents.At(i);
        }
    iStoredEvents.Delete(0, iStoredEvents.Count());

    // Delete log writer
    delete iLogger;

    delete iSit;
    }


TInt CEventMediatorServer::RunError(TInt aError)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::RunError - error = %d\n"), aError));
    Message().Complete(aError);

    // The leave will result in an early return from CServer::RunL(),
    // skipping the call to request another message. So we issue the
    // request here in order to keep the server running.
    ReStart();

    // Handled the error fully
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// CEventMediatorServer::NewSessionL
// Creates a new session and returns the handle to the session.
// ----------------------------------------------------------------------------
//
CSession2* CEventMediatorServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& aMessage) const
    {
    // New sessions are not accepted if the server is shutting down
    // (it's just waiting for the last session (from SIT) to die)
    if (iShuttingDown)
        {
        User::Leave(KErrServerTerminated);
        }
    
    
    CSession2* session = CEventMediatorSession::NewL(const_cast<CEventMediatorServer&>(*this), 
                                                     IsClientTheSitL(aMessage));
    iSessionCount++;
    return session;
    }

void CEventMediatorServer::SessionDeleted(TBool aIsSitSession)
    {
    TInt normalSessionCount = NormalSessionCount();

    // If this too is a normal session and is dying,
    // decrement the normal session count by one
    if (!aIsSitSession)
        {
        normalSessionCount--;        
        }

    if (normalSessionCount == 0)
        {
        // If "normal" (non-sit) sessions are no longer present,
        // we complete the task arrival observation request, thus
        // causing the SIT to terminate and close its connection
        // to this server. This should be the last connection whose
        // closing will cause this server to terminate.
        // NOTE. KErrCancel cannot be used here as the Event Mediator 
        // does not report it to the caller
        LOG(Log::Printf(_L("CEventMediatorServer::SessionDeleted - normal session count = 0\n")));
        CompleteListener(ETaskArrivedEvent, NULL, KErrAbort);

        // Set the server state to "shutting down". This will
        // cause the server to discard any new connect requests
        // with KErrServerTerminated.
        iShuttingDown = ETrue;
        }

    __ASSERT_DEBUG(iSessionCount > 0, User::Invariant());
    iSessionCount--;
    if (iSessionCount == 0) 
        {
        LOG(Log::Printf(_L("CEventMediatorServer::SessionDeleted - session count 0, stopping scheduler and thus the server\n")));
        CActiveScheduler::Stop();   
        }       
    }

void CEventMediatorServer::ReportEventL(const TEventType aType, TDesC8* aSpec,
                                        TDesC8* aData, TInt aStatus)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::ReportEventL - event type = %d\n"), aType));
    TInt listenerCount = 0;
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        // Some listeners listen this event with specification
        listenerCount += session->CheckEventL(aType, aSpec, aData, aStatus); 
        // Some without specification, all events are good for them
        if (aSpec != NULL)
            {
            listenerCount += session->CheckEventL(aType, NULL, aData, aStatus);
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }
    if (listenerCount)
        {
        // Need to save the event data as it may/will be fetched later
        TEventContainer* container = new (ELeave) TEventContainer(listenerCount, aData);
        CleanupStack::PushL(container);
        iStoredEvents.AppendL(container);
        CleanupStack::Pop();
        }
    
    // Write event to log
    __ASSERT_DEBUG(iLogger != NULL, User::Invariant());
    
    if (aType == ELogEvent)
        {
        LOG(Log::Printf(_L("CEventMediatorServer::ReportEventL - calling iLogger->LogEvent\n")));
        iLogger->LogEvent(*aData);
        }

    // If there are no listeners, delete data
    if (listenerCount == 0)
        {
        delete aData;
        }
    }

TInt CEventMediatorServer::CopyEventDataL(const RMessage2& aMessage)
    {
    TBool found = EFalse;
    TInt i = 0;
    TInt err = KErrNone;
    while (!found && i < iStoredEvents.Count())
        {
        if (iStoredEvents.At(i)->iData == aMessage.Ptr0())
            {
            found = ETrue;
            }
        else
            {
            i++;
            }
        }
    if (found)
        {
        aMessage.WriteL(SECOND_ARGUMENT, *(iStoredEvents.At(i)->iData));
        MarkStoredEventListened(i);
        }
    else
        {
        err = KErrNotFound;
        }
    return err;
    }

void CEventMediatorServer::MarkStoredEventListened(TInt aIndex)
    {
    iStoredEvents.At(aIndex)->iListenerCount--;
    if (iStoredEvents.At(aIndex)->iListenerCount == 0)
        {
        delete iStoredEvents.At(aIndex)->iData;
        delete iStoredEvents.At(aIndex);
        iStoredEvents.Delete(aIndex);
        }
    }

TBool CEventMediatorServer::IsClientTheSitL(const RMessage2& aMessage) const
    {
    LOG(Log::Printf(_L("CEventMediatorServer::IsClientTheSitL\n")));
    TBool isClientTheSit = EFalse;
    
    RThread clientThread;
    User::LeaveIfError(aMessage.Client(clientThread));
    
    if (clientThread.Id() == iSit->ThreadId())
        {
        LOG(Log::Printf(_L("CEventMediatorServer::IsClientTheSitL - YES\n")));
        isClientTheSit = ETrue;
        }

    clientThread.Close();

    return isClientTheSit;
    }
    
void CEventMediatorServer::MakeSureSitIsRunningL()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::MakeSureSitIsRunningL\n")));
    // If the SIT has not yet been started
    // or has died, try to start it
    iSit->StartL();
    }

void CEventMediatorServer::SetTaskArrivalListenerL(CListenerContainer* aListener)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::SetTaskArrivalListenerL\n")));
    if (aListener)
        {
        // Task arrival observation requests must
        // come from the SIT
        if (!IsClientTheSitL(aListener->Message()))
            {
            User::Leave(KErrNotSupported);
            }

        // Only one task arrival observation request
        // is allowed to be present at the same time
        if (iTaskArrivalListener && (aListener != iTaskArrivalListener))
            {
            User::Leave(KErrNotSupported);
            }
        }

    iTaskArrivalListener = aListener;
    }

void CEventMediatorServer::ClearTaskArrivalListener()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::ClearTaskArrivalListener\n")));
    iTaskArrivalListener = NULL;
    }

CListenerContainer* CEventMediatorServer::TaskArrivalListener()
    {
    return iTaskArrivalListener;
    }

void CEventMediatorServer::TaskRequestArrivedL(CListenerContainer* aTaskRequest)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskRequestArrivedL\n")));
    CompleteTaskArrivalObservationRequestL(aTaskRequest->Type(), aTaskRequest->Specification());
    }

void CEventMediatorServer::TaskArrivalObservationRequestArrivedL()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskArrivalObservationRequestArrivedL\n")));
    // Go through all pending event listening requests to see if any
    // one of those should be passed to the SIT (i.e. if the task
    // arrival observation request should be completed immediately)
    
    CListenerContainer* taskRequest = FindWaitingTaskRequest();
    
    if (taskRequest)
        {
        CompleteTaskArrivalObservationRequestL(taskRequest->Type(), taskRequest->Specification());
        }
    }

void CEventMediatorServer::TaskRequestEventSpecFetchingRequestArrivedL(CListenerContainer* aFetchingRequest)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskRequestEventSpecFetchingRequestArrivedL\n")));
    TFetchTaskInfoEventSpec taskRequestInfo;
    TPckg<TFetchTaskInfoEventSpec> taskRequestInfoDes(taskRequestInfo);
    taskRequestInfoDes.Copy(*(aFetchingRequest->Specification()));

    // Find the task request whose event specification
    // we should return to the SIT TH
    CListenerContainer* taskRequest = FindListener(taskRequestInfo.iEventType, taskRequestInfo.iEventSpecId);

    if (taskRequest && !taskRequest->BeingFulfilled())
        {
        CompleteTaskRequestEventSpecFetchingRequestL(KErrNone, aFetchingRequest->Specification(),
                                                     taskRequest->Specification());
        taskRequest->MarkAsBeingFulfilled();
        }
    else
        {
        CompleteTaskRequestEventSpecFetchingRequestL(KErrNotFound, aFetchingRequest->Specification(), NULL);
        }
    }

void CEventMediatorServer::TaskCancellationObservationRequestArrivedL(CListenerContainer* aRequest)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskCancellationObservationRequestArrivedL\n")));
    // Try to find a task request event type that corresponds to the received
    // event type. This will only succeed if the received event type is
    // one that is used to listen to the cancellation of a task request.
    TEventType taskRequestEventType = CSit::FindTaskRequestEventType(aRequest->Type());

    // If a corresponding task request type was found...
    if (taskRequestEventType != EUnfoundEvent)
        {
        // Try to find the listener container of the task request
        CListenerContainer* taskRequest = FindListener(taskRequestEventType, aRequest->Specification());

        // The listener container for the task request was not found
        // (i.e. the task request has been cancelled or the
        // corresponding client session has been closed), so we
        // complete the cancellation observation request right away
        if (taskRequest == NULL)
            {
            ReportEventL(aRequest->Type(), aRequest->Specification(), NULL);
            }
        }
    }

void CEventMediatorServer::CompleteTaskArrivalObservationRequestL(TEventType aEventType, TDesC8* aSpec)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::CompleteTaskArrivalObservationRequestL\n")));
    if (iTaskArrivalListener)
        {
        // In SIT events, the event specification
        // begins with the event specification ID
        TEventSpec* sitEventSpec = (TEventSpec*)(aSpec->Ptr());
        
        TTaskArrivedEventData eventData;
        eventData.iEventType = aEventType;
        eventData.iEventSpecId = sitEventSpec->iId;
        TPckg<TTaskArrivedEventData> eventDataDes(eventData);
        
        HBufC8* eventDataCopy = eventDataDes.AllocL();
        CleanupStack::PushL(eventDataCopy);

        ReportEventL(ETaskArrivedEvent, NULL, eventDataCopy);
        
        CleanupStack::Pop(); // eventDataCopy, freed elsewhere
        }
    }

void CEventMediatorServer::CompleteTaskRequestEventSpecFetchingRequestL(TInt aStatus, TDesC8* aEventSpec,
                                                                        TDesC8* aTaskRequestEventSpec)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::CompleteTaskRequestEventSpecFetchingRequestL\n")));
    // The event specification of the task request
    // is returned to the SIT TH as event data
    if (aTaskRequestEventSpec)
        {
        HBufC8* eventData = aTaskRequestEventSpec->AllocL();
        CleanupStack::PushL(eventData);

        ReportEventL(EFetchTaskInfoEvent, aEventSpec, eventData, aStatus);

        CleanupStack::Pop(); // eventData, freed elsewhere
        }
    else
        {
        ReportEventL(EFetchTaskInfoEvent, aEventSpec, NULL, aStatus);
        }
    }
    
CListenerContainer* CEventMediatorServer::FindWaitingTaskRequest()
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    CListenerContainer* listener = NULL;
    
    while (session != NULL)
        {
        listener = session->FindWaitingTaskRequest();
        if (listener != NULL)
            {
            break;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return listener;
    }

CListenerContainer* CEventMediatorServer::FindListener(TEventType aEventType,    
                                                       TInt aEventSpecId)
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    CListenerContainer* listener = NULL;
    
    while (session != NULL)
        {
        listener = session->FindListener(aEventType, aEventSpecId);
        if (listener != NULL)
            {
            break;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return listener;
    }
    
CListenerContainer* CEventMediatorServer::FindListener(TEventType aEventType,    
                                                       const TDesC8* aEventSpec)
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    CListenerContainer* listener = NULL;
    
    while (session != NULL)
        {
        listener = session->FindListener(aEventType, aEventSpec);
        if (listener != NULL)
            {
            break;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return listener;
    }

void CEventMediatorServer::CompleteListener(TEventType aEventType, const TDesC8* aEventSpec, TInt aStatus)
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        session->CompleteListener(aEventType, aEventSpec, aStatus);
        session = (CEventMediatorSession*) iSessionIter++;
        }
    }

TInt CEventMediatorServer::NormalSessionCount()
    {
    TInt normalSessionCount = 0;
    
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        if (!(session->IsASitSession()))
            {
            normalSessionCount++;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return normalSessionCount;
    }

TInt CEventMediatorServer::NewEventSpecId()
    {
    return ++iNextEventSpecId;
    }

TPtrC CEventMediatorServer::EventLogFileName(void)
    {
    TPtrC name(iEventLogFileName);
    return name;
    }
    
void CEventMediatorServer::SitDied()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::SitDied\n")));
    CompleteTaskRequests(KErrDied);
    }
    
void CEventMediatorServer::CompleteTaskRequests(TInt aStatus)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::CompleteTaskRequests\n")));    
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        session->CompleteTaskRequests(aStatus);
        session = (CEventMediatorSession*) iSessionIter++;
        }
    }
    
// ============================= CEventMediatorServer =============================    

CListenerContainer::CListenerContainer(const RMessage2& aMessage, TDesC8* aSpec,
                                       CEventMediatorServer& aServer)
    : iSpec(aSpec), iMessage(aMessage), iServer(aServer)
    {
    iEventType = Type();
    }

void CListenerContainer::AnalyzeRequestL()
    {
    LOG(Log::Printf(_L("CListenerContainer::AnalyzeRequestL\n")));
    if (CSit::EventRequiresSit(iEventType))
        {
        LOG(Log::Printf(_L("CListenerContainer::AnalyzeRequestL - event type = %d, requires SIT\n"), iEventType));
        iServer.MakeSureSitIsRunningL();
        iServer.TaskRequestArrivedL(this);
        }

    // If this event listening request is the one
    // made by the SIT task arrival observer...
    if (iEventType == ETaskArrivedEvent)
        {
        iServer.SetTaskArrivalListenerL(this);
        iServer.TaskArrivalObservationRequestArrivedL();
        }

    // If this event listening request is one
    // made by a SIT TH to fetch a task...
    if (iEventType == EFetchTaskInfoEvent)
        {
        iServer.TaskRequestEventSpecFetchingRequestArrivedL(this);
        }

    // If the event listening request is one
    // made by a SIT TH to listen to the cancellation
    // of the task request it is handling
    if (CSit::IsTaskCancellationObservationRequest(iEventType))
        {
        iServer.TaskCancellationObservationRequestArrivedL(this);
        }
    }
    
CListenerContainer::~CListenerContainer()
    {
    delete iSpec;

    if (iServer.TaskArrivalListener() == this)
        {
        iServer.ClearTaskArrivalListener();
        }
    }

TBool CListenerContainer::HandlesEvent(TEventType aEventType, const TDesC8* aEventSpec)
    {
    if (iEventType == aEventType)
        {
        // Specs are same if both are NULL
        if (iSpec == NULL && aEventSpec == NULL)
            {
            return ETrue;
            }
        // or data in buffers are identical
        else if (iSpec != NULL && aEventSpec != NULL && (*iSpec) == (*aEventSpec))
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
void CListenerContainer::Complete(TInt status)
    {
    // If there's a SIT fulfilling this event listening
    // request (i.e. if this is a task request) and this
    // task request is being cancelled...
    if (status == KErrCancel)
        {
        // See if we can find a task request cancellation observation
        // event type that corresponds to this event type
        TEventType cancelEventType = CSit::FindCancelEventType(iEventType);
        
        if (cancelEventType != EUnfoundEvent)
            {
            // Complete the task request cancellation
            // observation, if found
            iServer.CompleteListener(cancelEventType, iSpec, KErrNone);
            }
        }
    
    iMessage.Complete(status);
    }

TBool CListenerContainer::WaitingForFulfilling()
    {
    if (CSit::EventRequiresSit(iEventType) && !iBeingFulfilledBySit)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

void CListenerContainer::MarkAsBeingFulfilled()
    {
    iBeingFulfilledBySit = ETrue;
    }

TBool CListenerContainer::BeingFulfilled()
    {
    return iBeingFulfilledBySit;
    }
