#!/usr/bin/perl
#
# This will generate a Train PA request for reset simulator case that can be loaded in the radio simulator
# It is used to customise the train id and announcement id so anything can be sent back to the train.

use strict;
use Switch;
use Getopt::Long;

my $trainId = 0;
my $announcementId = 0;
my $outputFile = "";

GetOptions( "trainId=i"=>\$trainId,
            "announcementId=i"=>\$announcementId,
            "file=s"=>\$outputFile );

if ( ( $trainId == 0 ) ||
     ( $announcementId == 0 ) )
{
    print "Usage: ./TrainPaRequestForReset.pl --trainId=X --announcementId=X --file=<Output File Name>";
    exit;
}

my $trainIdDec = sprintf( "%02d", $trainId );
my $trainIdHex = sprintf( "%02X", $trainId );

my $announcementIdDec = sprintf( "%02d", $announcementId );
my $announcementIdHex = sprintf( "%02X", $announcementId );

my $template = <<TEMPLATE_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

Name = UC-PA-03.2 (/22) PA Request for Reset T:$trainIdDec Ann:$announcementIdDec
Data =
//
//24:  Length   Session  TransID  Status   IncSDS   SDSref
Resp = 19010000 ******** 00000000 00000000 02A00000 ******** 
Resp = "28${trainIdDec}1-7-525"/32
Resp = ******** ******** ******** ******** ******** ******** ******** ********
Resp = 00000000 08000000
//08:  Flag	Message	Train	Length	Origin	AnnID	CHKSUM
Resp = 07	16	$trainIdHex	08	00	$announcementIdHex	****
RespSize = 281

Ovar = #:-2:2

TEMPLATE_END

if ( $outputFile eq "" )
{
    print $template;
}
else
{
    open ( OUTPUT, ">$outputFile" );
    print OUTPUT $template;
    close ( OUTPUT ); 
}

