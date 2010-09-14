/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module defines event mediator server side session.
*
*/

#include "eventmediatorsession.h"
#include "eventmediator.h"
#include "eventmediatorclientservercommon.h"
#include "eventlogger.h"
#include "sit.h"
#include "log_em.h"

#define FIRST_ARGUMENT 0
#define SECOND_ARGUMENT 1
#define THIRD_ARGUMENT 2
#define FOURTH_ARGUMENT 3


// ============================= CEventMediatorSession =============================     

CEventMediatorSession* CEventMediatorSession::NewL(CEventMediatorServer& aServer, 
                                                   TBool aIsSitSession)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::NewL - begin\n")));
    
    CEventMediatorSession* self = new (ELeave) CEventMediatorSession(aServer, aIsSitSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self

    LOG(Log::Printf(_L("CEventMediatorSession::NewL - end\n")));
    return self;
    }

CEventMediatorSession::CEventMediatorSession(CEventMediatorServer& aServer, TBool aIsSitSession)
    : iListenedEvents(2), iServer(aServer), iIsSitSession(aIsSitSession)
    {
    }

void CEventMediatorSession::ConstructL()
    {
    if ( !IsASitSession() )
        {
        User::LeaveIfError(iFs.Connect()); // For EventLog
        iEventLogFileOpen = EFalse;
        }    
    }


CEventMediatorSession::~CEventMediatorSession(void)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::~CEventMediatorSession\n")));
    CancelAll();

    // If this too is a normal session and is dying,
    // decrement the normal session count by one
    if (!IsASitSession())
        {
        iEventLogFile.Close();
        iEventLogFileOpen = EFalse;
        iFs.Close();         
        }
    iServer.SessionDeleted(IsASitSession());    
    }

void CEventMediatorSession::ServiceL(const RMessage2& aMessage)
    {
    TInt status = 0;

    switch (aMessage.Function())
        {
        case KEventMediatorListen:
            ListenToEventL(aMessage);
            break;

        case KEventMediatorListenWithSpec:
            ListenToEventWithSpecL(aMessage);
            break;

        case KEventMediatorCancel:
            CancelListening(aMessage);        
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorCancelWithSpec:
            CancelListeningWithSpecL(aMessage);        
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorCancelAll:
            CancelAll();        
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorReportEvent:
            ReportEventL(aMessage);
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorReportEventWithSpec:
            ReportEventWithSpecL(aMessage);
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorFetchData:
            status = FetchDataL(aMessage);
            aMessage.Complete(status);
            break;

        case KEventMediatorReportLogEvent:
            ReportLogEventL(aMessage);
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorNewEventSpecId:
            NewEventSpecIdL(aMessage);
            aMessage.Complete(KErrNone);
            break;
        case KEventMediatorDeletePrivateFiles:
            status = DeletePrivateFiles();
            aMessage.Complete(status);
            break;

        case KEventMediatorGetEventLogSize:
            status = GetEventLogSize(aMessage);
            aMessage.Complete(status);
            break;

        case KEventMediatorGetEventLogHeader:
            status = GetEventLogHeader(aMessage);
            aMessage.Complete(status);
            break;
        case KEventMediatorGetEventLogData:
            status = GetEventLogData(aMessage);
            aMessage.Complete(status);
            break;
        case KEventMediatorClearEventLog:
            status = ClearEventLog();
            aMessage.Complete(status);
            break;
        default:
            aMessage.Complete(KErrGeneral);
            break;
        }
    }

TInt CEventMediatorSession::ListenToEventL(const RMessage2& aMessage)
    {
    CListenerContainer* listener = new (ELeave) CListenerContainer(aMessage, NULL, iServer);
    CleanupStack::PushL(listener);

    iListenedEvents.AppendL(listener);
    CleanupStack::Pop(); // listener

    listener->AnalyzeRequestL();

    return KErrNone;
    }

TInt CEventMediatorSession::ListenToEventWithSpecL(const RMessage2& aMessage)
    {
    HBufC8* specBuf = NULL;

    // Read specification
    specBuf = ReadSpecificationFromClientL(aMessage);
    CleanupStack::PushL(specBuf);

    // Ownership of specBuf is given to listener
    CListenerContainer* listener = new (ELeave) CListenerContainer(aMessage, specBuf, iServer);
    CleanupStack::Pop(); // specBuf
    CleanupStack::PushL(listener);
        
    iListenedEvents.AppendL(listener);
    CleanupStack::Pop(); // listener

    listener->AnalyzeRequestL();

    return KErrNone;
    }

