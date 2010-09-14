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
* Description:   This module defines event mediator and the services of it.
*
*/



/**  
 * @file eventmediator.h
 *
 * This module defines event mediator and the services of it.
 *
 */

#ifndef __EVENTMEDIATOR_H__
#define __EVENTMEDIATOR_H__

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "eventmediatorapi.h"
#include "eventmediatordefs.h"

#include "sitdeathobserver.h"

// FORWARD DECLARATIONS
class CEventMediatorSession;
class TEventContainer;
class CEventLogger;

class CSit;

class CListenerContainer;

// CLASS DECLARATION

/**
* Defines the event mediator server. 
*/
class CEventMediatorServer : public CPolicyServer, public MSitDeathListener
    {
    public: //Methods

    // Constructors and destructor

        /**
        * Static constructor
        */
        static CEventMediatorServer* NewL(void);

        /**
        * Static constructor
        */
        static CEventMediatorServer* NewLC(void);

        /**
        * Destructor
        */      
        ~CEventMediatorServer(void);

        /**
        * Creates a new session.
        */
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

        /**
         * Notifies the server that a session has been deleted.
         */
        void SessionDeleted(TBool aIsSitSession);
    // Other methods
        
        /**
         * Reports new envent to the eventmediator server.
         * @param aType: type of the event.
         * @param aSpec: additional info on event.
         * @param aData: a descriptor containing event data.
         */
        void ReportEventL(const TEventType aType, TDesC8* aSpec,
                          TDesC8* aData, TInt aStatus = KErrNone);

        /**
         * Completes the last message in error case.
         * @param aError: Cause of the call of this function.
         */
        void CompleteLastMessage(TInt aError);

        /**
         * Finds out whether the specified client thread is a SIT
         * thread or not
         */
        TBool IsClientTheSitL(const RMessage2& aMessage) const;

        /**
         * Starts the SIT thread if it is not running
         */
        void MakeSureSitIsRunningL();

        /**
         * Saves a pointer to the listening request that represents the
         * task arrival observation request
         */
        void SetTaskArrivalListenerL(CListenerContainer* aListener);

        /**
         * Clears the pointer to the listening request that represents the
         * task arrival observation request
         */
        void ClearTaskArrivalListener();

        /**
         * Returns the pointer to the task arrival observation request
         */
        CListenerContainer* TaskArrivalListener();

        /**
         * Notifies the server about a new task request that has arrived
         */
        void TaskRequestArrivedL(CListenerContainer* aTaskRequest);

        /**
         * Notifies the server about a new task arrival
         * observation request that has arrived from the SIT
         */
        void TaskArrivalObservationRequestArrivedL();

        /**
         * Notifies the server about a new task request event
         * specification fetching request that has arrived from
         * the SIT.
         */
        void TaskRequestEventSpecFetchingRequestArrivedL(CListenerContainer* aFetchingRequest);

        /**
         * Notifies the server about a new task cancellation
         * observation request that has arrived from the SIT.
         */
        void TaskCancellationObservationRequestArrivedL(CListenerContainer* aRequest);

        /**
         * Completes a pending task arrival observation
         * request when a new task request has arrived.
         */
        void CompleteTaskArrivalObservationRequestL(TEventType aEventType, TDesC8* aSpec);

        /**
         * Completes a pending task request event
         * specification fetching request.
         */
        void CompleteTaskRequestEventSpecFetchingRequestL(TInt aStatus, TDesC8* aEventSpec,
                                                          TDesC8* aTaskRequestEventSpec);

        /**
         * Tries to find a task request that is not yet
         * being fulfilled by the SIT.
         */
        CListenerContainer* FindWaitingTaskRequest();
        
        /**
         * Retrieves the event listener object, if any, that has the
         * specified event type and event specification ID.
         */
        CListenerContainer* FindListener(TEventType aEventType, TInt aEventSpecId);
        
        /**
         * Retrieves the event listener object, if any, that is using
         * the SIT that has the specified thread ID and is fulfilling
         * the specified event.
         */
        CListenerContainer* FindListener(TEventType aEventType, const TDesC8* aEventSpec);

        /**
         * Completes the listener that matches the specified search
         * criteria.
         */
        void CompleteListener(TEventType aEventType, const TDesC8* aEventSpec, TInt aStatus);

        /**
         * Completes the listener that matches the specified search
         * criteria.
         */
        void CompleteListener(CListenerContainer* aListenerPtr, TInt aStatus);

        /**
         * Returns the number of "normal" (non-sit)
         * sessions that are present
         */
        TInt NormalSessionCount();

        /**
         * Completes all task requests with the specified status
         */
        void CompleteTaskRequests(TInt aStatus);
        
        /**
         * Returns a new event specification ID.
         */
        TInt NewEventSpecId();

        TPtrC EventLogFileName(void);
        
        /**
         * Copies data describing an event to the client.
         * @param aMessge: a message from client side sent by RConnection::FetchData
         * @return error code, KErrNone if successfull
         */
        TInt CopyEventDataL(const RMessage2& aMessage);    
        
        
   public: // From MSitDeathListener
        void SitDied();

   protected:
        /**
         * Process any errors
         * 
         * @param aError the leave code reported
         */
        TInt RunError(TInt aError);

    private:    // Methods

    // Constructors and destructor

        /**
         * Default constructor
         */
        CEventMediatorServer(void);

        /**
         * Perform the second phase construction of a CVpnManagerServer
         * object.
         */
        void ConstructL();

    // Other methods


        /**
         * Reduces listener count of a stored event by one and destroys it if count becomes zero.
         * @param aIndex: the index of the stored event in the list.
         */
        void MarkStoredEventListened(TInt aIndex);

    private: //Data
        // List of Events that have been reported but not handled by all listeners.
        CArrayFixFlat<TEventContainer*> iStoredEvents;
        // Number of currently existing sessions.
        mutable TInt iSessionCount;
        // Log writer
        CEventLogger* iLogger;
        // The next event specification ID
        // (used with SIT event listening requests)
        TInt iNextEventSpecId;
        // The single SIT instance
        CSit* iSit;
        // The single task arrival listener
        CListenerContainer* iTaskArrivalListener;

        // A flag that is set to true when
        // the server starts going down
        TBool iShuttingDown;
        
        TFileName iEventLogFileName;

        static const TUint iRangeCount;
        static const TInt iRanges[];
        static const TUint8 iElementIndex[];

        static const CPolicyServer::TPolicyElement iElements[];
        static const CPolicyServer::TPolicy iPolicy;

    };


