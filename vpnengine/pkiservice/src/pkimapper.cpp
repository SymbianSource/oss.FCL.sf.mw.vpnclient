/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* CPKIMapper class holds the information required to map API set to 
* use the storage model which is not native for that API.
*
*/


#include <x500dn.h>
#include <x509cert.h>

#include "PKIMapper.h"
#include "pkcs10.h"
#include "log_r6.h"
#include "pkiserviceassert.h"


CMapDescriptor::CMapDescriptor(TSecurityObjectDescriptor &aDesc)
    {
    this->iSubjectKeyId = aDesc.iSubjectKeyId;
    this->iOwnerType = aDesc.iOwnerType;
    this->iKeySize = aDesc.iKeySize;            
    this->iKeyAlgorithm = aDesc.iKeyAlgorithm;      
    this->iIsDeletable = aDesc.iIsDeletable;      
    }

CMapDescriptor& CMapDescriptor::operator=(CMapDescriptor& aMapDesc)
{
    delete this->iTrustedAuthority;
    this->iTrustedAuthority = NULL;
    if(aMapDesc.iTrustedAuthority != NULL)
        {
        this->iTrustedAuthority     = aMapDesc.iTrustedAuthority->Des().AllocL();           // Cert TrustedAuthority
        }
    delete this->iIdentitySubjectName;
    this->iIdentitySubjectName = NULL;
    if(aMapDesc.iIdentitySubjectName != NULL)
        {
        this->iIdentitySubjectName  = aMapDesc.iIdentitySubjectName->Des().AllocL();        // Identity subject name
        }
    delete this->iIdentityRfc822Name;
    this->iIdentityRfc822Name = NULL;
    if(aMapDesc.iIdentityRfc822Name != NULL)
        {
        this->iIdentityRfc822Name   = aMapDesc.iIdentityRfc822Name->Des().AllocL();         // Identity subjectAltName rfc822 name
        }
    delete this->iSerialNumber;
    this->iSerialNumber = NULL;
    if(aMapDesc.iSerialNumber != NULL)
        {
        this->iSerialNumber         = aMapDesc.iSerialNumber->Des().AllocL();               // Serialnumber
        }
        
    this->iSubjectKeyId         = aMapDesc.iSubjectKeyId;                               // SHA1 hash of the corresponding private key
    this->iOwnerType            = aMapDesc.iOwnerType;                                  // User, CA or peer. If user certificate, at least key usage must be set
    this->iKeyUsageDer          = aMapDesc.iKeyUsageDer;                                // Der format flags
    this->iObjectName           = aMapDesc.iObjectName;
    this->iKeySize              = aMapDesc.iKeySize;                                    // Key size
    this->iKeyAlgorithm         = aMapDesc.iKeyAlgorithm;                               // RSA, DSA
    this->iStartTime            = aMapDesc.iStartTime;
    this->iEndTime              = aMapDesc.iEndTime;
    this->iIsDeletable          = aMapDesc.iIsDeletable;
    TInt i;
    for(i=0;i<aMapDesc.iApplUids.Count();i++)
        {
        this->iApplUids.Append(aMapDesc.iApplUids[i]);
        }
        
    this->iCertStoreType        = aMapDesc.iCertStoreType;
    return *this;
}


