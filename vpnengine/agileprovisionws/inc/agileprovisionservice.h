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
* Description:  VPN Agile provision service 
*
*/

#ifndef AGILEPROVISIONWS_H
#define AGILEPROVISIONWS_H

#include <e32base.h>
#include <base64.h>
#include <MSenServiceConsumer.h>
#include <SenServiceConnection.h>
#include <SenSoapFault.h>

 
NONSHARABLE_CLASS(MAgileProvisionServiceConsumer) : public MSenServiceConsumer
{

public:

    // ----- To be implemented by consumer (from MSenServiceConsumer): -----
    virtual void HandleErrorL(const TInt aErrorCode, const TDesC8& aMessage) = 0;
    virtual void SetStatus(const TInt aStatus) = 0;
    
    /* Implementation of this callback is optional.
     * If you are not interested in SOAP Fault details,
     * the default implementation will fall back to HandleErrorL
     * with the error code KErrSenSoapFault.
     */
    virtual void HandleSoapFaultL(const CSenSoapFault& /*aFault*/)
        { HandleErrorL(KErrSenSoapFault, _L8("Soap fault received") ); }
    
       
};


NONSHARABLE_CLASS(CAgileProvisionServiceService) : public CBase
{
    CSenServiceConnection *iConnection;
    TBool iOwnsConnection;

    CAgileProvisionServiceService(CSenServiceConnection* aConnection, TBool aOwnsConnection);

public:

    ~CAgileProvisionServiceService();

   
    static CAgileProvisionServiceService* NewL(MSenServiceConsumer& aObserver, MSenServiceDescription& aDescription);
    


       
public: // remote methods 
   
    
    
    /**************************************************************************
     * Asynchronous operations that handle result in callback methods.        *
     * Methods can leave.                                                     *
     **************************************************************************/
    
    void GetConfAsyncL();
    void GetCertificateAsyncL(HBufC8* aCertReq);
        
    void CancelTransaction();
    void ResetTransactionId();
    
    HBufC8*             iCertreq;
    TUint               iIapId;
private: // data
    /**
     * Transaction id.
     * Own.
     */    
    TInt                iTransactionId;
   
   
};

#endif
