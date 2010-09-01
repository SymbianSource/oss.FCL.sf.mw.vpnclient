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
* Description:  VPN Agile provision Web Service Client
*
*/


#ifndef C_AGILEPROVISIONWS_H
#define C_AGILEPROVISIONWS_H


#include <e32base.h>
#include <MSenServiceConsumer.h>
#include "agileprovisionservice.h"
#include "agilecrypto.h"
#include "base64.h"

const TInt KPublicKeyLen = 1024; 

NONSHARABLE_CLASS(CAgileProvisionWs) : public CBase, public MAgileProvisionServiceConsumer  
{
public:        
	IMPORT_C static CAgileProvisionWs* NewL();
	IMPORT_C static CAgileProvisionWs* NewLC();
	~CAgileProvisionWs();	

public:
    /**
     * Gets policy.
     * @param aEndpointUri Service endpoint
     * @param aStatus Completion status (returned)
     */
    IMPORT_C void GetPolicy( const TDesC8& aEndPointUri, TUint aIapId,
                             TRequestStatus& aStatus );
    
    /**
     * Cancels Get policy request.
     */
    IMPORT_C void CancelGetPolicy();    
    
     
    //gets ownership of aServiceAddr
    IMPORT_C void SetServiceAddr(const HBufC8* aServiceAddr); 
	
// from base class MAgileProvisionServiceConsumer
public: 
    void HandleMessageL(const TDesC8& aMessage);
	void HandleErrorL(const TInt aErrorCode, const TDesC8& aError);
	void SetStatus(const TInt aStatus);
	
 
	/*
     * Callback methods, for each asynchronous remote operation
     */
 
    IMPORT_C void HandleGetConfSoapInL(const TDesC8& aMessage);

    IMPORT_C void HandleGetCertificateSoapInL(const TDesC8& aMessage);
    
    IMPORT_C void GenerateCertReqL( );

    
private:
    enum 
        { 
        KStateInitial,
        KStateGettingConf,
        KStateGettingCertificate        
        };
    
    CAgileProvisionWs();

    void ConstructL();
    
    void DoGetPolicyL( const TDesC8& aEndPointUri, const TUint aIapId );
    void ParseFileFromSoapEnvelopeL( RPointerArray<CSenElement>& aArray,
                                        TInt aContentType, TInt aLevel, 
														TDesC& aFileName );
	
	    
	void DoComplete( const TInt aStatus );
    
private: // data
    
    /**
     * State.
     * Own.
     */
    TInt                            iState;

  
    CAgileProvisionServiceService*  iPService;
    
    /**
     * Client's request's status.
     * Not own.
     */
    TRequestStatus*                 iClientStatus;
    
    TPrivKeyRef iKeyRef;
    
    const HBufC8* iServiceAddr;
    
    const TDesC8* iEndPointUri;
    
    };

#endif // C_AGILEPROVISIONWS_H