TBool CMapDescriptor::IsMatchingL(TSecurityObjectDescriptor &aDesc, 
                                  const TBool aInfoOnly, 
                                  TPkiServiceStoreType aCertStoreType) const
    {
    TBool match(EFalse);

    LOG(Log::Printf(_L("Matching")));
    LOG_1(" Pure informational: %d", aInfoOnly);

    LOG(Log::Printf(_L("Matching: certificate %S"), &iObjectName));
    for(;;)
        {
        if (aDesc.iOwnerType != EPKICACertificate &&
            aCertStoreType != EPkiStoreTypeAny) 
            {            
            if (iCertStoreType != aCertStoreType)
                {
                LOG(Log::Printf(_L("    Store doesn't match, aborting")));
                match = EFalse;
                break;                
                }
            }
        else 
            {
            LOG(Log::Printf(_L("Skipping store check, not relevant")));
            }
                    
			
        if (aDesc.iSubjectKeyIdUsed)
            {            
			if(iSubjectKeyId == aDesc.iSubjectKeyId)
				{
				match = ETrue;
				}
			else
				{
				match = EFalse;
				break;
				}
			}
			
        if(aDesc.iTrustedAuthorityUsed)
            {
            if(iTrustedAuthority == NULL)
                {
                match = EFalse;
                break;
                }
            else
                {
                CX500DistinguishedName* dnSuffix1 = CX500DistinguishedName::NewLC(*iTrustedAuthority);
                CX500DistinguishedName* dnSuffix2;
                TInt popCount = 3;

                // ASN1 or plain text
                if((aDesc.iTrustedAuthority[0] != 0x30)
                   || ((aDesc.iTrustedAuthority[1] != 0x81)
                   && (aDesc.iTrustedAuthority[1] != 0x82)
                   && ((aDesc.iTrustedAuthority[1] + 2) != aDesc.iTrustedAuthority.Length())))
                    {
                    HBufC8* name2Der;
                    CPkcs10Req::BuildDistinguishedNameDerFromTextL(name2Der,
                                                                   aDesc.iTrustedAuthority,
                                                                   EFalse, KNullDesC8);
                    CleanupStack::PushL(name2Der);

                    dnSuffix2 = CX500DistinguishedName::NewLC(*name2Der);
                    }
                else
                    {
                    dnSuffix2 = CX500DistinguishedName::NewLC(aDesc.iTrustedAuthority);
                    popCount = 2;
                    }


                if(PkiUtil::MatchL(*dnSuffix1, *dnSuffix2))
                    {
                    match = ETrue;
                    CleanupStack::PopAndDestroy(popCount);
                    }
                else
                    {
                    match = EFalse;
                    CleanupStack::PopAndDestroy(popCount);
                    break;
                    }
                }
            }
        if(aDesc.iOwnerTypeUsed)
            {
            if(iOwnerType == aDesc.iOwnerType)
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }
        if(aDesc.iSerialNumberUsed)
            {
            if ((iSerialNumber != NULL) && ((*iSerialNumber).Compare(aDesc.iSerialNumber) == 0))
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }

        if(aDesc.iIdentitySubjectNameUsed)
            {
            if(iIdentitySubjectName == NULL)
                {
                match = EFalse;
                break;
                }
            else
                {
                CX500DistinguishedName* dnSuffix1 = CX500DistinguishedName::NewLC(*iIdentitySubjectName);
                CX500DistinguishedName* dnSuffix2;
                TInt popCount = 3;
                // ASN1 or plain text
                if((aDesc.iIdentitySubjectName[0] != 0x30)
                   || ((aDesc.iIdentitySubjectName[1] != 0x81)
                   && (aDesc.iIdentitySubjectName[1] != 0x82)
                   && ((aDesc.iIdentitySubjectName[1] + 2) != aDesc.iIdentitySubjectName.Length())))
                    {
                    HBufC8* name2Der;
                    CPkcs10Req::BuildDistinguishedNameDerFromTextL(name2Der,
                                                                   aDesc.iIdentitySubjectName,
                                                                   EFalse, KNullDesC8);
                    CleanupStack::PushL(name2Der);

                    dnSuffix2 = CX500DistinguishedName::NewLC(*name2Der);
                    }
                else
                    {
                    dnSuffix2 = CX500DistinguishedName::NewLC(aDesc.iIdentitySubjectName);
                    popCount = 2;
                    }

                if(PkiUtil::MatchL(*dnSuffix1, *dnSuffix2))
                    {
                    CleanupStack::PopAndDestroy(popCount);
                    match = ETrue;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(popCount);
                    match = EFalse;
                    break;
                    }
                }
            }

        if(aDesc.iIdentityRfc822NameUsed)
            {
            if(iIdentityRfc822Name == NULL)
                {
                match = EFalse;
                break;
                }
            else
                {
                TInt bytes = aDesc.iIdentityRfc822Name.Length();
                TPtrC8 tail = (*iIdentityRfc822Name).Right(bytes);
                if (tail.CompareF(aDesc.iIdentityRfc822Name) == 0)
                    {
                    match = ETrue;
                    }
                else
                    {
                    match = EFalse;
                    break;
                    }
                }
            }

        if(aDesc.iKeyUsageUsed)
            {
            CX509KeyUsageExt* tempUsage = NULL;
            if(iKeyUsageDer.Length() != 0)
                {
                tempUsage = CX509KeyUsageExt::NewL(iKeyUsageDer);
                }
            if((tempUsage == NULL) || tempUsage->IsSet(aDesc.iKeyUsage))       
                {
                delete tempUsage;
                match = ETrue;
                }
            else
                {
                delete tempUsage;
                match = EFalse;
                break;
                }
            }
            
        if(aDesc.iKeySizeUsed)
            {
            if(iKeySize == aDesc.iKeySize)
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }
            
        if (match && !aInfoOnly)
            {            
            TValidity  val = CPKIMapper::CertValidity(iStartTime, iEndTime);
            // Treat future certificates as valid
            if((val == EValid) || (val == ENotValidYet))
                {
                match = ETrue;
                }
            else
                {
                LOG_("Matching: Expired, and not an informational request");
                match = EFalse;
                break;
                }
            }
            
        break;
        }
		
    return match;
    }