void CEventMediatorSession::CancelListening(const RMessage2& aMessage)
    {
    TInt index;
    
    while (FindListenerMsg((TEventType)aMessage.Int0(), index))
        {
        CompleteListener(index, KErrCancel);
        }
    }

void CEventMediatorSession::CancelListeningWithSpecL(const RMessage2& aMessage)
    {
    HBufC8* specBuf = NULL;
    TInt index;
    
    // Read specification
    specBuf = ReadSpecificationFromClientL(aMessage);
    CleanupStack::PushL(specBuf);

    // Cancel listeners
    while (FindListenerMsg((TEventType)aMessage.Int0(), specBuf, index))
        {
        CompleteListener(index, KErrCancel);
        }
    CleanupStack::PopAndDestroy(); // specBuf
    }

void CEventMediatorSession::CancelAll()
    {
    TInt nEvents = iListenedEvents.Count();
    for (TInt i = 0; i < nEvents; i++)  
        {
        iListenedEvents.At(i)->Complete(KErrCancel);
        delete iListenedEvents.At(i);
        iListenedEvents.At(i) = NULL;
        }
    iListenedEvents.Reset();
    }

void CEventMediatorSession::ReportEventL(const RMessage2& aMessage)
    {
    TEventType eventType = (TEventType)aMessage.Int0();
    LOG(Log::Printf(_L("CEventMediatorSession::ReportEventL - event type = %d\n"), eventType));
    // Read data
    HBufC8* dataBuf = ReadEventDataFromClientL(aMessage);
    CleanupStack::PushL(dataBuf);
    // Report event to server
    iServer.ReportEventL(eventType, NULL, dataBuf);
    CleanupStack::Pop();
    }

void CEventMediatorSession::ReportEventWithSpecL(const RMessage2& aMessage)
    {
    TEventType eventType=(TEventType) aMessage.Int0();
    LOG(Log::Printf(_L("CEventMediatorSession::ReportEventWithSpecL - event type = %d\n"), eventType));
    // Read data
    HBufC8* dataBuf = ReadEventDataFromClientL(aMessage);
    CleanupStack::PushL(dataBuf);
    // Read specification
    HBufC8* specBuf = ReadSpecificationFromClientL(aMessage);
    CleanupStack::PushL(specBuf);
    // Report event to server
    iServer.ReportEventL(eventType, specBuf, dataBuf);
    CleanupStack::PopAndDestroy(); // specBuf
    CleanupStack::Pop(); // dataBuf 
    }

void CEventMediatorSession::ReportLogEventL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::ReportLogEventL\n")));
    // Read event
    TLogEvent event;
    TPckg<TLogEvent> eventPckg(event);
    aMessage.ReadL(FIRST_ARGUMENT, eventPckg);

    // Create one buffer to contain put everything in a normal buffer and
    TInt lengthsDesLth = event.iDesCount * sizeof(TInt);
    TInt position = eventPckg.Length();
    TInt dataLength = position + lengthsDesLth + aMessage.Int2();
    HBufC8* dataBuf = HBufC8::NewLC(dataLength);
    TPtr8 dataPtr = dataBuf->Des();

    // Copy event to buffer
    dataPtr.Append(eventPckg);
    // Read lengths to buffer
    TPtr8 tmpPtr(&dataPtr[position], 0, dataLength - position);
    aMessage.ReadL(SECOND_ARGUMENT, tmpPtr);
    // Read descriptors to the buffer
    position= dataPtr.Length();
    tmpPtr.Set(&dataPtr[position], 0, dataLength - position);
    aMessage.ReadL(THIRD_ARGUMENT, tmpPtr);

    // Report event to server
    iServer.ReportEventL(ELogEvent, NULL, dataBuf);
    CleanupStack::Pop();
    }

TInt CEventMediatorSession::FetchDataL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::FetchDataL\n")));
    return iServer.CopyEventDataL(aMessage);
    }

TInt CEventMediatorSession::CheckEventL(const TEventType aType, const TDesC8* aSpec,
                                        const TDesC8* aData, TInt aStatus)
    {
    TInt index;
    TInt listenerCount = 0;
    TInt dataLth = 0;
    // Some events don't not have data
    if (aData)
        {
        dataLth = aData->Length();
        }
    TPckg<TInt> lengthpckg(dataLth);
    TPckgC<const TAny*> ptrpckg(aData);

    while (FindListenerMsg(aType, aSpec, index))
        {
        RMessage2& listener = iListenedEvents.At(index)->Message();

        if (aStatus == KErrNone)
            {
            // Write info about data
            listener.WriteL(SECOND_ARGUMENT, lengthpckg);
            listener.WriteL(THIRD_ARGUMENT, ptrpckg);

            // Complete listener
            listener.Complete(KErrNone);
            }
        else
            {
            listener.Complete(aStatus);
            }
        
        delete iListenedEvents.At(index);
        iListenedEvents.Delete(index);
        listenerCount++;    
        }

    return listenerCount;
    }

