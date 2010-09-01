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

#ifndef __AGILE_CRYPTO_H
#define __AGILE_CRYPTO_H

#include "pkiserviceapi.h"


const TInt KAgileCryptoMaxSignatureLth = 512; //publicKey->Size() / 8;

typedef TPKIKeyIdentifier TPrivKeyRef;
typedef HBufC8*           TRetBufCertReq;
typedef HBufC8*           TRetBufCertRef;
typedef TInt              TErrCode;

// --- agile crypto error codes
const TInt KAgileErrCryptoCertFileNotFound            = -102101;
const TInt KAgileErrCryptoReadCertRequestFailed       = -102103;


class CAgileCryptoCert : public CBase
/** 
* Certificate operations
* @internalComponent
*/
{
public:
    
     
    IMPORT_C static CAgileCryptoCert* NewL(const TDesC8& aCert, TBool aBase64Encoded);
    IMPORT_C ~CAgileCryptoCert();                      //agile_crypto_cert_free

    IMPORT_C void SaveCACertL(void);    
    IMPORT_C void SaveClientCertL(const TPrivKeyRef& aKeyId, TUint aKeySize);
    
private:
    CAgileCryptoCert();
  
    void ConstructL(const TDesC8& aCert, TBool aBase64Encoded);
    
    
private:
    HBufC8              *iCertDER;
    
};

class CAgileCryptoGenerateKeypair;
class CAgileCryptoPrivKey : public CBase
/** 
* Private key operations
* @internalComponent
*/
{
public:
    //friend class CAgileCryptoGenerateKeypair;
    
    IMPORT_C static CAgileCryptoPrivKey* NewL(void);
    IMPORT_C ~CAgileCryptoPrivKey();
    

    IMPORT_C void GenerateCertReqL(const TDesC8&          aDistinguishedName,
                                       const TDesC8&          aSubjectAltNameRfc822,
                                       const TDesC8&          aChallengePassword,
                                       const TDesC8&          aDnsName,
                                       const TPrivKeyRef&     aKeyId,
                                       TDes&                  aCertReqRef,
                                       TInt&                  aCertReqSize); //agile_crypto_generate_cert_req_pem
    IMPORT_C TRetBufCertReq ReadCertReqL(const TDesC& aCertReqRef, TInt aCertReqSize);

    RPKIServiceAPI GetPkiService();
    
private:
    CAgileCryptoPrivKey();
    void ConstructL(void);


private:
    RPKIServiceAPI iPkiServiceApi;
};


class CAgileCryptoGenerateKeypair : public CBase
/** 
* Generate key pair
* @internalComponent
*/
{
public:
    IMPORT_C static CAgileCryptoGenerateKeypair* NewL(CAgileCryptoPrivKey* aAgileCryptoPrivKey);
    IMPORT_C TErrCode GenerateKeypairL(const TInt&          aPublicKeyLen,
                                       TPrivKeyRef&         aKeyId,
                                       TRequestStatus&      aStatus);
    IMPORT_C void GenerateKeypairCancel(void);
    

private:
    CAgileCryptoGenerateKeypair(CAgileCryptoPrivKey* aAgileCryptoPrivKey);
    void ConstructL(void);

private:
    CAgileCryptoPrivKey*  iAgileCryptoPrivKey;

};

#endif