TBool CMapDescriptor::IsEqual(CMapDescriptor &aDesc)
    {
    TBool match = EFalse;

    for(;;)
        {
        if((iTrustedAuthority != NULL) && (aDesc.iTrustedAuthority != NULL) && (iTrustedAuthority->Des().Compare(*aDesc.iTrustedAuthority) == 0))
            {
            match = ETrue;
            }
        else
            {
            match = EFalse;
            break;
            }
        
        if((iSerialNumber != NULL) && (aDesc.iSerialNumber != NULL) && (iSerialNumber->Des().Compare(*aDesc.iSerialNumber) == 0))
            {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            
        break;
        }

    return match;
    }
    
    
void CMapDescriptor::SetCertStoreType(TPkiServiceStoreType aCertStoreType)
    {
    iCertStoreType = aCertStoreType;
    }

CPKIMapper* CPKIMapper::NewL()
    {
    CPKIMapper* self = new (ELeave) CPKIMapper();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPKIMapper::~CPKIMapper()
    {
    if ( iMapping )
        {
        iMapping->ResetAndDestroy();
        delete iMapping;
        }
    }


CPKIMapper::CPKIMapper()
    {
    }

void CPKIMapper::ConstructL()
    {
    LOG_("CPKIMapper::ConstructL");
    // Allocate mapping vector. Approx granularity 20 
    iMapping = new (ELeave) RPointerArray<CMapDescriptor>(20);
    }

void CPKIMapper::SetCacheCreated()
    {
    iCacheCreated = ETrue;
    }
TBool CPKIMapper::CacheCreated()
    {
    return iCacheCreated;
    }
    
TInt CPKIMapper::AddMapping(CMapDescriptor& aMap)
    {
    // Insert as first element, so that newest object will be found first
    return iMapping->Insert(&aMap, 0);
    }

void CPKIMapper::LogMap(CMapDescriptor &aDescriptor) const 
    {
    TInt len = aDescriptor.iSubjectKeyId.Length();
    LOG(Log::Printf(_L("====CertificateObject====\n")));
    LOG(Log::Printf(_L("ObjectName: %S\n"), &(aDescriptor.iObjectName)));
    LOG(Log::Printf(_L("KeyId:")));
    LOG(Log::HexDump(NULL, NULL, aDescriptor.iSubjectKeyId.Ptr(), len));
    }

    
void CPKIMapper::DeleteMapping(TInt aIndex)
    {
    // Delete mapping at index
    LOG(Log::Printf(_L("Delete mapping entry")));
    LogMap(*(*iMapping)[aIndex]);
    delete (*iMapping)[aIndex]; 
    iMapping->Remove(aIndex);
    iMapping->Compress();
    }

TInt CPKIMapper::DeleteMapping(CMapDescriptor &aDesc)
    {
    TInt status(KErrNotFound);
    
    for(TInt i(0); i < iMapping->Count(); i++)
        {
        if((*iMapping)[i]->IsEqual(aDesc))
            {
            DeleteMapping(i);
            status = KErrNone;
            break;
            }
        }
    return status;
    }

TInt CPKIMapper::CertCount()
    {
    TInt count(0);    
    
    for(TInt i(0); i < iMapping->Count(); i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        count++;
        if (CertValidity(mapping->iStartTime, mapping->iEndTime) == EExpired)
            {
            // do not count expider certificates
            count--;
            }                
        }
    iCount = count;
    return count;            
    }

TInt CPKIMapper::ApplicableCertCount(const RArray<TUid>& aUidArray)
{
    TInt count(0);
    TBool uidMatch(EFalse);
    
    for(TInt i(0); i < iMapping->Count(); i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        if(mapping->iOwnerType == EPKICACertificate)
            {
            if(CertValidity(mapping->iStartTime, mapping->iEndTime) != EExpired)
                {
                uidMatch = EFalse;
                for(TInt j = 0; j < mapping->iApplUids.Count(); j++)
                    {
                    for(TInt k = 0; k < aUidArray.Count(); k++)
                        {
                        if(mapping->iApplUids[j].iUid == aUidArray[k].iUid)
                            {
                            uidMatch = ETrue;
                            break;
                            }
                        }
                    if(uidMatch)
                        {
                        count++;
                        break;
                        }
                    }
                }
            }
        }
    iCount = count;
    return count;
}


TInt CPKIMapper::GetCertListL(const RMessage2 &aMessage, TPkiServiceStoreType aStoreType, 
                              TBool aDescUsed)
{
    TInt pos = 0;
    TInt iLast = 0;
    TInt iFirst = 0;

    if(aDescUsed)
        {
        aMessage.ReadL(1, iCurrentDescriptor);
        TSecurityObjectDescriptor& secDesc = iCurrentDescriptor();
        TInt status = ResolveCertMappingL(
            iCurrentDescriptor(), iObjectName, iFirst, EFalse, aStoreType);
        if(status == KErrNone)
            {
            iCount = 1;
            iLast = iFirst + 1;
            }
        }
    else
        {
        iLast = iMapping->Count();
        }
    
    CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateListEntry));
    CleanupStack::PushL(list);
    list->ResizeL(iCount * sizeof(TCertificateListEntry));
    TCertificateListEntry certInfo;
    
    for(TInt i = iFirst; i < iLast; i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        if(CertValidity(mapping->iStartTime, mapping->iEndTime) != EExpired)
            {
            certInfo.iObjectName = mapping->iObjectName;
            certInfo.iOwnerType = mapping->iOwnerType;
            if(mapping->iTrustedAuthority != NULL)
                {
                certInfo.iTrustedAuthority = *(mapping->iTrustedAuthority);
                }
            if(mapping->iIdentitySubjectName != NULL)
                {
                certInfo.iIdentitySubjectName = *(mapping->iIdentitySubjectName);
                }
            if(mapping->iSerialNumber != NULL)
                {
                certInfo.iSerialNumber = *(mapping->iSerialNumber);
                }
            certInfo.iSubjectKeyId = mapping->iSubjectKeyId;
            certInfo.iKeySize = mapping->iKeySize;                   // Key size
            certInfo.iKeyAlgorithm = mapping->iKeyAlgorithm;         // RSA, DSA
            certInfo.iIsDeletable = mapping->iIsDeletable;           // IsDeletable

            list->Write(pos * sizeof(TCertificateListEntry),
                        (TAny*)&certInfo,
                        sizeof(TCertificateListEntry));
            pos++;
            if(pos >= iCount)
                {
                break;
                }
            }            
        }
    TPtr8 ptrList = list->Ptr(0);
    aMessage.WriteL(0, ptrList);

    CleanupStack::PopAndDestroy(1); // list
    return KErrNone;
}


