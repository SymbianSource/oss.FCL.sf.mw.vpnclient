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
* Description: Handles API requests for VPN Manager.
*
*/


#include <e32std.h>
#include <f32file.h>

#include "fileutil.h"
#include "vpnapiservant.h"
#include "policystore.h"
#include "policyimporter.h"
#include "pwdchanger.h"
#include "vpnapidefs.h"
#include "vpnmanagerserverdefs.h"
#include "vpnextapiservantdefs.h"
#include "vpnmaninternal.h"
#include "log_r6.h"
#include "agileprovisionws.h"
#include "agileprovisiondefs.h"
#include "cmmanagerutils.h"


CVpnApiServant* CVpnApiServant::NewL(RFs& aFs)
    {
    CVpnApiServant* self = new (ELeave) CVpnApiServant(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CVpnApiServant::CVpnApiServant(RFs& aFs) : iFs(aFs), iFileUtil(aFs)
    {
    }

void CVpnApiServant::ConstructL()
    {
    iPolicyStore = CPolicyStore::NewL(iFs);
    INIT_EVENT_LOGGER(*iPolicyStore);
    }

CVpnApiServant::~CVpnApiServant()
    {
    delete iPolicyStore;
    RELEASE_EVENT_LOGGER;
    }
    
TBool CVpnApiServant::ServiceL(const RMessage2& aMessage)
    {
    TBool requestHandled = ETrue;
    
    switch (aMessage.Function())
        {
        case EVpnImportPolicy:
            ImportPolicyL(aMessage);
            break;
            
        case EVpnCancelImport:
            CancelImportPolicy(aMessage);
            break;
            
        case EVpnEnumeratePolicies:
            EnumeratePoliciesL(aMessage);
            break;
            
        case EVpnGetPolicyInfo:
            GetPolicyInfoL(aMessage);
            break;
            
        case EVpnGetPolicyDetails:
            GetPolicyDetailsL(aMessage);
            break;
            
        case EVpnDeletePolicy:
            DeletePolicyL(aMessage);
            break;
            
        case EVpnChangePassword:
            ChangePasswordL(aMessage);
            break;

        case EVpnCancelChange:
            CancelChangePassword(aMessage);
            break;

        case EVpnGetPolicySize:
            GetPolicySizeL(aMessage);
            break;
            
        case EVpnGetPolicyData:
            GetPolicyDataL(aMessage);
            break;

        // New methods (to facilitate
        // OMA DM based policy management)
        
        case EVpnAddPolicy:
            AddPolicyL(aMessage);
            break;

        case EVpnUpdatePolicyDetails:
            UpdatePolicyDetailsL(aMessage);
            break;
            
        case EVpnUpdatePolicyData:
            UpdatePolicyDataL(aMessage);
            break;
            
        
        //Policy Provision Methods
        case EExtCreateProvisionServer:
            CreateProvisionServerL(aMessage);
            break;
         
        case EExtVPNPolicyServerList:
            ListProvisionServerL(aMessage);
            break;
            
        case EExtVPNPolicyServerDetails:
            GetProvisionServerDetailsL(aMessage);
            break;
            
        case EExtSynchronizePolicyServer:
            SynchronizeVPNPolicyServerL(aMessage);
            break;
            
        case EExtDeletePolicyServer:
             DeleteVPNPolicyServerL(aMessage);
             break;
        
        case EExtCancelSynchronize:
             CancelSynchronize(aMessage);
             break;
        
        case EExtGetPolicyName:
             GetVPNPolicyNameL(aMessage);
             break;
             
        default:
            requestHandled = EFalse;
            break;
        }

    return requestHandled;
    }

void CVpnApiServant::ImportPolicyL(const RMessage2& aMessage)
    {
    if (iPolicyImporter)
        {
        aMessage.Complete(KVpnErrImportOngoing);
        }
    else
        {
        TInt len = aMessage.GetDesLength(0);
        HBufC* input = HBufC::NewL(len);
        CleanupStack::PushL(input);
        
        TPtr ptrInput = input->Des();
        aMessage.ReadL(FIRST_ARGUMENT, ptrInput);
        
        if (input->Length() > 0)
            {                    
            iPolicyImporter = CPolicyImporter::NewL(aMessage, *this, *iPolicyStore, iFs);
            iPolicyImporter->ImportPolicyL(*input);
            }
        else
            {
            aMessage.Complete(KVpnErrPolicyNotFound);
            }

        CleanupStack::PopAndDestroy(); // input
        }
    }

void CVpnApiServant::ImportSinglePolicyL(const TDesC& aDir, TVpnPolicyId& aNewPolicyId,
                                         TRequestStatus& aStatus)
    {
    if (iPolicyImporter)
        {
        User::Leave(KVpnErrImportOngoing);
        }
    else
        {
        iPolicyImporter = CPolicyImporter::NewL(aStatus, *this, *iPolicyStore, iFs);
        aStatus = KRequestPending;
        iPolicyImporter->ImportSinglePolicyL(aDir, aNewPolicyId);
        }
    }
    
void CVpnApiServant::CancelImportPolicy(const RMessage2& aMessage)
    {
    if (iPolicyImporter)
        {
        iPolicyImporter->Cancel();
        delete iPolicyImporter;
        iPolicyImporter = NULL;
        }
    
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::CancelImportSinglePolicy()
    {
    if (iPolicyImporter)
        {
        iPolicyImporter->Cancel();
        delete iPolicyImporter;
        iPolicyImporter = NULL;
        }
    }
    
void CVpnApiServant::PolicyImportComplete()
    {
    delete iPolicyImporter;
    iPolicyImporter = NULL;
    }

void CVpnApiServant::EnumeratePoliciesL(const RMessage2& aMessage)
    {
    TInt policyCount = iPolicyStore->PolicyCount();

    TPckg<TInt> pckgPolicyCount(policyCount);
    aMessage.WriteL(FIRST_ARGUMENT, pckgPolicyCount);

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetPolicyInfoL(const RMessage2& aMessage)
    {
	TInt expectedPolicyCount = aMessage.Int0();

    if (expectedPolicyCount != iPolicyStore->PolicyCount())
        {
        aMessage.Complete(KVpnErrPolicyCountChanged);
        return;
        }

    TUint8* rawPolicyData = iPolicyStore->RawPolicyData();
    
	// Write the state array back to the client's address space
	TPtrC8 policyData(rawPolicyData, expectedPolicyCount * sizeof(TVpnPolicyInfo));
    
    aMessage.WriteL(SECOND_ARGUMENT, policyData);

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetPolicyDetailsL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CVpnApiServant::GetPolicyDetailsL")));    
    
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TVpnPolicyDetails policyDetails;
    TInt ret = iPolicyStore->GetPolicyDetailsL(policyId, policyDetails);

    if (ret == KErrNone)
        {
        TPckg<TVpnPolicyDetails> pckgPolicyDetails(policyDetails);
        aMessage.WriteL(SECOND_ARGUMENT, pckgPolicyDetails);
        }
        
            
    aMessage.Complete(ret);
    
    LOG(Log::Printf(_L("CVpnApiServant::GetPolicyDetailsL: aMessage completed withd %d"), ret));    
    }

void CVpnApiServant::DeletePolicyL(const RMessage2& aMessage)
    {
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TVpnPolicyDetails *policyDetails = new (ELeave) TVpnPolicyDetails;
    CleanupStack::PushL(policyDetails);    
    
    iPolicyStore->GetPolicyDetailsL(policyId, *policyDetails);    
    iPolicyStore->DeletePolicyL(policyId);


    HBufC8* tempBuf = HBufC8::NewLC(policyDetails->iName.Length());
    tempBuf->Des().Copy(policyDetails->iName);
    
    LOG_EVENT(R_VPN_MSG_DELETED_POLICY, tempBuf, NULL, 0, 0);

    CleanupStack::PopAndDestroy(2); //tempBuf, policyDetails
    
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::ChangePasswordL(const RMessage2& aMessage)
    {
    if (iPwdChanger)
        {
        aMessage.Complete(KVpnErrPwdChangeOngoing);
        }
    else
        {
        iPwdChanger = CPwdChanger::NewL(aMessage, *this);
        iPwdChanger->ChangePassword();
        }
    }

void CVpnApiServant::CancelChangePassword(const RMessage2& aMessage)
    {
    if (iPwdChanger)
        {
        iPwdChanger->Cancel();
        delete iPwdChanger;
        iPwdChanger = NULL;
        }
    
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::PasswordChangeComplete()
    {
    delete iPwdChanger;
    iPwdChanger = NULL;
    }

void CVpnApiServant::GetPolicySizeL(const RMessage2& aMessage)
    {
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TInt policySize = PolicySizeL(policyId);

    TPckg<TInt> pckgPolicySize(policySize);
    aMessage.WriteL(SECOND_ARGUMENT, pckgPolicySize);

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetPolicyDataL(const RMessage2& aMessage)
    {
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    TInt expectedPolicySize;
    TPckg<TInt> pckgExpectedPolicySize(expectedPolicySize);

    aMessage.ReadL(SECOND_ARGUMENT, pckgExpectedPolicySize);

    TInt currentPolicySize = PolicySizeL(policyId);

    if (expectedPolicySize != currentPolicySize)
        {
        aMessage.Complete(KVpnErrPolicySizeChanged);
        return;
        }

    HBufC8* policyData = NULL;
    User::LeaveIfError(iPolicyStore->LoadPolicyDataL(policyId, policyData));
    CleanupStack::PushL(policyData);
    
    aMessage.WriteL(THIRD_ARGUMENT, *policyData);

    CleanupStack::PopAndDestroy(); // policyData

    aMessage.Complete(KErrNone);
    }
    
CPolicyStore* CVpnApiServant::PolicyStore()
    {
    return iPolicyStore;
    }

TInt CVpnApiServant::PolicySizeL(const TVpnPolicyId& aPolicyId)
    {
    HBufC8* policyData = NULL;
    
    User::LeaveIfError(iPolicyStore->LoadPolicyDataL(aPolicyId, policyData));
    TInt policySize = policyData->Size();
    delete policyData;
    
    return policySize;
    }

// New methods to facilitate OMA DM based VPN policy management

void CVpnApiServant::AddPolicyL(const RMessage2& aMessage)
    {
    
    LOG(Log::Printf(_L("CVpnApiServant::AddPolicyL")));    
    
    // Read policy details
    TVpnPolicyDetails *policyDetails = new (ELeave) TVpnPolicyDetails;
    CleanupStack::PushL(policyDetails);
    
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(*policyDetails);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyDetails);

    // Read policy data
    TInt len = aMessage.GetDesLength(SECOND_ARGUMENT);
    HBufC8* policyData = HBufC8::NewLC(len);
    TPtr8 ptrPolicyData = policyData->Des();
    aMessage.ReadL(SECOND_ARGUMENT, ptrPolicyData);

    //Make a validy check to the policy data
    //by parsing it and checking certificate formats
    HBufC* policyData16 = HBufC::NewLC(policyData->Length());
    policyData16->Des().Copy(*policyData);
    
    CIkeDataArray* ikeDataArray = CIkeDataArray::NewL(1);
    CleanupStack::PushL(ikeDataArray);
    
    TIkeParser* ikeParser = new (ELeave) TIkeParser(*policyData16);
    CleanupStack::PushL(ikeParser);
    ikeParser->ParseIKESectionsL(ikeDataArray);

    for (TInt i = 0; i < ikeDataArray->Count(); ++i)
        {
        const CIkeData* ikeData = ikeDataArray->At(i);
        if (ikeData->iCAList != NULL)
            {
            for (TInt j = 0; j < ikeData->iCAList->Count(); j++)
                {
                if (ikeData->iCAList->At(j)->iFormat == BIN_CERT)
                    {
                    LOG(Log::Printf(_L("Policy contains BIN certificates --> Failing")));
                    //Ca cert in wrong format --> Error
                    User::Leave(KVpnErrInvalidPolicyFile);
                    }
                }            
            }
        if ((ikeData->iOwnCert.iData.Length() > 0 &&
             ikeData->iOwnCert.iFormat == BIN_CERT) ||
            (ikeData->iPrivKey.iData.Length() > 0 &&
             ikeData->iPrivKey.iFormat == BIN_CERT) ||
            (ikeData->iPeerCert.iData.Length() > 0 &&
            ikeData->iPeerCert.iFormat == BIN_CERT))
            {
            LOG(Log::Printf(_L("Policy contains BIN certificates --> Failing")));    
            //Key or user cert in wrong format
            User::Leave(KVpnErrInvalidPolicyFile);
            }        
        }
    
    CleanupStack::PopAndDestroy(); //ikeParser
    CleanupStack::PopAndDestroy(ikeDataArray);
    CleanupStack::PopAndDestroy(policyData16);
    
    LOG(Log::Printf(_L("Calling: iPolicyStore->AddNewPolicyL")));    
    TRAPD(err, iPolicyStore->AddNewPolicyL(*policyDetails, *policyData));
    if (err == KErrNone)
    {
        TBuf<20> serverName(_L("VPN DM Server"));
        TPtrC8 tempCastPtr8(reinterpret_cast<const TUint8*>(serverName.Ptr()), serverName.Length() * 2);
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &(policyDetails->iId),
                &tempCastPtr8, err, EFalse);
    }
    else
    {
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &(policyDetails->iId), NULL,
                 err, EFalse);
        User::Leave(err);
    }    

    LOG(Log::Printf(_L("Writing back policy details")));    
    // Write back the possibly changed policy details
    aMessage.WriteL(FIRST_ARGUMENT, pckgPolicyDetails);

    CleanupStack::PopAndDestroy(policyData);    
    CleanupStack::PopAndDestroy(); //policyDetails

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::UpdatePolicyDetailsL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CVpnApiServant::UpdatePolicyDetailsL")));    
    
    // Read policy details
    TVpnPolicyDetails* policyDetails = new (ELeave) TVpnPolicyDetails;
    CleanupStack::PushL(policyDetails);
    
    TPckg<TVpnPolicyDetails> pckgPolicyDetails(*policyDetails);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyDetails);

    // Update the policy details
    TRAPD(err, iPolicyStore->UpdatePolicyDetailsL(*policyDetails));
    if (err != KErrNone)
    {
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &(policyDetails->iId), NULL,
                 err, ETrue);
        User::Leave(err);
    }

    // Write back the possibly changed policy details
    aMessage.WriteL(FIRST_ARGUMENT, pckgPolicyDetails);

    CleanupStack::PopAndDestroy(); //policyDetails
    
    aMessage.Complete(KErrNone);
    }
    
void CVpnApiServant::UpdatePolicyDataL(const RMessage2& aMessage)
    {
    // Read policy ID
    TVpnPolicyId policyId;
    TPckg<TVpnPolicyId> pckgPolicyId(policyId);

    aMessage.ReadL(FIRST_ARGUMENT, pckgPolicyId);

    // Read policy data
    TInt len = aMessage.GetDesLength(SECOND_ARGUMENT);
    HBufC8* policyData = HBufC8::NewL(len);
    CleanupStack::PushL(policyData);

    TPtr8 ptrPolicyData = policyData->Des();
    aMessage.ReadL(SECOND_ARGUMENT, ptrPolicyData);

    // Update the policy data
    TRAPD(err, iPolicyStore->UpdatePolicyDataL(policyId, *policyData));
    if (err == KErrNone)
    {
        TBufC8<20> serverName(_L8("VPN DM Server"));
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &policyId,
                &serverName, err, ETrue);
    }
    else
    {
        LOG_EVENT(R_VPN_MSG_INSTALLED_POLICY_SERVER, &policyId, NULL,
                 err, ETrue);
        User::Leave(err);
    }
    CleanupStack::PopAndDestroy(); // policyData

    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::CreateProvisionServerL( const RMessage2& aMessage )
    {
      TAgileProvisionApiServerSettings* serverCreate = new (ELeave) TAgileProvisionApiServerSettings();
      CleanupStack::PushL(serverCreate);
      TPckg<TAgileProvisionApiServerSettings> pckgServerCreate(*serverCreate);
      aMessage.ReadL(0, pckgServerCreate);
      TAgileProvisionServerLocals* serverAccountLocalData = new (ELeave) TAgileProvisionServerLocals();
      CleanupStack::PushL(serverAccountLocalData);
      serverAccountLocalData->iSelection = serverCreate->iSelection;
      serverAccountLocalData->iServerAddress.Copy(serverCreate->iServerUrl);
      serverAccountLocalData->iServerNameLocal.Copy(serverCreate->iServerNameLocal);
  
      TFileName serverFilePath;
      User::LeaveIfError(iFs.PrivatePath(serverFilePath));
      serverFilePath.Append(KProvisionServerSettings);   
         
      /* Check if file allready exists and copy policy and vpn iap id to a new file */
      TFileName policyFileName;
      TUint32 agileProvisionAPId=0;
            
      if ( iFileUtil.FileExists(serverFilePath) )
         {
          HBufC8* fileData=iFileUtil.LoadFileDataL(serverFilePath);
          CleanupStack::PushL(fileData);
             
          TPtrC8 restOfData = fileData->Des();
                
          TInt bofInt;
          TInt line=1;             
          while ( (bofInt=restOfData.Find(KCRLF)) != KErrNotFound && line < KPolicyFileLine )
                {
                restOfData.Set(restOfData.Mid(bofInt + KCRLF().Length()));
                line++;                                  
                }
          TInt iapIdStart=restOfData.Find(KCRLF);
          HBufC16* iapIdBuf;
                 
          if ( iapIdStart!=KErrNotFound )
              {
              TPtrC8 iapIdPtr=restOfData.Mid(iapIdStart + KCRLF().Length(),restOfData.Length()-KCRLF().Length()-iapIdStart);
              iapIdBuf=iFileUtil.To16BitL(iapIdPtr);
              CleanupStack::PushL(iapIdBuf);
              TLex iapIdConverter(*iapIdBuf);
              iapIdConverter.Val(agileProvisionAPId,EDecimal);     
              CleanupStack::PopAndDestroy(iapIdBuf);           
              }
                 
          if ( agileProvisionAPId >0)
              {
               restOfData.Set(restOfData.Mid(0,iapIdStart));
               HBufC16* policyFileNameBuf = iFileUtil.To16BitL(restOfData);
               policyFileName = *policyFileNameBuf;
               delete policyFileNameBuf;
              }
          CleanupStack::PopAndDestroy(fileData);
          }
      /* end of saving old values */
      
      //IAP data Max value 255 
      TBuf<10> iapIdStr;
      TBuf<10> iapModeStr;
      TBuf<10> iapAgileIdStr;
      
      iapIdStr.Num(serverAccountLocalData->iSelection.iId);
      iapModeStr.Num(serverAccountLocalData->iSelection.iResult);
      HBufC* serverSettingsDataBuf;
      if ( agileProvisionAPId >0 )
          {
          iapAgileIdStr.Num(agileProvisionAPId);
                                                                                                                          
          serverSettingsDataBuf = HBufC::NewL(serverAccountLocalData->iServerNameLocal.Length() + serverAccountLocalData->iServerAddress.Length() + 
                                              iapIdStr.Length() + iapModeStr.Length() + policyFileName.Length() + iapAgileIdStr.Length() + 5*(KCRLF().Length()) );
          }
      else
          {                                                                                                         
          serverSettingsDataBuf = HBufC::NewL(serverAccountLocalData->iServerNameLocal.Length() + serverAccountLocalData->iServerAddress.Length() +
                                              iapIdStr.Length() + iapModeStr.Length() + 3*(KCRLF().Length()) );
          }
      CleanupStack::PushL(serverSettingsDataBuf);
      TPtr tPtr(serverSettingsDataBuf->Des());
      tPtr.Copy(serverAccountLocalData->iServerAddress);
      _LIT(KCRLF, "\r\n"); 
      tPtr.Append(KCRLF);
      tPtr.Append(serverAccountLocalData->iServerNameLocal);
      tPtr.Append(KCRLF);
      tPtr.Append(iapIdStr);
      tPtr.Append(KCRLF);
      tPtr.Append(iapModeStr);
      if ( agileProvisionAPId >0 )
          {
          tPtr.Append(KCRLF);
          tPtr.Append(policyFileName);
          tPtr.Append(KCRLF);
          tPtr.Append(iapAgileIdStr);
          }
     
      iFileUtil.SaveFileDataL(serverFilePath,tPtr);
      CleanupStack::PopAndDestroy(3);
      aMessage.Complete(KErrNone);
    }

void CVpnApiServant::ListProvisionServerL( const RMessage2& aMessage )
    {
      _LIT8(KCRLF, "\r\n");
      
      const TInt KEolLen = 2;
      
      TAgileProvisionApiServerListElem* serverList = new (ELeave) TAgileProvisionApiServerListElem();
      CleanupStack::PushL(serverList);
      TPckg<TAgileProvisionApiServerListElem> serverPckg(*serverList);
          
      TFileName serverFilePath;
      User::LeaveIfError(iFs.PrivatePath(serverFilePath));
      serverFilePath.Append(KProvisionServerSettings);   
      
      HBufC8* fileData(NULL);
     
      if ( iFileUtil.FileExists(serverFilePath) )
          {
          fileData=iFileUtil.LoadFileDataL(serverFilePath);
          CleanupStack::PushL(fileData);
          TInt endOfLine=fileData->Find(KCRLF);
          serverList->iServerUrl=fileData->Mid(0,endOfLine);
          
          TInt startOfLine(endOfLine+KEolLen);
          TPtrC8 nameData=fileData->Right(fileData->Length()-startOfLine);
          endOfLine=nameData.Find(KCRLF);
          HBufC16* serverName=iFileUtil.To16BitL(nameData.Left(endOfLine));
          serverList->iServerNameLocal=*serverName;
          delete serverName;
          serverName = NULL;
          }
      
      aMessage.WriteL(0, serverPckg);
      if ( iFileUtil.FileExists(serverFilePath) )
          CleanupStack::PopAndDestroy(fileData);
      
      CleanupStack::PopAndDestroy(serverList);
      aMessage.Complete(KErrNone);
    }

void CVpnApiServant::GetProvisionServerDetailsL( const RMessage2& aMessage )
    {
     
      TAgileProvisionApiServerSettings* serverList = new (ELeave) TAgileProvisionApiServerSettings();
      CleanupStack::PushL(serverList);
      TPckg<TAgileProvisionApiServerSettings> serverPckg(*serverList);
      
      _LIT8(KCRLF, "\r\n");     
      
      TFileName serverFilePath;
      User::LeaveIfError(iFs.PrivatePath(serverFilePath));
      serverFilePath.Append(KProvisionServerSettings);  
      
      HBufC8* fileData(NULL);
      const TInt KEolLen = 2;
      TBool serverFileExist = EFalse;
      
      if ( iFileUtil.FileExists(serverFilePath) )
         {
          fileData=iFileUtil.LoadFileDataL(serverFilePath);
          CleanupStack::PushL(fileData);  
          TInt endOfLine=fileData->Find(KCRLF);
          serverList->iServerUrl=fileData->Mid(0,endOfLine);
                
          TInt startOfLine(endOfLine+2);
          TPtrC8 nameData=fileData->Right(fileData->Length()-startOfLine);
          endOfLine=nameData.Find(KCRLF);      
             
          HBufC16* serverName=iFileUtil.To16BitL(nameData.Left(endOfLine));
          serverList->iServerNameLocal=*serverName;
          delete serverName;
          serverName = NULL;
          
          startOfLine = endOfLine + KEolLen;
          TPtrC8 iapIdData=nameData.Right(nameData.Length()-startOfLine);
          endOfLine=iapIdData.Find(KCRLF);
          TLex8 iapIdConverter(iapIdData.Left(endOfLine));
          TUint idInt;
          iapIdConverter.Val(idInt);
          serverList->iSelection.iId = idInt;
        
          startOfLine = endOfLine + KEolLen;
          TPtrC8 iapModeData=iapIdData.Right(iapIdData.Length()-startOfLine);
          TLex8 iapModeConverter;
          endOfLine=iapModeData.Find(KCRLF);
          if ( endOfLine==KErrNotFound )
              iapModeConverter = iapModeData;
          else
              iapModeConverter = iapModeData.Left(endOfLine);
              
          iapModeConverter.Val(idInt);
          CMManager::TCmSettingSelectionMode selectionMode = (CMManager::TCmSettingSelectionMode) idInt;
          serverList->iSelection.iResult = selectionMode; 
          serverFileExist = ETrue;   
          }
      aMessage.WriteL(0, serverPckg);
      
      if ( serverFileExist )
          CleanupStack::PopAndDestroy(fileData);
      
      CleanupStack::PopAndDestroy(serverList);
      
      aMessage.Complete(KErrNone);
    }

void CVpnApiServant::SynchronizeVPNPolicyServerL( const RMessage2& aMessage )
    {
    iPolicyImporter = CPolicyImporter::NewL(aMessage, *this, *iPolicyStore, iFs);
    
    //Asynchronous call
    iPolicyImporter->SynchronizeVpnPolicyServerL();
    }

void CVpnApiServant::DeleteVPNPolicyServerL( const RMessage2& aMessage )
    {
    TFileName serverFilePath;
    User::LeaveIfError(iFs.PrivatePath(serverFilePath));
    serverFilePath.Append(KProvisionServerSettings);  
    iFileUtil.DeleteFileL(serverFilePath);
    aMessage.Complete(KErrNone);
    }

void CVpnApiServant::CancelSynchronize( const RMessage2& aMessage )
    {
    if (iPolicyImporter)
        {
        if ( iPolicyImporter->iAgileProvisionWs != NULL )
            {
            CAgileProvisionWs* ws = iPolicyImporter->iAgileProvisionWs;
            ws->CancelGetPolicy();
            }
        iPolicyImporter->Cancel();
        delete iPolicyImporter;
        iPolicyImporter = NULL;
        }
    aMessage.Complete(KErrCancel);
    }

void CVpnApiServant::GetVPNPolicyNameL( const RMessage2& aMessage )
    {
    TAgileProvisionPolicy* policy = new (ELeave) TAgileProvisionPolicy();
    CleanupStack::PushL(policy);
    TPckg<TAgileProvisionPolicy> serverPckg(*policy);
    
    _LIT8(KCRLF, "\r\n");
    
    TFileName serverFilePath;
    User::LeaveIfError(iFs.PrivatePath(serverFilePath));
    serverFilePath.Append(KProvisionServerSettings);  
    
    HBufC8* fileData(NULL);
         
    const TInt KEolLen = 2;
    TBool serverFileExist = EFalse;
    
    if ( iFileUtil.FileExists(serverFilePath) )
       {
        fileData=iFileUtil.LoadFileDataL(serverFilePath);
        CleanupStack::PushL(fileData);   
        TInt endOfLine=fileData->Find(KCRLF);
        if (endOfLine<=0)
            User::Leave(KErrArgument);
                        
        TInt startOfLine(endOfLine + KEolLen);
        TPtrC8 nameData=fileData->Right(fileData->Length()-startOfLine);
        endOfLine=nameData.Find(KCRLF);      
        if (endOfLine<=0)
            User::Leave(KErrArgument);      
                
        startOfLine = endOfLine + KEolLen;
        TPtrC8 iapIdData=nameData.Right(nameData.Length()-startOfLine);
        endOfLine=iapIdData.Find(KCRLF);
        if (endOfLine<=0)
            User::Leave(KErrArgument);      
           
        startOfLine = endOfLine + KEolLen;
        TPtrC8 iapModeData=iapIdData.Right(iapIdData.Length()-startOfLine);
        endOfLine=iapModeData.Find(KCRLF);
        if (endOfLine<=0)
            User::Leave(KErrArgument);      
        
        startOfLine = endOfLine + KEolLen;
        TPtrC8 policyData=iapModeData.Right(iapModeData.Length()-startOfLine);
        endOfLine=policyData.Find(KCRLF);
        if (endOfLine<=0)
            User::Leave(KErrArgument);      
        
        HBufC16* policyName = iFileUtil.To16BitL(policyData.Left(endOfLine));
        policy->iPolicyName = *policyName;
        delete policyName;
        policyName=NULL;
        serverFileExist = ETrue;
        }

    aMessage.WriteL(0, serverPckg);
    
    if ( serverFileExist )
        CleanupStack::PopAndDestroy(fileData);
    
    CleanupStack::PopAndDestroy(policy);
    
    aMessage.Complete(KErrNone);
    }
