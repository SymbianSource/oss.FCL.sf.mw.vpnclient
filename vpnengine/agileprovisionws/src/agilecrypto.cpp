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
* Description:  VPN Agile provision Crypto library
*
*/

#include <x509cert.h>

#include <x500dn.h>
#include <random.h>
#include <signed.h>

#include "agilecrypto.h"
#include "utlcrypto.h"
#include "base64.h"
#include "pkcs10.h"
#include "vpnclientuids.h"




EXPORT_C CAgileCryptoCert::~CAgileCryptoCert()
/**
* Destructor.
*/
    {

    delete iCertDER;

    }

CAgileCryptoCert::CAgileCryptoCert()
    {
    }



EXPORT_C CAgileCryptoCert* CAgileCryptoCert::NewL(const TDesC8&  aCert, TBool aBase64Encoded)
/**
* Makes crypto certififate object.
* @param aCert Certificate data.
* @param aCert aBase64Encoded If ETrue, certificate data is base64 encoded.
* @return CAgileCryptoCert* Pointer to crypto certififate object.
*/
    {
    CAgileCryptoCert* self = new (ELeave) CAgileCryptoCert();
    CleanupStack::PushL(self);
    self->ConstructL(aCert, aBase64Encoded);
    CleanupStack::Pop();        // self
    return self;
    }
 

void CAgileCryptoCert::ConstructL(const TDesC8& aCert, TBool aBase64Encoded)
    {

    HBufC8* certDerCodedHBuf;
    TPtrC8 certDerCoded;
    
    if (aBase64Encoded)
        {
        TBase64Codec base64Codec;
        if ((certDerCodedHBuf = base64Codec.Base64DecodeLC(aCert)) == NULL)
            {
            User::Leave(KErrNoMemory);
            }
        }
    else
        {
        certDerCodedHBuf = aCert.AllocLC();
        }
    certDerCoded.Set(certDerCodedHBuf->Des());
    
    iCertDER = certDerCodedHBuf;

    CleanupStack::Pop(1);  //certDerCodedHBuf
    }

EXPORT_C void CAgileCryptoCert::SaveCACertL(void)   
/**
* Saves server certififate to certificate store.
* @return TRetBufCertRef The identifier of the certificate.
*/
    {
    RPKIServiceAPI pkiServiceApi;
    CleanupClosePushL(pkiServiceApi);
    User::LeaveIfError(pkiServiceApi.Connect());

    TPtr8 certDERDesc(iCertDER->Des());
    TRequestStatus requestStatus;
    TAny* resArray;
    TUint keySize = 0;
    pkiServiceApi.StoreCertificateL(EPKICACertificate,
                                    keySize,
                                    EPKIRSA,
                                    certDERDesc,
                                    &resArray,
                                    requestStatus);
    User::WaitForRequest(requestStatus);
    pkiServiceApi.Finalize(resArray);
    TInt status = requestStatus.Int();
    if (status != KErrNone)
        {
        User::Leave(status);
        }
    
    
    // Set VPN trusted
    CX509Certificate* tempCert = CX509Certificate::NewLC(certDERDesc);
    RArray<TUid> appArray;
    CleanupClosePushL(appArray);
    appArray.AppendL(TUid::Uid(KUidVpnManager));

    const TPtrC8* serialNumber = tempCert->DataElementEncoding(
        CX509Certificate::ESerialNumber);
    const TPtrC8* issuername = tempCert->DataElementEncoding(
        CX509Certificate::EIssuerName);

    pkiServiceApi.SetApplicabilityL(
        *issuername,
        *serialNumber,
        appArray);

    CleanupStack::PopAndDestroy(3); // appArray, tempCert, pkiServiceApi
    
    }

EXPORT_C void CAgileCryptoCert::SaveClientCertL(const TPrivKeyRef& aKeyId, TUint aKeySize)   
/**
* Saves client certififate to certificate store.
* @param aKeyId Private key identifier.
* @param aKeySize Private key size.
* @return TRetBufCertRef The identifier of the certificate
*/
    {
    RPKIServiceAPI pkiServiceApi;
    CleanupClosePushL(pkiServiceApi);
    User::LeaveIfError(pkiServiceApi.Connect());

    TPtr8 certDERDesc(iCertDER->Des());
    TRequestStatus requestStatus;
    TAny* resArray;
    pkiServiceApi.AttachCertificateL(aKeyId,
                                     aKeySize,
                                     EPKIRSA,
                                     certDERDesc,
                                     &resArray,
                                     requestStatus);
    User::WaitForRequest(requestStatus);
    pkiServiceApi.Finalize(resArray);
    TInt status = requestStatus.Int();
    if (status != KErrNone)
       User::LeaveIfError(status);
        
    CleanupStack::PopAndDestroy(); //pkiServiceApi


    }

