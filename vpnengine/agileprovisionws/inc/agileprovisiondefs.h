/*
* Copyright (c) 2000 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Provides file handling functions.
*
*/
#ifndef __VPNAGILEPROVISIONDEFS_H__
#define __VPNAGILEPROVISIONDEFS_H__

#include <e32base.h>


_LIT(KAgileProvisionPolicyFileName, "agileVPN.pol");
_LIT(KAgileProvisionPinFileName, "agileVPN.pin");
_LIT(KAgileProvisionCertificateFileName, "agileVPN-User.cer");
_LIT(KAgileProvisionServiceNameSpace, "agileVPNProvisionService");
_LIT8(KCRLF, "\r\n");
_LIT8(KHTTPprefix, "https://");
_LIT8(KServiceSuffix, "/AWS/AgileVPNProvisionService.asmx");
_LIT8(KPolicyRequestPrefix, "<xdb:GetConf xmlns:xdb=\"");
_LIT8(KCertificatRequestPrefix, "<xdb:GetCertificate xmlns:xdb=\"");
_LIT8 (KSoapActionGetPolicySuffix, "/GetConf");
_LIT8 (KSoapActionGetCertificateSuffix, "/GetCertificate");
_LIT(KPolFileExtension, ".pol");

_LIT8(KSoapAction, "http://vpn.agileprovision/GetConf");
_LIT8(KRequest, "<xdb:GetConf xmlns:xdb=\"http://vpn.agileprovision\"/>");

_LIT8(KSoapActionCert, "http://vpn.agileprovision/GetCertificate");

_LIT8(KRequest1, "<xdb:GetCertificate xmlns:xdb=\"http://vpn.agileprovision\"><xdb:certreq>");
_LIT8(KRequest2, "</xdb:certreq></xdb:GetCertificate>");


#endif  // __VPNAGILEPROVISIONDEFS_H__



