/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CPKIMapper class holds the information required to map API set 
*				 to use the storage model which is not native for that API.
*
*/



#if !defined (__PKIMAPPER_H__)
#define __PKIMAPPER_H__

#include <e32base.h>
#include <securitydefs.h>

#include "pkidefs.h"
#include "pkiserviceclientservercommon.h"

class TSecurityObjectDescriptor;
class CX500DistinguishedName;
class CX520AttributeTypeAndValue;

enum TValidity
{
    EValid,
    EExpired,
    ENotValidYet
};


/**
 *  CMapDescriptor
 *
 *  Maintains information of installed certificates and keys
 *
 *  @lib internal (pkiservice.exe)
 *  @since S60 v3.0
 */
class CMapDescriptor : public CBase
{
    public:
        CMapDescriptor()
                {
                iOwnerType = EPKICACertificate;
                iKeySize = 0;
                iKeyAlgorithm = EPKIInvalidAlgorithm;
                };

        ~CMapDescriptor()
            {
            delete iTrustedAuthority;
            iTrustedAuthority = NULL;
            delete iIdentitySubjectName;
            iIdentitySubjectName = NULL;
            delete iIdentityRfc822Name;
            iIdentityRfc822Name = NULL;
            delete iSerialNumber;
            iSerialNumber = NULL;
            iApplUids.Close();
            };
        CMapDescriptor(TSecurityObjectDescriptor& aDesc);
        CMapDescriptor& operator=(CMapDescriptor& aMapDesc);
        void SetMapObjectName(const TDesC& aFilename) {iObjectName.Copy(aFilename);};
        TBool IsMatchingL(TSecurityObjectDescriptor &aDesc, 
                          const TBool aInfoOnly, 
                          TPkiServiceStoreType aCertStoreType) const;
                          
        TBool IsEqual(CMapDescriptor &aDesc);
        void SetMapSubjectKeyId(const TPKIKeyIdentifier &aKeyId)
            {
            iSubjectKeyId.Copy(aKeyId);
            };
        void SetMapOwnerType(const TPKICertificateOwnerType aOwnerType) {iOwnerType = aOwnerType;};        
        void SetMapKeyUsageDer(const TDesC8 &aKeyUsage)
            {
            iKeyUsageDer.Copy(aKeyUsage);
            };                          // Optional if only certificate is needed
        // Key usage is not defined in the filter, this will be checked separately
        void SetMapKeySize(const TUint aKeySize) {iKeySize = aKeySize;};                // Optional if only certificate is needed
        // Issuer and serial are not defined in the filter, these will be checked separately
        void SetMapTrustedAuthorityL(const TDesC8 &aTrustedAuthority)
            {
            delete iTrustedAuthority;
            iTrustedAuthority = NULL;
            iTrustedAuthority = aTrustedAuthority.AllocL();
            };
        void SetMapIdentitySubjectNameL(const TDesC8 &aIdentitySubjectName)
            {
            delete iIdentitySubjectName;
            iIdentitySubjectName = NULL;
            iIdentitySubjectName = aIdentitySubjectName.AllocL();
            };
        void SetMapIdentityRfc822NameL(const TDesC8 &aIdentityRfc822Name)
            {
            delete iIdentityRfc822Name;
            iIdentityRfc822Name = NULL;
            iIdentityRfc822Name = aIdentityRfc822Name.AllocL();
            };
        void SetMapSerialNumberL(const TDesC8 &aSerialNumber)
            {
            delete iSerialNumber;
            iSerialNumber = NULL;
            iSerialNumber = aSerialNumber.AllocL();
            };

        void SetCertStoreType(TPkiServiceStoreType aCertStoreType);

        void SetMapKeyAlgorithm(const TPKIKeyAlgorithm &aKeyAlgorithm) {iKeyAlgorithm = aKeyAlgorithm;};
        void SetMapStartTime(const TTime &aTime) {iStartTime = aTime;};
        void SetMapEndTime(const TTime &aTime) {iEndTime = aTime;};
        void SetMapTrusted(const TBool &aTrusted) {iIsTrusted = aTrusted;};
        void SetMapIsDeletable(const TBool &aIsDeletable) {iIsDeletable = aIsDeletable;};
		void SetMapApplications(const RArray<TUid> &aApplications) {for(TInt i=0; i<aApplications.Count();i++){ iApplUids.Append(aApplications[i]);}};
        
        TBool IsMatchingCertStore(const TDesC& aCertStoreName);

    public: // data
        // No get methods introduced, values are used directly!     
        
