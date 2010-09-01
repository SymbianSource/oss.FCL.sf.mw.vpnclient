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

#include <SenXmlServiceDescription.h>
#include <SenServiceConnection.h>
#include <SenServiceManager.h>
#include <SenIdentityProvider.h>
#include "agileprovisionws.h"
#include "agileprovisiondefs.h"
#include "policyinstaller_constants.h"
#include "base64.h" 


EXPORT_C CAgileProvisionWs* CAgileProvisionWs::NewL()
    {
	CAgileProvisionWs* self = NewLC();
	CleanupStack::Pop(); // self
	return self;
    }


EXPORT_C CAgileProvisionWs* CAgileProvisionWs::NewLC() 	
    {
	CAgileProvisionWs* self = new (ELeave) CAgileProvisionWs();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

CAgileProvisionWs::CAgileProvisionWs()
    {
    }


void CAgileProvisionWs::ConstructL()
    {
    }


CAgileProvisionWs::~CAgileProvisionWs()
    {
    delete iPService;
    delete iServiceAddr;
    }

EXPORT_C void CAgileProvisionWs::GetPolicy(const TDesC8& aEndPointUri, const TUint aIapId,
                                           TRequestStatus& aStatus)
    {
    __ASSERT_DEBUG( iClientStatus == NULL,
                    User::Invariant() );
    
    // Store client's request status.
       
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;

    TRAPD( err, DoGetPolicyL( aEndPointUri, aIapId) );
    if ( err != KErrNone )
        {
        DoComplete( err );
        }
    }

EXPORT_C void CAgileProvisionWs::CancelGetPolicy()
    {
    if ( iPService != NULL )
        {
        iPService->CancelTransaction();
        delete iPService;
        iPService = NULL;
        }
    DoComplete( KErrCancel );
    }

EXPORT_C void CAgileProvisionWs::SetServiceAddr(const HBufC8* aServiceAddr)
    {
    iServiceAddr=aServiceAddr;
    }

void CAgileProvisionWs::DoGetPolicyL(const TDesC8& aEndPointUri, const TUint aIapId)
    {
    
    CSenXmlServiceDescription* pServiceDesc = CSenXmlServiceDescription::NewLC( aEndPointUri,
                                                                                    KNullDesC8() );
    iEndPointUri=&aEndPointUri;
    pServiceDesc->SetFrameworkIdL( KDefaultBasicWebServicesFrameworkID );            
    delete iPService;
    iPService = NULL;

    iPService  = CAgileProvisionServiceService::NewL( *this, *pServiceDesc );
    
    CleanupStack::PopAndDestroy( pServiceDesc );
    
    iPService->iIapId= aIapId;   
    
    iState = KStateGettingConf;
      
    }

void CAgileProvisionWs::HandleMessageL(const TDesC8& aMessage)
{
    
    iPService->ResetTransactionId();
    
    if ( iState == KStateGettingConf )
        {
        HandleGetConfSoapInL( aMessage );
        GenerateCertReqL();
        iPService->GetCertificateAsyncL(iPService->iCertreq);
        iState = KStateGettingCertificate;
        }
    else if ( iState == KStateGettingCertificate )
        {
        HandleGetCertificateSoapInL( aMessage );        
        DoComplete( KErrNone );
        iState = KStateInitial;
        }
    else
        {
        User::Leave( KErrArgument );
        }
}

void CAgileProvisionWs::HandleErrorL(const TInt aErrorCode, const TDesC8&  /*aError*/)
    {
    DoComplete( aErrorCode );
    }


void CAgileProvisionWs::SetStatus(const TInt aStatus)
    {
    TInt err = KErrNone;
    if ( aStatus == KSenConnectionStatusReady )
        {
        TRAP( err, iPService->GetConfAsyncL() );
        }
    else if ( aStatus == KSenConnectionStatusCreateFailed )
        {
        err = aStatus;
        }
    else
        {
        // Do nothing.
        }
    
    if ( err != KErrNone )
        {
        DoComplete( err );
        }
    }

EXPORT_C void CAgileProvisionWs::HandleGetConfSoapInL(const TDesC8& aMessage)
    {
    CSenXmlReader *pXmlReader = CSenXmlReader::NewL();
    CleanupStack::PushL(pXmlReader);
     
    //CSenDomFragment
    CSenDomFragment* pBase = CSenDomFragment::NewL();
    CleanupStack::PushL(pBase);

    //must set the content handler
    pXmlReader->SetContentHandler(*pBase);
   // and the reader
    pBase->SetReader(*pXmlReader);

    pXmlReader->ParseL(aMessage);
    
    RPointerArray<CSenElement>& array = pBase->AsElement().ElementsL();
        
	//start the listing of the elements, first level is 1
    TInt contentType(1);
		
	TInt vpnFilePathLth = KAgileProvisionPolicyFileName().Length() + KTempDirectory().Length();
	HBufC* vpnFilePathBuf = HBufC::NewLC(vpnFilePathLth);
	TPtr vpnFilePath(vpnFilePathBuf->Des());
	vpnFilePath.Copy(KTempDirectory);
	vpnFilePath.Append(KAgileProvisionPolicyFileName);
       
	ParseFileFromSoapEnvelopeL(array, contentType, 1, vpnFilePath);
    
    CleanupStack::PopAndDestroy(vpnFilePathBuf); // file
    CleanupStack::PopAndDestroy(pBase); // delete pXmlReader, procMsg, responseStr
    CleanupStack::PopAndDestroy(pXmlReader); // delete pXmlReader, procMsg, responseStr
    }


EXPORT_C void CAgileProvisionWs::HandleGetCertificateSoapInL(const TDesC8& aMessage)
    {
    CSenXmlReader *pXmlReader = CSenXmlReader::NewL();
    CleanupStack::PushL(pXmlReader);
     
    //CSenDomFragment
    CSenDomFragment* pBase = CSenDomFragment::NewL();
    CleanupStack::PushL(pBase);

    //must set the content handler
    pXmlReader->SetContentHandler(*pBase);
   // and the reader
    pBase->SetReader(*pXmlReader);

    pXmlReader->ParseL(aMessage);
    
    RPointerArray<CSenElement>& array = pBase->AsElement().ElementsL();

    //start the listing of the elements, first level is 1
       
    TInt vpnFilePathLth = KAgileProvisionCertificateFileName().Length() + KTempDirectory().Length();
    HBufC* vpnFilePathBuf = HBufC::NewLC(vpnFilePathLth);
    TPtr vpnFilePath(vpnFilePathBuf->Des());
    vpnFilePath.Copy(KTempDirectory);
    vpnFilePath.Append(KAgileProvisionCertificateFileName);
    TInt contentType(2);
    ParseFileFromSoapEnvelopeL(array, contentType, 1, vpnFilePath);
    
    CleanupStack::PopAndDestroy(vpnFilePathBuf); // file
    CleanupStack::PopAndDestroy(pBase); // delete pXmlReader, procMsg, responseStr
    CleanupStack::PopAndDestroy(pXmlReader); // delete pXmlReader, procMsg, responseStr
    
    }

// The listing of the elements is done in ListNodesL function:
void CAgileProvisionWs::ParseFileFromSoapEnvelopeL(RPointerArray<CSenElement>& aArray, 
                                                       TInt aContentType,TInt aLevel, TDesC& aFileName )
    {

    TInt size = aArray.Count();
    for(TInt i=0; i<size; i++)
        {
        //get an element
        CSenElement* pElement = aArray[i];
        if ( pElement->HasContent() )
            {
            //get the content of the element
            TPtrC8 content =  pElement->Content();
            TBase64Codec* base64Inst = new (ELeave) TBase64Codec();
            CleanupStack::PushL(base64Inst);
            HBufC8* dataBuf = base64Inst->Base64DecodeLC(content);
            HBufC8* fileBuf = base64Inst->Base64DecodeLC(*dataBuf);
                       
            //CA from getConf envelope
            if ( aContentType == 1 && i==1 )
                {
                CAgileCryptoCert* agileClientCert = CAgileCryptoCert::NewL(fileBuf->Des(), EFalse);
                CleanupStack::PushL(agileClientCert);
                agileClientCert->SaveCACertL();
                CleanupStack::PopAndDestroy(agileClientCert);
                }
            else if ( aContentType == 2)
                {
                CAgileCryptoCert* agileClientCert = CAgileCryptoCert::NewL(fileBuf->Des(), ETrue);
                CleanupStack::PushL(agileClientCert);
                agileClientCert->SaveClientCertL(iKeyRef,KPublicKeyLen);
                CleanupStack::PopAndDestroy(agileClientCert);
                }
            else           
                {
                RFile file;
                RFs iFs;
                TInt err= iFs.Connect();
                CleanupClosePushL(file);
                User::LeaveIfError(file.Replace(iFs, aFileName, EFileWrite));
                User::LeaveIfError(file.Write(*fileBuf));
                
                //write pin file
                TInt vpnPinFilePathLth = KAgileProvisionPinFileName().Length() + KTempDirectory().Length();
                HBufC* vpnPinFilePathBuf = HBufC::NewLC(vpnPinFilePathLth);
                TPtr vpnPinFilePath(vpnPinFilePathBuf->Des());
                vpnPinFilePath.Copy(KTempDirectory);
                vpnPinFilePath.Append(KAgileProvisionPinFileName);
                User::LeaveIfError(file.Replace(iFs, vpnPinFilePath, EFileWrite));
                CleanupStack::PopAndDestroy(vpnPinFilePathBuf); // file                
                file.Flush();
                file.Close();
                CleanupStack::PopAndDestroy(&file);
                iFs.Close();
                }
            
            
			CleanupStack::PopAndDestroy(fileBuf);
            CleanupStack::PopAndDestroy(dataBuf);
            CleanupStack::PopAndDestroy(base64Inst);
                       
            
            }
  
        //get the first child
        CSenElement* child = pElement->Child(0);

        //check if element has childs
        if( child )
            {
            //get the child elements
            RPointerArray<CSenElement>& tree = pElement->ElementsL();

            //list child elements by a recursive call
            ParseFileFromSoapEnvelopeL(tree, aContentType, ++aLevel, aFileName);
            }
        aLevel--;
        } //for loop ends
  }

EXPORT_C void CAgileProvisionWs::GenerateCertReqL()
    {
    TRequestStatus status;
    CAgileCryptoPrivKey* agileCryptoPrivKey = CAgileCryptoPrivKey::NewL();
    CleanupStack::PushL(agileCryptoPrivKey);
    CAgileCryptoGenerateKeypair* agileCryptoGenerateKeypair = CAgileCryptoGenerateKeypair::NewL(agileCryptoPrivKey);
    CleanupStack::PushL(agileCryptoGenerateKeypair);
    
    // generate key
    agileCryptoGenerateKeypair->GenerateKeypairL(KPublicKeyLen, iKeyRef, status);
    User::WaitForRequest(status);
    
    // generate subjectname
    _LIT8(KCnHeader, "CN=");
    _LIT8(KCnUsername, "vpnuser");
    TInt distinguishedNameLth = KCnHeader().Length() + KCnUsername().Length();
    HBufC8* distinguishedNameHBuf = HBufC8::NewLC(distinguishedNameLth);
    TPtr8 distinguishedName(distinguishedNameHBuf->Des());
    distinguishedName.Copy(KCnHeader);
    distinguishedName.Append(KCnUsername);

    // generate certreq
    const TInt KDmAdMaxCertReqRefLth = KMaxFileName;
    HBufC* certReqRef = HBufC::NewLC(KDmAdMaxCertReqRefLth);
    TPtr certReqRefPtr(certReqRef->Des());
    TInt certReqSize;
    
    agileCryptoPrivKey->GenerateCertReqL(*distinguishedNameHBuf, //subjectname
                                                           KNullDesC8, // subjectaltname
                                                           KNullDesC8, //aChallengePassword,
                                                           KNullDesC8, //DnsName,
                                                           iKeyRef,
                                                           certReqRefPtr,
                                                           certReqSize);


    // read certreq
    TRetBufCertReq certReq;
    certReq = agileCryptoPrivKey->ReadCertReqL(certReqRefPtr, certReqSize);
    if ( iPService )
        {
        iPService->iCertreq = certReq;
        }
    else
        {
        delete certReq;
        certReq=NULL;
        }
    
    CleanupStack::PopAndDestroy(certReqRef);
    CleanupStack::PopAndDestroy(distinguishedNameHBuf);
    CleanupStack::PopAndDestroy(agileCryptoGenerateKeypair);
    CleanupStack::PopAndDestroy(agileCryptoPrivKey);            
    }

void CAgileProvisionWs::DoComplete( const TInt aStatus )
    {
    if ( iClientStatus != NULL )
        {
        User::RequestComplete( iClientStatus,
                               aStatus );
        }
    }
