#!/usr/bin/perl
#
# This will generate a Train PA message completed simulator case that can be loaded in the radio simulator
# It is used to customise the train id, announcement id, and result, so anything can be sent back to the train.

use strict;
use Switch;
use Getopt::Long;

my $trainId = 0;
my $announcementId = 0;
my $result = 0;
my $resultString = "";
my $resultText = "";
my $outputFile = "";


GetOptions( "trainId=i"=>\$trainId,
            "announcementId=i"=>\$announcementId,
            "result=s"=>\$resultString,
            "file=s"=>\$outputFile );

switch ( $resultString )
{
    case "Successful"	{ $result = 1; $resultText = "Successful"; }
    case "OCC"		{ $result = 2; $resultText = "Interrupted by OCC"; }
    case "PSC"		{ $result = 3; $resultText = "Interrupted by PSC"; }
    case "Driver"	{ $result = 4; $resultText = "Interrupted by Driver"; }
}

if ( ( $result == 0 ) ||
     ( $trainId == 0 ) ||
     ( $announcementId == 0 ) )
{
    print "Usage: ./TrainPaMessageCompleted.pl --trainId=X --announcementId=X --result=Successful|OCC|PSC|Driver --file=<Output File Name>";
    exit;
}

my $trainIdDec = sprintf( "%02d", $trainId );
my $trainIdHex = sprintf( "%02X", $trainId );

my $announcementIdDec = sprintf( "%02d", $announcementId );
my $announcementIdHex = sprintf( "%02X", $announcementId );

my $resultHex = sprintf( "%02X", $result );

my $template = <<TEMPLATE_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

Name = UC-PA-01.2 (/24) Train Message Complete T:$trainIdDec Ann:$announcementIdDec Result:$resultText
Data = 
//24:  Length   Session  TransID  Status   IncSDS   SDSref
Resp = 19010000 ******** 00000000 00000000 02A00000 ******** 
Resp = "28${trainIdDec}1-7-525"/32
Resp = ******** ******** ******** ******** ******** ******** ******** ********
Resp = 00000000 09000000
//09:  Flag	Message	Train	Length	Origin	Status4	AnnID	CHKSUM
Resp = 07	18	$trainIdHex	09	00	$resultHex	$announcementIdHex	****
RespSize = 281

Ovar = #:-2:2

Wait = 0

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

