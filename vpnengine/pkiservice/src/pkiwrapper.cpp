/*
* Copyright (c) 2006 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* A class wrapping symbian certificate storage for use by PKI service clients.
*
*/



#include <x509cert.h>

#include "pkiwrapper.h"
#include "PKIMapper.h"
#include "pkisession.h"
#include "pkisupport.h"
#include "log_r6.h"



CPKIWrapper* CPKIWrapper::NewL(CPKIMapper& aMapper)
    {
    CPKIWrapper* self = new (ELeave) CPKIWrapper(aMapper);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CPKIWrapper::~CPKIWrapper()
    {
    LOG_("-> CPKIWrapper::~CPKIWrapper()");
    Cancel();
    delete iCurrentMapping;
    delete iObjectName;
    delete iCertBuffer;
    iUidArray.Close();
    
    delete iPKISupport;    
    LOG_("<- CPKIWrapper::~CPKIWrapper()");
    }

CPKIWrapper::CPKIWrapper(CPKIMapper& aMapper)
    :CActive(CActive::EPriorityStandard), 
    iMapper(aMapper),
    iPtrCertBuffer(0,0)
    {
    CActiveScheduler::Add(this);
    }

void CPKIWrapper::ConstructL()
    {
    LOG_("-> CPKIWrapper::ConstructL()");   
    iObjectName = new (ELeave) TBuf<MAX_FILENAME_LENGTH>;
    iPKISupport = CPKISupport::NewL(iMapper, *this);

    SetCertStoreType(EPkiStoreTypeAny);
    SetInformational(EFalse);

    LOG_("<- CPKIWrapper::ConstructL()");
    }

//====================================================================================================================================    
/**
*   Initialize
*/
//====================================================================================================================================    
void CPKIWrapper::InitializeL(const RMessage2& aMessage)
{
    LOG_("CPKIWrapper::InitializeL()");    
    iPKISupport->StartInitializeL(aMessage);    
}
    

//====================================================================================================================================    
/**
*   Initialization routine
*/
//====================================================================================================================================    
TInt CPKIWrapper::InitOperation(const RMessage2& aMessage)
{
    if (IsActive())
        {
        LOG(Log::Printf(_L("Pkiservice busy. Function %d\n"), aMessage.Function()));
        return KPKIErrServiceBusy;
        }
    
    LOG(Log::Printf(_L("InitOperation function %d\n"), aMessage.Function()));
    iCurrentStatus = KErrNone;              // Clear status
    iCurrentState = EExecute;               // Set state
    iMessage = aMessage;                    // Save message
    iPKISupport->SetCurrentFunction(iMessage.Function());
    iIndex = KErrNotFound;
    
    // Trigger function
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
    
    return KErrNone;
}
    
/**---------------------------------------------------------
 *
 * CancelPendingOperation
 *
 *----------------------------------------------------------*/
void CPKIWrapper::CancelPendingOperation()
{
    Cancel();
    
    delete iCertBuffer;
    iCertBuffer = NULL;
}

/**---------------------------------------------------------
 *
 * GetRequiredBufferSize
 *
 *----------------------------------------------------------*/
TInt CPKIWrapper::GetRequiredBufferSizeL(const RMessage2& aMessage)
{
    iCurrentStatus = KErrNone;                      // Clear status
    LOG(Log::Printf(_L("Start Function %d\n"), iMessage.Function()));
    iMessage = aMessage;                            // Save message
    CompleteRequestAndCleanupL();
    return iCurrentStatus;
}

//====================================================================================================================================    
/**
*   Starter routines
*/
//====================================================================================================================================    

void CPKIWrapper::SetTrusted(TBool aValue)
{
    iTrusted = aValue;
}

void CPKIWrapper::SetApplications(const RArray<TUid>& aApplUids)
{
    iUidArray.Close();
	for(TInt i=0;i<aApplUids.Count();i++)
		{
		iUidArray.Append(aApplUids[i]);
		}
}


/**---------------------------------------------------------
 *
 * StartReadCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartReadCertificateL()
    {
    LOG(Log::Printf(_L("StartReadCertificate\n")));
    iMessage.ReadL(0, iCurrentDescriptor);
    TInt outDataSize = iMessage.GetDesMaxLength(1);
    iCertBuffer = HBufC8::NewL(outDataSize);
    iPtrCertBuffer.Set(iCertBuffer->Des()); // A pointer for changing the contents of buf
    }



/**---------------------------------------------------------
 *
 * StartStoreCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartStoreCertificateL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    iObjectName->Copy(iCurrentDescriptor().iObjectName);
    TInt inDataSize = iMessage.GetDesLength(1);
    iCertBuffer = HBufC8::NewL(inDataSize);
    iPtrCertBuffer.Set(iCertBuffer->Des()); // A pointer for changing the contents of buf
    iMessage.ReadL(1, iPtrCertBuffer); 
    }

/**---------------------------------------------------------
 *
 * StartAttachCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartAttachCertificateL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    iObjectName->Copy(iCurrentDescriptor().iObjectName);

    TInt inDataSize = iMessage.GetDesLength(1);
    iCertBuffer = HBufC8::NewL(inDataSize);
    iPtrCertBuffer.Set(iCertBuffer->Des()); // A pointer for changing the contents of buf
    iMessage.ReadL(1, iPtrCertBuffer); 
    }


/**---------------------------------------------------------
 *
 * StartRemoveCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartRemoveCertificateL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    }

/**---------------------------------------------------------
 *
 * StartSetTrustL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartSetTrustL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    TPckg<TBool> pckgTrusted(iTrusted);
    iMessage.ReadL(1, pckgTrusted, 0);
    }

/**---------------------------------------------------------
 *
 * StartTrustedL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartTrustedL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    }


/**---------------------------------------------------------
 *
 * StartSetApplicabilityL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartSetApplicabilityL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    TPckg<TUint> pckgCount(iCount);
    iMessage.ReadL(1, pckgCount);
    // Allocate list for applications
    CBufFlat* list = CBufFlat::NewL(sizeof(TUid));
    CleanupStack::PushL(list);
    list->ResizeL(iCount * sizeof(TUid));
    TPtr8 ptrList = list->Ptr(0);
    iMessage.ReadL(2, ptrList);
    iUidArray.Close();
    if(iCount > 0)
        {
        TUid tempUid;
        for (TUint i = 0; i < iCount; i++)
            {
            list->Read(i * sizeof(TUid), (TAny*)&tempUid, sizeof(TUid));
            iUidArray.Append(tempUid);
            }
        }
    CleanupStack::PopAndDestroy(1);     // list
    }

/**---------------------------------------------------------
 *
 * StartApplicationsL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::StartApplicationsL()
    {
    iMessage.ReadL(0, iCurrentDescriptor);
    TPckg<TUint> pckgCount(iMaxCount);
    iMessage.ReadL(1, pckgCount);
    }

    
//====================================================================================================================================    
/**
*   Continuation routines
*   Activate active object if necessary
*/
//====================================================================================================================================        
    

/**---------------------------------------------------------
 *
 * ExecuteReadCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteReadCertificateL()
    {
    TInt index;

    LOG(Log::Printf(_L("CPKIWrapper::ExecuteReadCertificateL()\n")));
    iCurrentStatus = iMapper.ResolveCertMappingL(iCurrentDescriptor(), *iObjectName, 
                                                           index, iInfoOnly, 
                                                           iPKISupport->CertStoreType());
    if(iCurrentStatus == KErrNone)
        {
        TSecurityObjectDescriptor sdesc = iCurrentDescriptor();
        iCurrentState = EComplete;
        iPKISupport->RetrieveCertificateL(*iObjectName, iPtrCertBuffer, sdesc.iOwnerType, iStatus);
        SetActive();
        }
    LOG_1("CPKIWrapper::ExecuteReadCertificateL() exit:%d", iCurrentStatus);
    }


/**---------------------------------------------------------
 *
 * ExecuteStoreCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteStoreCertificateL()
    {
    LOG(Log::Printf(_L("CPKIWrapper::ExecuteStoreCertificateL()\n")));
    
    if (iPtrCertBuffer.Length() > 0) 
        {
        // See whether the certificate already exists in the store
        TBool certIsNew = iMapper.CertificateIsUniqueL(
            iPtrCertBuffer);

        if (certIsNew)
            {
            LOG(Log::Printf(_L("Creating new certificate entry\n")));
            delete iCurrentMapping;
            iCurrentMapping = NULL;
            iCurrentMapping = new (ELeave) CMapDescriptor(iCurrentDescriptor());
            iMapper.GenerateUniqueNameL(iPtrCertBuffer, *iObjectName);
            iCurrentMapping->SetMapObjectName(*iObjectName);
            SaveIdentityL(*iCurrentMapping, iPtrCertBuffer, (TCertificateOwnerType)iCurrentDescriptor().iOwnerType);
            
            TPkiServiceStoreType storeType = iPKISupport->CertStoreType();
            if (storeType == EPkiStoreTypeAny)
                {
                storeType = EPkiStoreTypeUser;
                }
            iCurrentMapping->SetCertStoreType(storeType);

            if(iCurrentStatus == KErrNone)
                {
                LOG(Log::Printf(_L("Status OK, identity saved\n")));
                iCurrentStatus = KErrNone;
                iCurrentState = EComplete;
                LOG(Log::Printf(_L("Storing CERT with LABEL:")));
                LOG(Log::Printf(iCurrentMapping->iObjectName));
                iPKISupport->StoreCertificateL(iCurrentMapping->iObjectName, 
                    (TCertificateOwnerType)iCurrentDescriptor().iOwnerType, 
                    iPtrCertBuffer, iCurrentDescriptor().iIsDeletable, iStatus);
                LOG(Log::Printf(_L("CERT stored\n")));
                SetActive();
                }
            }
        else 
            {
            // Identical certificate already existed, no need to add this one.
            // Set current status to KErrBadName so that the operations can
            // continue (any other error would halt the process)
            LOG(Log::Printf(_L("Certificate already exists, not adding")));
            iCurrentStatus = KErrBadName;
            }
        }
    else 
        {
        LOG(Log::Printf(_L("Certificate buffer invalid")));
        iCurrentStatus = KErrGeneral;
        }
    }


/**---------------------------------------------------------
 *
 * ExecuteAttachCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteAttachCertificateL()
    {
    LOG(Log::Printf(_L("ExecuteAttachCertificateL")));

    iMapper.GenerateUniqueNameL(iPtrCertBuffer, *iObjectName, EUserCertificate);

    delete iCurrentMapping;
    iCurrentMapping = NULL;
    iCurrentMapping = new (ELeave) CMapDescriptor(iCurrentDescriptor());
    iCurrentMapping->SetMapObjectName(*iObjectName);
    SaveIdentityL(*iCurrentMapping, iPtrCertBuffer, (TCertificateOwnerType)iCurrentDescriptor().iOwnerType);

    TPkiServiceStoreType storeType = iPKISupport->CertStoreType();
    if (storeType == EPkiStoreTypeAny)
        {
        storeType = EPkiStoreTypeUser;
        }
    iCurrentMapping->SetCertStoreType(storeType);
    if(iCurrentStatus == KErrNone)
        {
        iCurrentStatus = KErrNone;
        iCurrentState = EComplete;
        LOG(Log::Printf(_L("Attaching certificate")));
        LOG(Log::Printf(iCurrentMapping->iObjectName));
        iPKISupport->AttachCertificateL(iCurrentMapping->iObjectName, iCurrentDescriptor().iSubjectKeyId, iPtrCertBuffer, iStatus);
        LOG(Log::Printf(_L("Certificate attached")));
        SetActive();
        }
    }


/**---------------------------------------------------------
 *
 * ExecuteRemoveCertificateL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteRemoveCertificateL()
    {
    TInt index;

    iCurrentStatus = iMapper.ResolveCertMappingL(iCurrentDescriptor(), 
                                        *iObjectName, index, iInfoOnly,
                                        iPKISupport->CertStoreType());
    if(iCurrentStatus == KErrNone)
        {
        delete iCurrentMapping;
        iCurrentMapping = NULL;
        iCurrentMapping = new (ELeave) CMapDescriptor(iCurrentDescriptor());
        *iCurrentMapping = iMapper.GetMapDescriptorAtIndex(index);
        iCurrentState = EComplete;
        iPKISupport->RemoveCertificateL(*iObjectName, iStatus);
        SetActive();
        }
    }

/**---------------------------------------------------------
 *
 * ExecuteSetTrustL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteSetTrustL()
    {
    TInt index;

    iCurrentStatus = iMapper.ResolveCertMappingL(iCurrentDescriptor(), *iObjectName, 
                                                           index, iInfoOnly, 
                                                           iPKISupport->CertStoreType());
    if(iCurrentStatus == KErrNone)
        {
        const CMapDescriptor& mapping = iMapper.GetMapDescriptorAtIndex(index);
        if (mapping.iOwnerType == EPKICACertificate)
            {            
            iCurrentState = EComplete;
            iPKISupport->SetTrustL(*iObjectName, iTrusted, iStatus);
            SetActive();
            }
        else
            {
            iCurrentStatus = KErrArgument;
            }
        }
    }

/**---------------------------------------------------------
 *
 * ExecuteTrustedL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteTrustedL()
    {
    TInt index;

    iCurrentStatus = iMapper.ResolveCertMappingL(iCurrentDescriptor(), *iObjectName, 
                                                            index, iInfoOnly,
                                                            iPKISupport->CertStoreType());
    if(iCurrentStatus == KErrNone)
        {
        iCurrentState = EComplete;
        iPKISupport->TrustedL(*iObjectName, iStatus);
        SetActive();
        }
    }

/**---------------------------------------------------------
 *
 * ExecuteSetApplicabilityL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteSetApplicabilityL()
    {
    TInt index(KErrNotFound);
    iCurrentStatus = iMapper.ResolveCertMappingL(
        iCurrentDescriptor(), *iObjectName, 
        index, iInfoOnly,
        iPKISupport->CertStoreType());
    
	// Save index
	iIndex = index;
    if(iCurrentStatus == KErrNone)
        {
        LOG_1("ExecuteSetApplicabilityL:%d", iIndex);
        iCurrentState = EComplete;
        iPKISupport->SetApplicabilityL(*iObjectName, iUidArray, iStatus);
        SetActive();
        }
    }

/**---------------------------------------------------------
 *
 * ExecuteApplicationsL
 *
 *----------------------------------------------------------*/
void CPKIWrapper::ExecuteApplicationsL()
{
    TInt index;

    iCurrentStatus = iMapper.ResolveCertMappingL(iCurrentDescriptor(), *iObjectName, 
                                                           index, iInfoOnly,
                                                           iPKISupport->CertStoreType());
    iUidArray.Close();
    if(iCurrentStatus == KErrNone)
        {
        iCurrentState = EComplete;
        iPKISupport->ApplicationsL(*iObjectName, iStatus);
        SetActive();
        }
}

    
//====================================================================================================================================    
/*
*   Completion functions
*/    
//====================================================================================================================================    
void CPKIWrapper::CompleteRequestAndCleanupL()
    {
    switch (iMessage.Function())
        {
        case PkiService::EGetRequiredBufferSize:
            if (iCurrentStatus == KErrNone)
                {
                TPckg<TInt> pckgSize(iRequiredBufferLength);
                iMessage.WriteL(0, pckgSize);
                }
            break;
            
        case PkiService::EReadCertificate:
            if (iCurrentStatus == KErrNone)
                {
                iMessage.WriteL(1, iPtrCertBuffer);
                }
            break;
                        
        case PkiService::EStoreCertificate:
            // FALLTROUGH
        case PkiService::EAttachCertificate:
            if (iCurrentStatus == KErrNone)
                {
                User::LeaveIfError( 
                    iMapper.AddMapping(*iCurrentMapping) );
                iCurrentMapping = NULL;    
                }
            if (iCurrentStatus == KErrBadName)
                {
                // Already exists
                iCurrentStatus = KErrNone;
                }
            break;
                        
        case PkiService::ERemoveCertificate:
            if (iCurrentStatus == KErrNone)
                {
                iMapper.DeleteMapping(*iCurrentMapping);
                }
            break;

		case PkiService::ESetApplicability:
			if (iCurrentStatus == KErrNone)
				{
				iMapper.GetMapDescriptorAtIndex(iIndex).iApplUids.Close();
				TUint i;
				for(i=0;i<iCount;i++)
					{
					iMapper.GetMapDescriptorAtIndex(iIndex).iApplUids.Append(iUidArray[i]);
					}
				}
			break;

        case PkiService::ETrusted:
            if (iCurrentStatus == KErrNone)
                {
                TPckgC<TBool> pckgTrusted(iTrusted);
                iMessage.WriteL(1, pckgTrusted);
                }
            break;
            
        case PkiService::EApplications:
            if (iCurrentStatus == KErrNone)
                {
                TUint pos = 0;
                CBufFlat* list = CBufFlat::NewL(sizeof(TUid));
                CleanupStack::PushL(list);
				iCount = iUidArray.Count();
				if(iCount > iMaxCount)
					{
					// Prevent overrun
					iCount = iMaxCount;
					}
                list->ResizeL(iCount * sizeof(TUid));

                for(TUint i = 0; i < iCount; i++)
                    {
                    list->Write(pos * sizeof(TUid),
                                (TAny*)&iUidArray[i].iUid,
                                sizeof(TUid));
                    pos++;
                    }
                
                TPckgC<TUint> pckgCount(iCount);
                iMessage.WriteL(1, pckgCount);
                TPtr8 ptrList = list->Ptr(0);
                iMessage.WriteL(2, ptrList);

                CleanupStack::PopAndDestroy(1); // list
                }
            break;
        }
    
    LOG(Log::Printf(_L("Complete function %d, status %d\n"), 
        iMessage.Function(), iCurrentStatus));

    delete iCertBuffer;
    delete iCurrentMapping;
    iCurrentMapping = NULL;
    iCertBuffer = NULL;
    iMessage.Complete(iCurrentStatus);
    }


    
//====================================================================================================================================    
/*
*   Active object functions
*/    
//====================================================================================================================================    
void CPKIWrapper::RunL()
     {
     TRequestStatus *status = &iStatus;
     iCurrentStatus = iStatus.Int(); // Status from interface active object     
     switch (iCurrentState)
         {
         case EExecute:
             // Begin case EExecute
             if(iCurrentStatus == KErrNone)
                 {
                 switch ( iMessage.Function() )
                     {                             
                     case PkiService::ESetTrust:
                         StartSetTrustL();
                         ExecuteSetTrustL();
                         break;

                     case PkiService::ETrusted:
                         StartTrustedL();
                         ExecuteTrustedL();
                         break;
                         
                     case PkiService::ESetApplicability:
                         StartSetApplicabilityL();
                         ExecuteSetApplicabilityL();
                         break;

                     case PkiService::EApplications:
                         StartApplicationsL();
                         ExecuteApplicationsL();
                         break;

                     case PkiService::EReadCertificate:
                         StartReadCertificateL();
                         ExecuteReadCertificateL();
                         break;

                     case PkiService::EStoreCertificate:
                         StartStoreCertificateL();
                         ExecuteStoreCertificateL();
                         break;

                     case PkiService::EAttachCertificate:
                         StartAttachCertificateL();
                         ExecuteAttachCertificateL();
                         break;

                     case PkiService::ERemoveCertificate:
                         StartRemoveCertificateL();
                         ExecuteRemoveCertificateL();
                         break;
                     default:
                         iCurrentStatus = KPKIErrNotSupported;
                         iCurrentState = EComplete;
                         break;                           
                     }                   
                 }
             if(iCurrentStatus != KErrNone)
                 {
                 // Trigger completion
                 iCurrentState = EComplete;
                 iStatus = KRequestPending;
                 SetActive();
                 User::RequestComplete(status, iCurrentStatus);
                 }
             break;
             // End case EExecute

         case EComplete:
             // Begin case EComplete
             if(iCurrentStatus == KPKIErrBufferTooShort)
                 {
                 iRequiredBufferLength = iPKISupport->GetRequiredBufferSize();
                 }
             if (iMessage.Function() == PkiService::ELogon)
                 {
                 iCurrentStatus = iStatus.Int();
                }
             CompleteRequestAndCleanupL();
             break;
             // End case EComplete
         default:
            LOG_1("CPKIWrapper::RunL unknown State:%d", iCurrentState);
            break;    
         }
     }

TInt CPKIWrapper::RunError(TInt aError)
    {
    LOG(Log::Printf(_L("CPKIWrapper::RunError, Complete function %d, status %d\n"), iMessage.Function(), aError));
    delete iCertBuffer;
    delete iCurrentMapping;
    iCurrentMapping = NULL; 
    iCertBuffer = NULL;
    
    iMessage.Complete(aError);
    return KErrNone;
    }

void CPKIWrapper::DoCancel()
    {
    LOG_1("Cancel function %d", iMessage.Function());
    if ( iPKISupport )
        {
        iPKISupport->Cancel();
        }
    iMessage.Complete(KErrCancel);
    }
      


void CPKIWrapper::SaveIdentityL(CMapDescriptor &aCertDesc, 
                                const TDesC8& aCertDataIn,
                                TCertificateOwnerType aOwner)
{
    LOG(Log::Printf(_L("CPKIWrapper::SaveIdentityL()\n")));

    CX509Certificate* certificate = CX509Certificate::NewLC(aCertDataIn);            

    // Validity period
    aCertDesc.SetMapStartTime(certificate->ValidityPeriod().Start());
    aCertDesc.SetMapEndTime(certificate->ValidityPeriod().Finish());    
    
    
    // Copy issuer
    const TPtrC8* issuer = certificate->DataElementEncoding(CX509Certificate::EIssuerName);
    aCertDesc.SetMapTrustedAuthorityL(*issuer);

    // Copy subject name
    const TPtrC8* subject = certificate->DataElementEncoding(CX509Certificate::ESubjectName);    
    aCertDesc.SetMapIdentitySubjectNameL(*subject);

    // Copy rfc822 name from subjectAlt name    
    const CX509CertExtension* subjAltName = certificate->Extension(KSubjectAltName);
    if(subjAltName != NULL)
        {
        CX509AltNameExt* subjectAlt = CX509AltNameExt::NewLC(subjAltName->Data());
        if(subjectAlt != NULL)
            {
            const CArrayPtrFlat<CX509GeneralName> *nameArray; 
            nameArray = &subjectAlt->AltName();
            // Search rfc822
            for(TInt i = 0; i < nameArray->Count(); i++)
                {
                if(nameArray->At(i)->Tag() == EX509RFC822Name)
                    {
                    TPtrC8 data = nameArray->At(i)->Data();
                    aCertDesc.SetMapIdentityRfc822NameL(data.Right(data.Length() - 2));
                    break;
                    }
                }                            
            }
        CleanupStack::PopAndDestroy(subjectAlt);
        }

    // Key usage
    const CX509CertExtension* keyUsage = certificate->Extension(KKeyUsage);
    if((keyUsage != NULL) && keyUsage->Critical())
        {
        aCertDesc.iKeyUsageDer.Copy(keyUsage->Data());        
        }

    // Serial number
	const TPtrC8* serial = certificate->DataElementEncoding(CX509Certificate::ESerialNumber);
	if(serial != NULL)
		{
		aCertDesc.SetMapSerialNumberL(*serial);               
		}
    
	// Set Subject Key Identifier if we are handling CA
	if(aOwner == ECACertificate)
	{
	    TPKIKeyIdentifier keyId = certificate->SubjectKeyIdentifierL();
	    aCertDesc.SetMapSubjectKeyId(keyId);
	}
	
    CleanupStack::PopAndDestroy(certificate); 
    

    if(CPKIMapper::CertValidity(aCertDesc.iStartTime, aCertDesc.iEndTime) == EExpired)
    {
        LOG(Log::Printf(_L("Certificate expired\n")));
    }
}


void CPKIWrapper::SetCertStoreType(TPkiServiceStoreType aStoreType)
    {
	LOG(Log::Printf(_L("CPKIWrapper: SETTING CERT STORE TYPE: %d\n"), aStoreType));
    iPKISupport->SetCertStoreType(aStoreType);
    }

    
    
TPkiServiceStoreType CPKIWrapper::CertStoreType() const
    {
    return iPKISupport->CertStoreType();
    }
        
void CPKIWrapper::SetInformational(const TBool aInfoOnly) 
    {
    iInfoOnly = aInfoOnly;
    }
    