void CPKIMapper::GetApplicableCertListL(const RMessage2& aMessage, const RArray<TUid>& aUidArray)
{
    TBool uidMatch = EFalse;
    TInt pos(0);
    
    CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateListEntry));
    CleanupStack::PushL(list);
    list->ResizeL(iCount * sizeof(TCertificateListEntry));
    TCertificateListEntry certInfo;

    for(TInt i = 0; (i < iMapping->Count()) && (pos < iCount); i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        if(mapping->iOwnerType == EPKICACertificate)
            {
            if(CertValidity(mapping->iStartTime, mapping->iEndTime) != EExpired)
                {
                uidMatch = EFalse;
                for(TInt j = 0; j < mapping->iApplUids.Count(); j++)
                    {
                    for(TInt k = 0; k < aUidArray.Count(); k++)
                        {
                        if(mapping->iApplUids[j].iUid == aUidArray[k].iUid)
                            {
                            uidMatch = ETrue;
                            break;
                            }
                        }
                    if(uidMatch)
                        {
                        certInfo.iObjectName = mapping->iObjectName;
                        certInfo.iOwnerType = mapping->iOwnerType;
                        if(mapping->iTrustedAuthority != NULL)
                            {
                            certInfo.iTrustedAuthority = *(mapping->iTrustedAuthority);
                            }
                        if(mapping->iIdentitySubjectName != NULL)
                            {
                            certInfo.iIdentitySubjectName = *(mapping->iIdentitySubjectName);
                            }
                        if(mapping->iSerialNumber != NULL)
                            {
                            certInfo.iSerialNumber = *(mapping->iSerialNumber);
                            }
                        certInfo.iSubjectKeyId = mapping->iSubjectKeyId;
                        certInfo.iKeySize = mapping->iKeySize;                   // Key size
                        certInfo.iKeyAlgorithm = mapping->iKeyAlgorithm;         // RSA, DSA
                        certInfo.iIsDeletable = mapping->iIsDeletable;           // IsDeletable

                        list->Write(pos * sizeof(TCertificateListEntry),
                                    (TAny*)&certInfo,
                                    sizeof(TCertificateListEntry));
                        pos++;
                        break;
                        }
                    }
                }
            }
        }
    TPtr8 ptrList = list->Ptr(0);
    aMessage.WriteL(0, ptrList);

    CleanupStack::PopAndDestroy(1); // list
}



