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

#include <f32file.h>

#include "credentialcache.h"
#include "ikedebug.h"
#include "cmutils.h"


_LIT( KFileName, "cache" );


CCredentialCache* CCredentialCache::NewL( MIkeDebug& aDebug )
{
    CCredentialCache* cache = new (ELeave) CCredentialCache( aDebug );

    CleanupStack::PushL( cache );

    cache->ConstructL();

    CleanupStack::Pop();

    return cache;
}


CCredentialCache::CCredentialCache( MIkeDebug& aDebug )
  : iDebug( aDebug )
{
}


void CCredentialCache::ConstructL()
{
    User::LeaveIfError( iFs.Connect() );
        
    DEBUG_LOG( _L( "CCredentialCache::ConstructL" ) );
}


CCredentialCache::~CCredentialCache()
{
    iFs.Close();
}


void CCredentialCache::SetUserName( const TDesC8& aUser )
{
    if( KErrNone != CheckCredential( aUser ) )
    {
        return;   
    }

    iBuf.iUser.Copy( aUser );
}


void CCredentialCache::SetSecret( const TDesC8& aSecret )
{
    if( KErrNone != CheckCredential( aSecret ) )
    {
        return;   
    }

    iBuf.iSecret.Copy( aSecret );
}


TInt CCredentialCache::GetCredentials(
    const TUint32 aVpnApId, HBufC8*& aUser, HBufC8*& aSecret )
{
    TInt ret = KErrNone;
    
    TRAPD( err, ret = GetCredentialsL( aVpnApId, aUser, aSecret ) );
    
    if( KErrNone != err )
    {
        DEBUG_LOG1( _L("CCredentialCache::GetCredentials, err=%d"), err );
        return err;
    }
    
    return ret;
}


TInt CCredentialCache::GetCredentialsL(
    const TUint32 aVpnApId, HBufC8*& aUser, HBufC8*& aSecret )
{
    TInt ret = ReadFile();

    if( KErrNone != ret )
    {
        return ret;   
    }

    TVpnPolicyId id;
    
    CmUtils::GetPolicyIdL( aVpnApId, id );

    if( id != iBuf.iId )
    {
        DEBUG_LOG1(
            _L("CCredentialCache::GetCredentialsL, pol=%S"), &iBuf.iId
        );
        
        return KErrNotFound;
    }

    aUser   = iBuf.iUser.AllocL();
    aSecret = iBuf.iSecret.AllocL();
    
    return KErrNone;
}


void CCredentialCache::Store( const TUint32 aVpnApId )
{
    TRAPD( err, StoreL( aVpnApId ) );
    
    if( KErrNone != err )
    {
        DEBUG_LOG1( _L("CCredentialCache::Store, err=%d"), err );
    }
}


void CCredentialCache::StoreL( const TUint32 aVpnApId )
{
    CmUtils::GetPolicyIdL( aVpnApId, iBuf.iId );

    StoreToFileL();
}


void CCredentialCache::Clear()
{
    TInt ret = CreateFileNameAndPath();
    
    if( KErrNone != ret )
    {
        return;
    }

    ret = iFs.Delete( iFileName );
    
    if( KErrNone != ret )
    {
        DEBUG_LOG1( _L("CCredentialCache::Clear, ret=%d"), ret );
    }
}


TInt CCredentialCache::CheckCredential( const TDesC8& cr )
{
    TInt len = cr.Length();
    
    if( 0 == len || KCredentialMaxLen < len )
    {
        DEBUG_LOG1( _L("CCredentialCache::CheckCredential, len=%d"), len );
        return KErrArgument;
    }

    return KErrNone;
}


void CCredentialCache::StoreToFileL()
{
    RFile cache;

    User::LeaveIfError( CreateFileNameAndPath() );

    User::LeaveIfError( cache.Replace(
        iFs, iFileName, EFileShareExclusive | EFileWrite
    ) );

    CleanupClosePushL( cache );

    TPckg< TCacheBuffer > data( iBuf );

    User::LeaveIfError( cache.Write( data ) );

    CleanupStack::PopAndDestroy( &cache );
}


TInt CCredentialCache::ReadFile()
{
    RFile cache;
    
    TInt ret = cache.Open( iFs, KFileName, EFileRead );
    
    if( KErrNone != ret )
    {
      return ret;   
    }

    TInt size = 0;
    
    ret = cache.Size( size );

    TPckg< TCacheBuffer > data( iBuf );

    if( size != data.Size() )
    {
        DEBUG_LOG1( _L("CCredentialCache::ReadFile, size=%d"), size );
        cache.Close();
        Clear();
        return KErrCorrupt;
    }

    ret = cache.Read( data );

    cache.Close();
    
    return ret;
}


TInt CCredentialCache::CreateFileNameAndPath()
{
    TInt ret = iFs.CreatePrivatePath( RFs::GetSystemDrive() );

    if( KErrNone          != ret &&
        KErrAlreadyExists != ret )
    {
        DEBUG_LOG1( _L("CCredentialCache, CreatePrivatePath ret=%d"), ret );
        return ret;
    }
 
    ret = iFs.PrivatePath( iFileName );

    if( KErrNone != ret )
    {
        DEBUG_LOG1( _L("CCredentialCache, PrivatePath ret=%d"), ret );
        return ret;
    }

    iFileName.Append( KFileName );
    
    return KErrNone;
}


/***/
