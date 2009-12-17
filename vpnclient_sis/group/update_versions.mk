#
# Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  Calls version update script 
#

do_nothing :
	@rem do_nothing
	
MAKMAKE : do_nothing

BLD :
	@echo Updating versions..
	perl update_versions.pl
	@echo Update done!
	
FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN FINAL SAVESPACE : do_nothing