        /// Key or certificate: used internally, depends on the operation              
        /// Own: Cert TrustedAuthority
        HBufC8*                     iTrustedAuthority;          
        HBufC8*                     iIdentitySubjectName;       // Identity subject name
        HBufC8*                     iIdentityRfc822Name;        // Identity subjectAltName rfc822 name
        HBufC8*                     iSerialNumber;              // Serialnumber
        TPKIKeyIdentifier           iSubjectKeyId;              // SHA1 hash of the corresponding private key
        TPKICertificateOwnerType    iOwnerType;                 // User, CA or peer. If user certificate, at least key usage must be set
        TBuf8<KMaxUsageDer>         iKeyUsageDer;               // Der format flags
        TBuf<SHORT_FILENAME_LENGTH> iObjectName;
        TUint                       iKeySize;                   // Key size
        TPKIKeyAlgorithm            iKeyAlgorithm;              // RSA, DSA
        TTime                       iStartTime;
        TTime                       iEndTime;
        TBool                       iIsDeletable;              
        TBool                       iIsTrusted;
        RArray<TUid>                iApplUids;
        TPkiServiceStoreType        iCertStoreType;
};

/**
 *  CPKIMapper
 *
 *  Maintains array of CMapDescriptor objects
 *
 *  @lib internal (pkiservice.exe)
 *  @since S60 v3.0
 */
class CPKIMapper : public CBase
{
    public:
        // Constructors, destructor
        // When constructing an invocation, check if some key/certificate has been manually removed.
        static CPKIMapper* NewL();
        static TValidity CertValidity(const TTime &aStartTime, const TTime &aEndTime);
        ~CPKIMapper();
        
        ////////////////////////////////////////////////////////////////////////////////////////////
        // Mapping methods
        ////////////////////////////////////////////////////////////////////////////////////////////
        // These are new methods, which will be called from ipsecmanager when importing policy or deleting a certificate/key
        // In acuagent, these will be called after user key has been generated and authorized by CA
        TInt AddMapping(CMapDescriptor &aMap);
        TInt DeleteMapping(CMapDescriptor &aDesc);        
        CMapDescriptor& GetMapDescriptorAtIndex(TInt aIndex);
        // One-to-one mapping functions        
        void GetCertificateKeyIdL(TSecurityObjectDescriptor &aDescriptor, TPKIKeyIdentifier &aKeyId,
                                  TPkiServiceStoreType aStoreType = EPkiStoreTypeUser) const;
                                 
        TInt ResolveCertMappingL(TSecurityObjectDescriptor &aDescriptor, TDes16 &aFilename, 
                                TInt &aIndex, const TBool aInfoOnly, 
                                TPkiServiceStoreType aStoreType = EPkiStoreTypeUser) const;
        
        TInt CertCount(void);
		TInt ApplicableCertCount(const RArray<TUid>& aUidArray);		
        TInt GetCertListL(const RMessage2& aMessage, TPkiServiceStoreType aStoreType, TBool aDescUsed = EFalse);
		void GetApplicableCertListL(const RMessage2& aMessage, const RArray<TUid>& aUidArray);               
        		    
        // Function to check whether the given certificate is unique (doesn't exist in cert store)
        TBool CertificateIsUniqueL(const TDesC8& aCertData);
        
        // Function that returns a guaranteedly unique certificate label.
        void GenerateUniqueNameL(const TDesC8& aCertData, TDes& aName, 
                                 TCertificateOwnerType aOwnerType = ECACertificate);       
        void SetCacheCreated();
        TBool CacheCreated();        
    
    private: // implementation
        
        void DeleteMapping(TInt aIndex);        
        TBool LabelIsUnique(const TDesC& aLabel) const;
        void LogMap(CMapDescriptor& aDescriptor) const;
        void LogSearchArguments(TSecurityObjectDescriptor& aDescriptor) const;        
                
    private: // C'tor    

        CPKIMapper();
        void ConstructL();
    
    private: // data    
        TBool   iCacheCreated;       
        TInt    iCount;
        /// Used when generating uniqname

        RPointerArray<CMapDescriptor>*      iMapping;
        TPckgBuf<TSecurityObjectDescriptor> iCurrentDescriptor;
    	TBuf<MAX_FILENAME_LENGTH>           iObjectName;
};

class PkiUtil
{
    public:
        static TBool MatchL(const CX500DistinguishedName& aDn1,
                            const CX500DistinguishedName& aDn2);

    private:    
        static TBool HasElementL(const CX500DistinguishedName& aDn,
                                 const CX520AttributeTypeAndValue& aElement);
};


#endif