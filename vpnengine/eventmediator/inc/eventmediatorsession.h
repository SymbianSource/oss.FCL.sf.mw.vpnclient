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
#ifndef EVENTMEDIATORSESSION_H_
#define EVENTMEDIATORSESSION_H_

#include <e32base.h>
#include <f32file.h>
#include "eventmediatorapi.h"

/**
* Defines the session to the VPN manager server. 
*/
class CEventMediatorServer;
class CListenerContainer;

class CEventMediatorSession : public CSession2
    {    
    public: // Methods

    // Constructors and destructor

        /**
        * Static constructor
        */
        static CEventMediatorSession* NewL(CEventMediatorServer& aServer, 
                                           TBool aIsSitSession);
        
        /**
        * Default destructor
        */
        ~CEventMediatorSession(void);

        /**
        * Wrapper function which Dispatches and executes the client's service calls
        * (See Message type definition).
        */
        void ServiceL(const RMessage2& aMessage);

        /**
         * Checks if client is listening events of the given type and completes message 
         * sent by clients ListenToEvent function. The length of the event data and
         * the pointer to that data are written to the message.
         * @param aType: type of the event.
         * @param aSpec: additional info on event.
         * @param aData: event data.
         * @return number of listeners for the event was listened.
         */
        TInt CheckEventL(const TEventType aType, const TDesC8* aSpec, const TDesC8* aData, TInt aStatus);

        /**
         * Tries to find a task request that is not being fulfilled
         * already
         */
        CListenerContainer* FindWaitingTaskRequest();

        /**
         * Retrieves the event listener object, if any, that is using
         * the SIT that is fulfilling the specified event.
         */
        CListenerContainer* FindListener(TEventType aEventType, TInt aEventSpecId);
        
        /**
         * Retrieves the event listener object, if any, that is using
         * the SIT that is fulfilling the specified event.
         */
        CListenerContainer* FindListener(TEventType aEventType, const TDesC8* aEventSpec);

        /**
         * Completes the specified listener if it is owned by the
         * session. Returns ETrue is the listener was found and
         * completed, EFalse otherwise.
         */
        void CompleteListener(TEventType aEventType, const TDesC8* aEventSpec, TInt aStatus);

        void CompleteTaskRequests(TInt aStatus);

        TBool IsASitSession();
        
    private: //Methods
    // Constructors and destructor

        /**
        * Constructor
        */
        CEventMediatorSession(CEventMediatorServer& aServer, TBool aIsSitSession);
        void ConstructL();

        /**
         * Starts listening events of requesteed type by coping the message to iListenedEvents.
         * @param aMessage: aMessage sent by clients ListenToEvent function.
         * @return: error code, KErrNone if successfull.
         */
        TInt ListenToEventWithSpecL(const RMessage2& aMessage);

        /**
         * Starts listening events of requesteed type by coping the message to iListenedEvents.
         * @param aMessage: aMessage sent by clients ListenToEvent function.
         * @return: error code, KErrNone if successfull.
         */
        TInt ListenToEventL(const RMessage2& aMessage);

        /**
         * Reports the event contained in the message to the event mediator server.
         * @param aMessage: aMessage sent by clients ReportEvent function.
         * @return: error code, KErrNone if successfull.
         */
        void ReportEventL(const RMessage2& aMessage);

        /**
         * Reports a log event contained in the message to the event mediator server.
         * @param aMessage: aMessage sent by clients ReportLogEvent function.
         * @return: error code, KErrNone if successfull.
         */
        void ReportLogEventL(const RMessage2& aMessage);

        /**
         * Reports the event contained in the message to the event mediator server.
         * @param aMessage: aMessage sent by clients ReportEvent function.
         * @return: error code, KErrNone if successfull.
         */
        void ReportEventWithSpecL(const RMessage2& aMessage);

        /**
         * Cancels listning of one event type.
         * @param aMessage: aMessage sent by clients CancelListenToEvent function.
         */
        void CancelListening(const RMessage2& aMessage);

        /**
         * Cancels listning of one event type.
         * @param aMessage: aMessage sent by clients CancelListenToEvent function.
         */
        void CancelListeningWithSpecL(const RMessage2& aMessage);

        /**
         * Cancels all listening.
         * @param aMessage: aMessage sent by clients CancelAllListening function.
         */
        void CancelAll();

        /**
         * Copies event data from the server to the client.
         * @param aMessage: aMessage sent by clients FetchData function.
         * @return: error code, KErrNone if successfull.
         */
        TInt FetchDataL(const RMessage2& aMessage);

        /**
         * Looks up a message from iListenedEvents.
         * @param aType: type of the event the message is listening to.
         * @param aIndex: the position of the message in iListenedEvents.
         * @return ETrue if message exists, EFalse if not.
         */
        TBool FindListenerMsg(const TEventType aType,TInt& index);

        /**
         * Looks up a message from iListenedEvents.
         * @param aType: type of the event the message is listening to.
         * @param aIndex: the position of the message in iListenedEvents.
         * @return ETrue if message exists, EFalse if not.
         */
        TBool FindListenerMsg(const TEventType aType,const TDesC8* aSpec,TInt& index);

        /**
         * Searches for a task request type event listening request
         */
        TBool FindTaskRequestListenerMsg(TInt& index);
        
        /**
         * Reads data that was reported with the event from client. Allocates a buffer
         * for data and gives ownership of that buffer to caller.
         * @param aMessage: aMessage sent by clients ReportEvent function.
         * @return pointer to the newly created buffer containig data.
         */
        HBufC8* ReadEventDataFromClientL(const RMessage2& aMessage);

        /**
         * Reads specification describing the event from client. Allocates a buffer
         * for secification and gives ownership of that buffer to caller.
         * @param aMessage: aMessage sent with event specification.
         * @return pointer to the newly created buffer containig data.
         */
        HBufC8* ReadSpecificationFromClientL(const RMessage2& aMessage);

        /**
         * Completes the specified listener and deletes it from the
         * listener list
         */
        void CompleteListener(TInt aIndex, TInt aStatus);

        /**
         * Returns to the client a new event specification ID
         */
        void NewEventSpecIdL(const RMessage2& aMessage);

        TInt DeletePrivateFiles();
        void DeletePrivateFilesL();
        
        TInt GetEventLogSize(const RMessage2& aMessage);
        TInt GetEventLogHeader(const RMessage2& aMessage);
        TInt GetEventLogData(const RMessage2& aMessage);
        TInt ClearEventLog();
        
    private: //Data
        // List of messages listening to events
        CArrayFixFlat<CListenerContainer*> iListenedEvents;
        // Event mediator server
        CEventMediatorServer& iServer;

        RFs     iFs;
        RFile   iEventLogFile;
        TBool   iEventLogFileOpen;
        // Is this session from a SIT or not

    public:        
        TBool iIsSitSession;
    };

#endif /* EVENTMEDIATORSESSION_H_ */
