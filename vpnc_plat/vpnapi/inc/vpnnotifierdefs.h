/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VPN notifier definitions
*
*/


#ifndef __VPNNOTIFIERDEFS_H__
#define __VPNNOTIFIERDEFS_H__

/**
 @file
 @released
 @publishedPartner
 
 VPN Notifier definitions
*/
class TNoteDialog
    {
public:
    enum TDialogId
        {
        EInfo = -10,
        EWarning,
        EError
        };
    };

class TVpnDialogInfo
    {
public:
    TVpnDialogInfo()
        {
        iDialogId = 0;
        iNoteDialogId = 0;
        }

    TVpnDialogInfo(TInt aDialogId, TInt aNoteDialogId)
        {
        iDialogId = aDialogId;
        iNoteDialogId = aNoteDialogId;
        }

    TInt DialogId() {return iDialogId;}
    TInt NoteDialogId() {return iNoteDialogId;}

private:
    TInt iDialogId;
    TInt iNoteDialogId;
    };

class TVpnDialogOutput
    {
public:
    TVpnDialogOutput()
        {
        Clear();
        }
    void Clear()
        {
        iOutBuf.SetLength(0);
        iOutBuf2.SetLength(0);
        iOutInt = 0;
        iOutInt2 = 0;
        }

public:
    TBuf<256> iOutBuf;
    TBuf<64> iOutBuf2;
    TInt iOutInt;
    TInt iOutInt2;
    };

// typedefs for backward compatibility
// typedef new_type old_type
typedef TVpnDialogInfo TIPSecDialogInfo;
typedef TVpnDialogOutput TIPSecDialogOutput;


// 
// KMD notifier definitions
//
const TUid KUidKmdDialogNotifier = {0x101F513F};

_LIT(KKmdNotifierResource, "\\resource\\KMDNOTIFIER.RSC");

class TKmdDialog
    {
public:
    enum TDialogId
        {
        EUserPwd = 1, // MUST be greater than the last value in enum TNoteDialog::TDialogId
        ESecurIdPin,
        ESecurIdNextPin,
        EChallengeResponse,
        EUsername
        };
    //added here for backward compatibility
    enum TDialogId_OLD_STYLE
        {
        //EUserPwd = 1, // MUST be greater than the last value in enum TNoteDialog::TDialogId
        ESecurIDPIN = 2,
        ESecurIDNextPIN
        };
    };

class TKmdNoteDialog
    {
public:
    enum TTextId
        {
        ELamTypeNotSupported = 1,
        EAuthenticationFailed,
        ECryptoLibraryTooWeak
        };
    //added here for backward compatibility
    enum TTextId_OLD_STYLE
        {
        ELAMTypeNotSupported = 1//,
        //EAuthenticationFailed,
        //ECryptoLibraryTooWeak
        };
    };

// typedefs for backward compatibility
// typedef existing_type old_type
typedef TKmdDialog TKMDDialog;

typedef TKmdNoteDialog TKMDNoteDialog;

// 
// PKI notifier definitions
//
_LIT(KPkiNotifierResource,"\\resource\\PKINOTIFIER.RSC");

const TUid KUidPkiDialogNotifier = {0x101FAE08};

class TPkiDialog
    {
public:
    enum TDialogId
        {
        EEnterPwd = 1, // MUST be greater than the last value in enum TNoteDialog::TDialogId (vpnnotifierdefs.h)
        EChangeActivePwd,
        EInstallVpnPwd,
        ECurrentVpnPwd,
        EChangePwd,
        EDefinePwd,
        EEnterImportPwd
        };
    };

class TPkiNoteDialog
    {
public:
    enum TTextId
        {
        EWrongSecurityPwdNote = 1,
        EWrongActivatePwdNote,
        EActivatePwdErrorNote,
        EPwdConfirmationNote,
        ECryptoLibraryTooWeak,
        EPwdTooShort,
        EPwdEmpty
        };
    };

// typedefs for backward compatibility
// typedef new_type old_type
typedef TPkiDialog TIPSecDialog;
typedef TPkiNoteDialog TIPSecNoteDialog; // this was only used in pkinotifier (ipsecnotifier)

#endif  // __VPNNOTIFIERDEFS_H__