CMapDescriptor& CPKIMapper::GetMapDescriptorAtIndex(TInt aIndex)
{
    PKISERVICE_ASSERT(aIndex < iMapping->Count());
    return *(*iMapping)[aIndex];
}


void CPKIMapper::GetCertificateKeyIdL(TSecurityObjectDescriptor &aDescriptor, TPKIKeyIdentifier &aKeyId,
                                      TPkiServiceStoreType aStoreType) const
    {
    LOG(Log::Printf(_L("-> CPKIMapper::GetCertificateKeyIdL"), aStoreType));
    aKeyId.Zero();
    
    TInt index;    
    
    TFileName* fileName = new (ELeave) TFileName;
    CleanupDeletePushL(fileName);
    fileName->Zero();
    
    TInt status = ResolveCertMappingL(aDescriptor, *fileName, index, EFalse, aStoreType);    

    CleanupStack::PopAndDestroy(fileName);
    
    
    if(status == KErrNone)
        {
        aKeyId.Copy((*iMapping)[index]->iSubjectKeyId);
        }
    else
        {
        LOG(Log::Printf(_L("ResolveKeyMapping: key NOT found\n")));
        LogSearchArguments(aDescriptor);
        }    
    LOG(Log::Printf(_L("<- CPKIMapper::GetCertificateKeyIdL"), aStoreType));        
    }

