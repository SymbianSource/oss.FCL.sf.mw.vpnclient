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

#include <SenXmlServiceDescription.h>
#include <SenSoapMessage.h>
#include <SenHttpTransportProperties.h>
#include <e32debug.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include "agileprovisionservice.h"
#include "agileprovisiondefs.h"


CAgileProvisionServiceService* CAgileProvisionServiceService::NewL(MSenServiceConsumer& aObserver, MSenServiceDescription& aServiceDescription)
{
    CSenServiceConnection* pConn = CSenServiceConnection::NewLC(aObserver, aServiceDescription);
    
    CAgileProvisionServiceService* pNew = new (ELeave) CAgileProvisionServiceService(pConn, ETrue); // owns connection
    
    CleanupStack::Pop(pConn);
    return pNew;
}


CAgileProvisionServiceService::CAgileProvisionServiceService(CSenServiceConnection* aConnection, TBool aOwnsConnection) :
    iConnection(aConnection), iOwnsConnection(aOwnsConnection)
{
}

CAgileProvisionServiceService::~CAgileProvisionServiceService()
{
    delete iCertreq;
    iCertreq=NULL;
    if (iOwnsConnection)
    {
        delete iConnection;
    }
   
}


void CAgileProvisionServiceService::GetConfAsyncL()
{
    TPtrC8 reqStr;
 
    CSenSoapMessage *soapRequest = CSenSoapMessage::NewL();
    CleanupStack::PushL(soapRequest);
    
    TPtrC8 soapActionPtr(KSoapAction);
    soapRequest->SetSoapActionL( soapActionPtr );
     
    
    TPtrC8 reqBodyStr(KRequest);
    soapRequest->SetBodyL(reqBodyStr);
   
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    
    using namespace CMManager;
       
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();        
    CleanupClosePushL( cmManagerExt );     

    RArray<TUint32> destinationArray;    
    cmManagerExt.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);    
    TBool destinationExist=EFalse;       
    for (TInt i = 0; i < destinationArray.Count(); ++i)
        {
        RCmDestinationExt destination = cmManagerExt.DestinationL( destinationArray[i] );
        CleanupClosePushL(destination);
       
        if ( destination.Id() == iIapId )
            {
             destinationExist=ETrue;
             CleanupStack::PopAndDestroy(); //destination
             break;
            }                
        CleanupStack::PopAndDestroy(); //destination
        }
    destinationArray.Reset();
    destinationArray.Close();
    CleanupStack::PopAndDestroy(2); //destinationArray, cmManagerExt
    
    if ( destinationExist == 1 ) //ETrue
        {
        pHttpProperties->SetSnapIdL(iIapId);
        }
    else
        {
        pHttpProperties->SetIapIdL(iIapId);
        }
    
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iConnection->SetTransportPropertiesL(*pSerializedProperties);
       
    TInt ret = iConnection->SendL(*soapRequest);
    User::LeaveIfError( ret );
   
    iTransactionId = ret ;
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    CleanupStack::PopAndDestroy(soapRequest);
}


void CAgileProvisionServiceService::GetCertificateAsyncL( HBufC8* aCertReq )
{
    TPtrC8 reqStr;
    
    CSenSoapMessage *soapRequest = CSenSoapMessage::NewL();
    CleanupStack::PushL(soapRequest);
    soapRequest->SetSoapActionL( KSoapActionCert );
    
    TBase64Codec base64Codec;    
    HBufC8* certreqBase64 = base64Codec.Base64EncodeLC(*aCertReq);
    HBufC8* certReqBodyHeap=HBufC8::NewLC(KRequest1().Length() + KRequest2().Length() + certreqBase64->Length());
    TPtr8 certReqBodyAppend(certReqBodyHeap->Des());
    certReqBodyAppend=KRequest1;
    certReqBodyAppend.Append(certreqBase64->Des());
    certReqBodyAppend.Append(KRequest2);
    
    TPtrC8 reqBodyStr(certReqBodyAppend);
    soapRequest->SetBodyL(reqBodyStr);

    TInt ret = iConnection->SendL(*soapRequest);
    User::LeaveIfError( ret );
    iTransactionId = ret;
    CleanupStack::PopAndDestroy(); //certReqBodyHeap
    CleanupStack::PopAndDestroy(); //certreqBase64
    CleanupStack::PopAndDestroy(); // soapRequest
}


void CAgileProvisionServiceService::CancelTransaction()
{
    if ( iTransactionId > 0 )
    {
        iConnection->CancelTransaction( iTransactionId );
        iTransactionId = 0;
    }
}

void CAgileProvisionServiceService::ResetTransactionId()
{
    iTransactionId = 0;
}