TBool CEventMediatorSession::FindListenerMsg(const TEventType aType, TInt& index)
    {
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->Type() == aType)
            {
            index=i;
            return ETrue;
            }
        }
    return EFalse;
    }

TBool CEventMediatorSession::FindListenerMsg(const TEventType aType, const TDesC8* aSpec, TInt& index)
    {
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->HandlesEvent(aType, aSpec))
            {
            index = i;
            return ETrue;
            }
        }
    return EFalse;
    }

TBool CEventMediatorSession::FindTaskRequestListenerMsg(TInt& index)
    {
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (CSit::EventRequiresSit(iListenedEvents.At(i)->Type()))
            {
            index = i;
            return ETrue;
            }
        }
    return EFalse;
    }
    
HBufC8* CEventMediatorSession::ReadSpecificationFromClientL(const RMessage2& aMessage)
    {
    HBufC8* specBuf;
    // Read specification descriptor length from client, create specification buffer
    const TAny* desPtr = aMessage.Ptr3();
    if (desPtr == NULL)
        {
        return NULL;
        }
    TInt specLength = aMessage.GetDesLength(FOURTH_ARGUMENT);
    // Create spcification buffer
    specBuf = HBufC8::NewLC(specLength);
    TPtr8 ptr = specBuf->Des();
    // Read specification
    aMessage.ReadL(FOURTH_ARGUMENT, ptr);
    CleanupStack::Pop(); // specBuf
    return specBuf;
    }

HBufC8* CEventMediatorSession::ReadEventDataFromClientL(const RMessage2& aMessage)
    {
    HBufC8* dataBuf = NULL;
    TInt desLength = aMessage.Int1();
    
    if (desLength != 0) // Some events have no data 
        {
        dataBuf = HBufC8::NewLC(desLength);
        TPtr8 ptr = dataBuf->Des();
        aMessage.ReadL(THIRD_ARGUMENT, ptr);
        CleanupStack::Pop(); // dataBuf
        }
    return dataBuf;
    }

CListenerContainer* CEventMediatorSession::FindWaitingTaskRequest()
    {
    CListenerContainer* listener = NULL;
    
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->WaitingForFulfilling())
            {
            listener = iListenedEvents.At(i);
            break;
            }
        }

    return listener;
    }

CListenerContainer* CEventMediatorSession::FindListener(TEventType aEventType,
                                                        TInt aEventSpecId)
    {
    CListenerContainer* listener = NULL;
    
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->Type() == aEventType)
            {
            TEventSpec* eventSpec = (TEventSpec*)(iListenedEvents.At(i)->Specification()->Ptr());

            if (eventSpec->iId == aEventSpecId)
                {
                listener = iListenedEvents.At(i);
                break;
                }
            }
        }

    return listener;
    }
    
CListenerContainer* CEventMediatorSession::FindListener(TEventType aEventType,
                                                        const TDesC8* aEventSpec)
    {
    CListenerContainer* listener = NULL;
    
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->HandlesEvent(aEventType, aEventSpec))
            {
            listener = iListenedEvents.At(i);
            break;
            }
        }

    return listener;
    }

void CEventMediatorSession::CompleteListener(TEventType aEventType,
                                             const TDesC8* aEventSpec,
                                             TInt aStatus)
    {
    TInt index;
    while (FindListenerMsg(aEventType, aEventSpec, index))
        {
        CompleteListener(index, aStatus);
        }
    }
        
void CEventMediatorSession::CompleteListener(TInt aIndex, TInt aStatus)
    {
    iListenedEvents.At(aIndex)->Complete(aStatus);
    delete iListenedEvents.At(aIndex);
    iListenedEvents.Delete(aIndex);
    }

void CEventMediatorSession::CompleteTaskRequests(TInt aStatus)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::CompleteTaskRequests\n")));
    TInt index;
    while (FindTaskRequestListenerMsg(index))
        {
        CompleteListener(index, aStatus);
        }
    }

TBool CEventMediatorSession::IsASitSession()
    {
    return iIsSitSession;
    }

