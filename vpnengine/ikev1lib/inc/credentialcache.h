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
* Description:  Cache for authentication credentials
*
*/

#ifndef CREDENTIALCACHE_H
#define CREDENTIALCACHE_H

#include <e32base.h>
#include <vpnapidefs.h>


class MIkeDebug;


const TInt KCredentialMaxLen = 64;


/**
 * Cache buffer stored to file.
 */
struct TCacheBuffer{
  TVpnPolicyId             iId;
  TBuf8<KCredentialMaxLen> iUser;
  TBuf8<KCredentialMaxLen> iSecret;
};


/**
 * Cache for authentication credentials.
 * The public interface of the class has been
 * designed for CTransNegotiation use.
 *
 * Error handling:
 * The methods return error code or leave only if
 * it is useful for the method caller to handle the error.
 * The user of CCredentialCache must work even if the
 * cache fails. (The credentials are asked from user in that case.)
 *
 * Example usage sequence:
 *   NewL
 *   SetUserName
 *   SetSecret
 *   Store
 *   GetCredentials
 */
NONSHARABLE_CLASS( CCredentialCache ) : public CBase{
  public:
    /**
     * Two-phased constructor.
     * @param aDebug Debug log interface.
     */
    static CCredentialCache* NewL( MIkeDebug& aDebug );

    ~CCredentialCache();

    /**
     * Sets user-name. Does not store to file.
     * @param aUser User name.
     */
    void SetUserName( const TDesC8& aUser );

    /**
     * Sets secret, e.g. password. Does not store to file.
     * @param aSecret Secret, e.g. password.
     */
    void SetSecret( const TDesC8& aSecret );

    /**
     * Gets credentials from cache file.
     * Caller is responsible for deallocating aUser and aSecret.
     *
     * @param aVpnApId VPN access point id
     * @param aUser On return, user name.
     * @param aSecret On return, secret.
     *
     * @return KErrNone if credentials are fetched from cache.
     * @return System-wide error code if cached credentials are not available.
     */
    TInt GetCredentials(
        const TUint32 aVpnApId, HBufC8*& aUser, HBufC8*& aSecret
    );

    /**
     * Stores user-name and secret to private file.
     * @param aVpnApId VPN access point id.
     */
    void Store( const TUint32 aVpnApId );

    /**
     * Clears cache.
     */
    void Clear();

  private:
    CCredentialCache( MIkeDebug& aDebug );

    void ConstructL();

    /**
     * Gets credentials from cache file.
     */
    TInt GetCredentialsL(
        const TUint32 aVpnApId, HBufC8*& aUser, HBufC8*& aSecret
    );

    /**
     * Stores user-name and secret to private file.
     */
    void StoreL( const TUint32 aVpnApId );

    TInt CheckCredential( const TDesC8& cr );

    /**
     * Stores cache to private file.
     */
    void StoreToFileL();

    /**
     * Reads cache data to iBuf.
     */ 
    TInt ReadFile();

    /**
     * Stores file name with path to iFileName.
     * Creates private path if needed.
     */
    TInt CreateFileNameAndPath();

    RFs iFs;

    TCacheBuffer iBuf;

    TFileName iFileName;

    MIkeDebug& iDebug;
};


#endif  // CREDENTIALCACHE_H
