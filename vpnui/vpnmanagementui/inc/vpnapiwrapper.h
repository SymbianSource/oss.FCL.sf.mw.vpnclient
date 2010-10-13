/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the CVpnApiWrapper class.
*
*/

#ifndef __VPNAPIWRAPPER_H__
#define __VPNAPIWRAPPER_H__

#include <e32base.h>
#include "vpnapi.h"


class MVpnApiWrapperCaller
    {
public:
    virtual void NotifyPolicyImportComplete(TInt aResult) = 0;
    };

/**
 * The class definitions for CVpnApiWrapper -
 * Wrapper class for VPN API calls 
 */
class CVpnApiWrapper : public CActive
    {
public:
    /**
     * NewL method starts the standard two phase construction.
     */	
    static CVpnApiWrapper* NewL();

    /**
     * Destructor
     */
    ~CVpnApiWrapper();

    CArrayFixFlat<TVpnPolicyInfo>* PolicyListL();
    
    /**
     * Deletes the specified policy.
     *
     * @param aPolicyIndex Index of the listbox in the policy view
     */
    void DeletePolicyL(TInt aPolicyIndex);
    
    /**
     * Gets detailed information about the specified policy.
     *
     * @param aPolicyIndex Index of the listbox in the policy view
     * 
     * @param aPolicyDetails [out] Policy information structure
     */
    void GetPolicyDetailsL(
        TInt aPolicyIndex, TVpnPolicyDetails& aPolicyDetails);
    
    /**
     * Imports a new VPN policy to the policy store.
     *
     * @param aImportDir An absolute path to the VPN policy files directory
     * 
     * @param aCaller
     */
    void ImportPolicyL(
        const TDesC& aImportDir, MVpnApiWrapperCaller* aCaller);

    /**
     * Gets the last update time of the specified policy.
     * @param aPolicyIndex Index of the listbox in the policy view.
     * @param aTime Last update time to be returned.
     * @return Returns KErrNone if succeed.
     * Otherwise it returns KErrNotFound.
     */
    TInt GetLastUpdateTime(TInt aPolicyIndex, TTime& aTime);

protected: // From CActive
    void DoCancel();
    void RunL();
    
private:
    /**
     * Constructor
     */
    CVpnApiWrapper();

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Builds the list of the installed policies.
     */
    void BuildPolicyListL();

private:

    enum TTask
        {
        ETaskNone = 1,
        ETaskImportPolicies
        };

    RVpnApi iVpnApi;

    CArrayFixFlat<TVpnPolicyInfo>* iPolicyList;
    TFileName iImportDir;

    TTask iOngoingTask;
    MVpnApiWrapperCaller* iCaller;
    };

#endif  // __VPNAPIWRAPPER_H__