// CLASS DECLARATION

/**
* Container for one reported event from witch it can be read by all listening sessions. 
*/
class TEventContainer
    {
    public: //Methods
    // Constructors and destructor

        /**
        * Inline constructor
        */
        inline TEventContainer(TInt aCount, TDesC8* aData) : iData(aData), iListenerCount(aCount){};

    public: //Data
        // Data descriptor reported with the event.
        TDesC8*     iData;
        // number of listeners that need this event
        TInt        iListenerCount;
    };


// CLASS DECLARATION

/**
* Container for one reported event from which it can be read by all listening sessions
*/
class CListenerContainer : public CBase
    {
    public: //Methods
    // Constructors and destructor
        CListenerContainer(const RMessage2& aMessage, TDesC8* aSpec,
                           CEventMediatorServer& aServer);
        ~CListenerContainer();

        inline TEventType Type() {return (TEventType) iMessage.Int0();}
        inline RMessage2& Message() {return iMessage;}
        inline TDesC8* Specification() {return iSpec;}
        void Complete(TInt status);

        void AnalyzeRequestL();
        TBool WaitingForFulfilling();
        void MarkAsBeingFulfilled();
        TBool BeingFulfilled();

        TBool HandlesEvent(TEventType aEventType, const TDesC8* aEventSpec);

    private: //Data
        // Data descriptor reported with the event.
        TDesC8*     iSpec;
        RMessage2   iMessage;
        TEventType  iEventType;
        // Pointer to the event mediator server
        CEventMediatorServer& iServer;
        // A flag that tells whether the request
        // is being fulfilled by the SIT or not
        TBool iBeingFulfilledBySit;
    };

#endif