/**
    Check whether the given label is unique among all VPN certs.
    @return True only if the given label doesn't exist
*/
TBool CPKIMapper::LabelIsUnique(const TDesC& aLabel) const
    {
    for (TInt i(0); i < iMapping->Count(); i++) 
        {
        if ((*iMapping)[i]->iObjectName.Compare(aLabel) == 0) 
            {
            return EFalse;
            }
        }
    return ETrue;
    }

/**
    Check whether the given certificate already exists among VPN certs.
    @return True only if the given certificate does not exist in VPN's cert list.
*/
TBool CPKIMapper::CertificateIsUniqueL(const TDesC8& aCertData)
    {
    TBool ret(ETrue);
    
    LOG(Log::Printf(_L("Verifying the uniqueness of certificate:")));

    CX509Certificate* certificate = CX509Certificate::NewLC(aCertData);
    const TPtrC8* issuername = certificate->DataElementEncoding(CX509Certificate::EIssuerName);
    const TPtrC8* subjectname = certificate->DataElementEncoding(CX509Certificate::ESubjectName);
    
    HBufC* issuerdispname = certificate->IssuerName().DisplayNameL();
    CleanupStack::PushL(issuerdispname);
    HBufC* subjectdispname = certificate->SubjectName().DisplayNameL();
    CleanupStack::PushL(subjectdispname);
    
    TInt count(iMapping->Count());

    for (TInt i(0); i < count; i++) 
        {
        CMapDescriptor* mapitem = (*iMapping)[i];

        //  Use subject name for uniqueness criterion
        if (mapitem->iIdentitySubjectName)
            {
            if (subjectname->Compare(*(mapitem->iIdentitySubjectName)) == 0) 
                {
                LOG(Log::Printf(_L("Found an existing cert that matches subject")));
                if (issuername->Compare(*(mapitem->iTrustedAuthority)) == 0) 
                    {
                    ret = EFalse;
                    break;
                    }
                }
            }
        else if (mapitem->iTrustedAuthority && (subjectname->Compare(*issuername) == 0))
            {
            if (subjectname->Compare(*(mapitem->iTrustedAuthority)) == 0) 
                {
                LOG(Log::Printf(_L("Found an existing cert that matches subject (CA)")));
                ret = EFalse;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy(3, certificate); // issuerdispname, subjectdispname

    return ret;
    }
    
/**
    Generates an unique label name for a certificate, based on its subject name.
    @param1 The binary data describing the certificate
    @param2 Return value for the generated unique name
    @param3 Certificate owner type -- currently not supported.
    @return none
*/
void CPKIMapper::GenerateUniqueNameL(
    const TDesC8 &aCertData, TDes& aName, 
    TCertificateOwnerType /*aOwnerType*/ )
    {
    LOG(Log::Printf(_L("CPKIMapper::GenerateUniqueNameL() entry")));

    CX509Certificate* certificate = CX509Certificate::NewLC(aCertData);

    HBufC* baseline = certificate->SubjectName().DisplayNameL();
    CleanupStack::PushL(baseline);
    // +5 for (999) suffix
    HBufC* variation = HBufC::NewLC(baseline->Length() + 5);
    variation->Des().Append(*baseline);
    
    if (baseline->Length() == 0)
        {
        TPtr ptr = variation->Des();
        ptr.Format(_L("(1)"));
        }

    // See whether the initial label is already unique    
    TInt iter(2);
    while (!LabelIsUnique(*variation)) 
        {
        // Iterate a new version of the label
        if (iter > 999) 
            {
            // too long name, just go with the previous.
            break;
            }
        TPtr ptr = variation->Des();
        ptr.Format(_L("%S(%d)"), &(*baseline), iter);
        iter++;
        }

    // Sanity check for string lengths
    aName = variation->Left(MAX_FILENAME_LENGTH);

    CleanupStack::PopAndDestroy(3); //variation, baseline, certificate

    LOG(Log::Printf(_L("CPKIMapper::GenerateUniqueNameL() exit")));
    }

TInt CPKIMapper::ResolveCertMappingL(TSecurityObjectDescriptor &aDescriptor, TDes16 &aFilename, 
                                     TInt &aIndex, const TBool aInfoOnly, 
                                     TPkiServiceStoreType aStoreType) const   
    {
    TInt i;
    TInt status = KErrNotFound;
    TInt foundIndex = -1;
    TTime furthestEndTime = TTime(0);

    LOG(Log::Printf(_L("Resolving cert mapping, STORETYPE: %d\n"), aStoreType));

    LOG(LogSearchArguments(aDescriptor));

    // Scan available mappings
    for(i = 0; i < iMapping->Count(); i++)
        {
        // Bypass entry in case that function leaves
        // (issuer/subject name can be invalid)
        CMapDescriptor* mapping = (*iMapping)[i];
        if(mapping->IsMatchingL(aDescriptor, aInfoOnly, aStoreType))
            {
            // If we found a match, process it further
            _LIT(KMidp2Label, "MIDP2");
            TBuf<12> buf;
            buf.Append(KMidp2Label);
            // Discard all MIDP2 certificates to avoid label-mapping problem
            if (buf.Compare(mapping->iObjectName) != 0)
                {
                if(mapping->iEndTime > furthestEndTime)
                    {
                    furthestEndTime = mapping->iEndTime;
                    foundIndex = i;
                    LOG(Log::Printf(_L("Resolve cert mapping: Tentatively found a suitable one")));
                    // Continue to search the longest lasting certificate
                    }
                }
            else 
                {
                LOG(Log::Printf(_L("Found a cert, but it was a MIDP2 one - continuing search")));
                }
            }
        }
    
    if(foundIndex == -1)
        {
        LOG(Log::Printf(_L("Resolve cert mapping: No matching certificate found")));       
        status = KErrNotFound;
        }
    else
        {
        aFilename.Zero();
        status = KErrNone;
        if(status == KErrNone)
            {
            aFilename.Append((*iMapping)[foundIndex]->iObjectName);
            status = KErrNone;
            aIndex = foundIndex;
            }
        LOG(Log::Printf(_L("ResolveCertMapping: certificate found\n")));
        LogSearchArguments(aDescriptor);
        LOG(Log::Printf(_L("ResolveCertMapping: Object info\n")));
        LogMap(*(*iMapping)[aIndex]);
        }
    return status;
    }    
  

TValidity CPKIMapper::CertValidity(const TTime &aStartTime,  const TTime &aEndTime) 
    {
    TTimeIntervalSeconds tolerance(120); 
    TTime currentTime;
    currentTime.UniversalTime();

    if (aEndTime <= currentTime)
        {
        return EExpired;
        }

    if (aStartTime >= currentTime + tolerance)
        {
        return ENotValidYet;
        }

    return EValid;
    }

void CPKIMapper::LogSearchArguments(TSecurityObjectDescriptor &aDescriptor) const
    {
    TBuf<256> temp;
    LOG(Log::Printf(_L("====Object Search arguments====\n")));
    if(aDescriptor.iTrustedAuthorityUsed)
        {
        temp.Copy(aDescriptor.iTrustedAuthority);
        if((aDescriptor.iTrustedAuthority[0] != 0x30)
           || ((aDescriptor.iTrustedAuthority[1] != 0x81)
               && (aDescriptor.iTrustedAuthority[1] != 0x82)
               && ((aDescriptor.iTrustedAuthority[1] + 2) != aDescriptor.iTrustedAuthority.Length())))
            {
            LOG(Log::Printf(_L("Trusted authority: %S\n"), &temp));
            }
        else
            {
            LOG(Log::Printf(_L("Trusted authority:")));
            LOG(Log::HexDump(NULL, NULL, aDescriptor.iTrustedAuthority.Ptr(), aDescriptor.iTrustedAuthority.Length()));
            }
        }
    if(aDescriptor.iIdentitySubjectNameUsed)
        {
        temp.Copy(aDescriptor.iIdentitySubjectName);
        if((aDescriptor.iIdentitySubjectName[0] != 0x30)
           || ((aDescriptor.iIdentitySubjectName[1] != 0x81)
               && (aDescriptor.iIdentitySubjectName[1] != 0x82)
               && ((aDescriptor.iIdentitySubjectName[1] + 2) != aDescriptor.iIdentitySubjectName.Length())))
            {
            LOG(Log::Printf(_L("SubjectName: %S\n"), &temp));
            }
        else
            {
            LOG(Log::Printf(_L("SubjectName:")));
            LOG(Log::HexDump(NULL, NULL, aDescriptor.iIdentitySubjectName.Ptr(), aDescriptor.iIdentitySubjectName.Length()));
            }
        }
    if(aDescriptor.iIdentityRfc822NameUsed)
        {
        temp.Copy(aDescriptor.iIdentityRfc822Name);
        LOG(Log::Printf(_L("Rfc822Name: %S\n"), &temp));
        }
    if(aDescriptor.iSerialNumberUsed)
        {
        LOG(Log::Printf(_L("SerialNumber:")));
        LOG(Log::HexDump(NULL, NULL, aDescriptor.iSerialNumber.Ptr(), aDescriptor.iSerialNumber.Length()));
        }
    if(aDescriptor.iSubjectKeyIdUsed)
        {
        LOG(Log::Printf(_L("KeyId:")));
        LOG(Log::HexDump(NULL, NULL, aDescriptor.iSubjectKeyId.Ptr(), aDescriptor.iSubjectKeyId.Length()));
        }
    if(aDescriptor.iOwnerTypeUsed)
        {
        LOG(Log::Printf(_L("OwnerType: %d\n"), aDescriptor.iOwnerType));
        }
    if(aDescriptor.iKeyUsageUsed)
        {
        LOG(Log::Printf(_L("KeyUsage:")));
        LOG(Log::HexDump(NULL, NULL, aDescriptor.iKeyUsageDer.Ptr(), aDescriptor.iKeyUsageDer.Length()));
        }
    if(aDescriptor.iKeySizeUsed)
        {
        LOG(Log::Printf(_L("KeySize: %d\n"), aDescriptor.iKeySize));
        }
    if(aDescriptor.iKeyAlgorithmUsed)
        {
        LOG(Log::Printf(_L("KeyAlgorithm: %d\n"), aDescriptor.iKeyAlgorithm));
        }
    }   

TBool PkiUtil::MatchL(const CX500DistinguishedName& aDn1, const CX500DistinguishedName& aDn2)
{
    if((aDn1.Count() == 0) || (aDn2.Count() == 0))
        {
        return EFalse;
        }
    
    if (aDn1.Count() < aDn2.Count())
        {
        return EFalse;
        }
    else
        {
        // For each field in aDn2, aDn1 must contain a field with the same value
        for (TInt i = 0; i < aDn2.Count(); i++)
            {
            if (!HasElementL(aDn1, aDn2.Element(i)))
                {
                return EFalse;
                }
            }
        }

    return ETrue;
}

TBool PkiUtil::HasElementL(const CX500DistinguishedName& aDn, const CX520AttributeTypeAndValue& aElement)
{
    for (TInt i = 0; i < aDn.Count(); i++)
        {
        if (aElement.ExactMatchL(aDn.Element(i)))
            {
            return ETrue;
            }
        }
    return EFalse;
}