EXPORT_C CAgileCryptoPrivKey* CAgileCryptoPrivKey::NewL(void)
/**
* Makes crypto private key object.
* @return CAgileCryptoPrivKey* Pointer to crypto private key object.
*/
    {
    CAgileCryptoPrivKey* self = new (ELeave) CAgileCryptoPrivKey();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();        // self
    return self;
    }

void CAgileCryptoPrivKey::ConstructL(void)
    {
    User::LeaveIfError(iPkiServiceApi.Connect());
    }
    
    
CAgileCryptoPrivKey::CAgileCryptoPrivKey()
    {
    ;
    }

EXPORT_C CAgileCryptoPrivKey::~CAgileCryptoPrivKey()
/**
* Destructor.
*/
    {
    iPkiServiceApi.Close();
    }


EXPORT_C void CAgileCryptoPrivKey::GenerateCertReqL(const TDesC8&          aDistinguishedName,
                                                      const TDesC8&          aSubjectAltNameRfc822,
                                                      const TDesC8&          aChallengePassword,
                                                      const TDesC8&          aDnsName,
                                                      const TPrivKeyRef&     aKeyId,
                                                      TDes&                  aCertReqRef,
                                                      TInt&                  aCertReqSize)
/**
* Creates certificate request.
* @param aDistinguishedName Subject name of the certificate owner.
* @param aSubjectAltNameRfc822 SubjectAlt name of the certificate owner.
* @param aChallengePassword ChallengePw of the certificate owner.
* @param aDnsName DNS name of the certificate owner.
* @param aKeyId KeyId of the key for which the certificate will be generated.
* @param aCertReqRef The identifier of created certificate request (output parameter).
* @param aCertReqSize The size of created certificate request (output parameter).
* @return TErrCode KErrNone, if OK.
*/
    {
    
    iPkiServiceApi.CreateAndSaveCertificateRequestL(aKeyId,
                                                    aDistinguishedName,
                                                    aSubjectAltNameRfc822,       
                                                    aChallengePassword,
                                                    aDnsName,
                                                    aCertReqRef,
                                                    aCertReqSize);

    }

EXPORT_C TRetBufCertReq CAgileCryptoPrivKey::ReadCertReqL(const TDesC& aCertReqRef, TInt aCertReqSize)
/**
* Reads certififate certificate.
* @param aCertReqRef Certificate request identifier.
* @param aCertReqSize The size of the certificate request.
* @return TRetBufCertReq Certificate request.
*/
    {
    HBufC8* certRequest = HBufC8::NewLC(aCertReqSize);
    TPtr8 certRequestDesc(certRequest->Des());
    

    TInt status = iPkiServiceApi.ReadCertificateRequest(aCertReqRef,
                                                        certRequestDesc);
    if (status != KErrNone)
        {
        User::Leave(KAgileErrCryptoReadCertRequestFailed);
        }
    CleanupStack::Pop(); //certRequest
    
    return certRequest;
    }

RPKIServiceAPI CAgileCryptoPrivKey::GetPkiService()
    {
    return iPkiServiceApi;
    }

    
EXPORT_C CAgileCryptoGenerateKeypair* CAgileCryptoGenerateKeypair::NewL(CAgileCryptoPrivKey* aAgileCryptoPrivKey)
/**
* Makes crypto generate key pair object.
* @param aAgileCryptoPrivKey Crypto private key object (contains session to PKI service).
* @return CAgileCryptoGenerateKeypair crypto generate key pair object.
*/
    {
    CAgileCryptoGenerateKeypair* self = new (ELeave) CAgileCryptoGenerateKeypair(aAgileCryptoPrivKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();        // self
    return self;
    }

void CAgileCryptoGenerateKeypair::ConstructL(void)
    {
    ;
    }
    
    
CAgileCryptoGenerateKeypair::CAgileCryptoGenerateKeypair(CAgileCryptoPrivKey* aAgileCryptoPrivKey)
    : iAgileCryptoPrivKey(aAgileCryptoPrivKey)
    {
    ;
    }

EXPORT_C TErrCode CAgileCryptoGenerateKeypair::GenerateKeypairL(const TInt&           aPublicKeyLen,
                                                              TPrivKeyRef&          aKeyId,
                                                              TRequestStatus&       aStatus)
/**
* Generates key pair.
* @param aPublicKeyLen The length of public key.
* @param aKeyId Key pair (private key) identifier (output parameter).
* @param aStatus Asynchronous request status.
* @return TErrCode KErrNone, if OK.
*/
    {
   // aStatus = KRequestPending;
    
    iAgileCryptoPrivKey->GetPkiService().GenerateKeypair(aKeyId,
                                                      (TUint)aPublicKeyLen,
                                                      EPKIRSA,
                                                      //&iResArray,
                                                      aStatus);
    return 0;
    }
    
EXPORT_C void CAgileCryptoGenerateKeypair::GenerateKeypairCancel(void)
/**
* Cancels operation.
*/
    {
    iAgileCryptoPrivKey->GetPkiService().CancelPendingOperation();
    }

