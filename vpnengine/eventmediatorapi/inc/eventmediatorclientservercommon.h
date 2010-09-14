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
* Description:   Commond definitions with the even
*
*/

#ifndef EVENTMEDIATORCLIENTSERVERCOMMON_H_
#define EVENTMEDIATORCLIENTSERVERCOMMON_H_

enum
    {
    KEventMediatorListen,
    KEventMediatorListenWithSpec,
    KEventMediatorCancel,
    KEventMediatorCancelWithSpec,
    KEventMediatorCancelAll,
    KEventMediatorReportEvent,
    KEventMediatorReportEventWithSpec,
    KEventMediatorFetchData,
    KEventMediatorReportLogEvent,
    KEventMediatorNewEventSpecId,
    KEventMediatorDeletePrivateFiles,
    KEventMediatorGetEventLogSize,
    KEventMediatorGetEventLogHeader,
    KEventMediatorGetEventLogData,
    KEventMediatorClearEventLog
    };


#endif /* EVENTMEDIATORCLIENTSERVERCOMMON_H_ */