void CEventMediatorSession::NewEventSpecIdL(const RMessage2& aMessage)
    {
    TInt newEventSpecId = iServer.NewEventSpecId();
    TPckg<TInt> newEventSpecIdDes(newEventSpecId);
    aMessage.WriteL(FIRST_ARGUMENT, newEventSpecIdDes);
    }

TInt CEventMediatorSession::DeletePrivateFiles()
    {
    TRAPD(err, DeletePrivateFilesL());
    if ( err )
        {
        LOG(Log::Printf(_L("DeletePrivateFilesL() leave error %d\n"), err));
        return err;
        }

    return KErrNone;
    }

void CEventMediatorSession::DeletePrivateFilesL()
    {
    LOG(Log::Printf(_L("DeletePrivateFilesL() called\n")));

    CFileMan* fileMan = CFileMan::NewL(iFs);
    CleanupStack::PushL(fileMan);

    TPath privateDir;
    User::LeaveIfError(iFs.PrivatePath(privateDir));

    TInt err = fileMan->RmDir(privateDir);
    if (err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(); //fileMan
    }

TInt CEventMediatorSession::GetEventLogSize(
    const RMessage2& aMessage)
    {
    if ( iEventLogFileOpen )
        {
        iEventLogFile.Close();
        iEventLogFileOpen = EFalse;
        }

    TInt err = iEventLogFile.Open(iFs, iServer.EventLogFileName(), EFileRead | EFileShareAny);
    if ( err )
        return err;

    TInt size(0);
    err = iEventLogFile.Size(size);
    if ( err )
        {
        iEventLogFile.Close();
        return err;
        }
    
    TPckg<TInt> sizePckg(size);
    err = aMessage.Write(FIRST_ARGUMENT, sizePckg);
    if ( err )
        {
        iEventLogFile.Close();
        return err;
        }

    iEventLogFileOpen = ETrue;
    
    return KErrNone;
    }

TInt CEventMediatorSession::GetEventLogHeader(
    const RMessage2& aMessage)
    {
    TInt err(0);
    
    if ( !iEventLogFileOpen )
        {
        err = iEventLogFile.Open(iFs, iServer.EventLogFileName(), EFileRead | EFileShareAny);
        if ( err )
            return err;
        iEventLogFileOpen = ETrue;
        }
    
    TInt position = 0;
    err = iEventLogFile.Seek(ESeekStart, position);     
    if (err != KErrNone)
        return err;

    TBuf8<EVENTLOG_FILE_HEADER_LTH> fileHeaderBuf;
    err = iEventLogFile.Read(fileHeaderBuf, EVENTLOG_FILE_HEADER_LTH);
    if (err != KErrNone)
        return err;

    TRAP(err, aMessage.WriteL(FIRST_ARGUMENT, fileHeaderBuf));
    if ( err )
        return err;

    return KErrNone;
    }
    
TInt CEventMediatorSession::GetEventLogData(
    const RMessage2& aMessage)
    {
    TInt err(0);

    if ( !iEventLogFileOpen )
        {
        err = iEventLogFile.Open(iFs, iServer.EventLogFileName(), EFileRead | EFileShareAny);
        if ( err )
            return err;
        iEventLogFileOpen = ETrue;
        }
    
    TInt size(0);
    err = iEventLogFile.Size(size);
    if ( err )
        return err;

    if ( size < EVENTLOG_FILE_HEADER_LTH )
        return KErrNotFound;

    HBufC8* eventLogFileBuf = NULL;
    TRAP(err, eventLogFileBuf = HBufC8::NewL(size));
    if ( err )
        {
        return err;
        }
    
    TPtr8 eventLogDataPtr(eventLogFileBuf->Des());
    TInt position(0);
    err = iEventLogFile.Seek(ESeekStart, position);
    if ( err )
        {
        delete eventLogFileBuf;
        return err;
        }
    err = iEventLogFile.Read(eventLogDataPtr); // iLogFileSize);
    if ( err )
        {
        delete eventLogFileBuf;
        return err;
        }
    
    TRAP( err, aMessage.WriteL(FIRST_ARGUMENT, eventLogDataPtr));
    if ( err )
        {
        delete eventLogFileBuf;
        return err;
        }

    delete eventLogFileBuf;
    eventLogFileBuf = NULL;
    
    return KErrNone;
    }
    
TInt CEventMediatorSession::ClearEventLog()
    {
    if ( iEventLogFileOpen )
        {
        iEventLogFile.Close();
        iEventLogFileOpen = EFalse;
        }

    TInt err = iFs.Delete(iServer.EventLogFileName());

    return err;
    }

