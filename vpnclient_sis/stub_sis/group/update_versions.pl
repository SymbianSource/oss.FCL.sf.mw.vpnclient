#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#
#
# update_versions.pl
#
# Updates pkg-files' and vpnclient_sis.mk's version information with current date.
# Creates temporary pkg-files with updated version.
#

use strict;
use POSIX qw(strftime);
use File::Copy;


my $PLATFORM = "41";

#
# Creates a temp file and replaces line
#
sub replace_line_and_create_temp
{
	# arguments
	my ($pattern, $replacement, $path) = @_;
		
	# create temp file
	my $temp_path = "temp_".$path;
	if (-e $temp_path)
	{
		chmod 0666, $temp_path;
		unlink $temp_path;
	}
	copy($path, $temp_path);
	chmod 0666, $temp_path;
	
	# update version information
	my ($infile, $outfile);
	open $infile, "<".$path or die "Can't open $path : $!";
	open $outfile, ">".$temp_path or die "Can't open $temp_path : $!";
	while (<$infile>)
	{
		my $line = $_;
		if ($line =~ s/$pattern/$replacement/g)
		{
			print "Replaced line in $temp_path\n";
		}
		print $outfile $line;
	}
	close $infile;
	close $outfile;
}

my $pkgtime = strftime($PLATFORM.",%y,%m%d", localtime());
my $mktime = strftime("%y%m%d", localtime());

# stub_nokia_vpn_vpnpolins_armv5.pkg
replace_line_and_create_temp(
    '^#{"Nokia VPN Policy Installer Stub"},\(0xA0000131\),.*?,.*?,.*?',
    "#{\"Nokia VPN Policy Installer Stub\"},(0xA0000131),$pkgtime",
    'stub_vpnpolins_armv5.pkg');
    
# stub_nokia_vpn_client_localised_armv5.pkg
replace_line_and_create_temp(
    '^\(0x101F5147\),.*?,.*?,.*?$',
    "(0x101F5147),$pkgtime",
    'stub_nokia_vpn_client_armv5.pkg');
