#!/usr/bin/perl
#
# This will generate a Train PA message completed simulator case that can be loaded in the radio simulator
# It is used to customise the train id, announcement id, and result, so anything can be sent back to the train.

use strict;
use Switch;
use Getopt::Long;

my $trainId = 0;
my $resultString = "";
my $repeatString = "";
my $outputFile = "";

my $result = 9;

GetOptions( "trainId=i"=>\$trainId,
            "result=s"=>\$resultString,
            "repeat=s"=>\$repeatString,
            "file=s"=>\$outputFile );

switch ( $resultString )
{
    case "Ok"            { $result = 0; }
    case "Failed"        { $result = 1; }
    case "Repetition"    { $result = 2; }
}

if ( ( $result == 9 ) ||
     ( $trainId == 0 ) )
{
    print "Usage: ./TrainTisEndOfDownload.pl --trainId=X  --result=Ok|Failed|Repetition --repeat=<frame1,frame2,...> --file=<Output File Name>\n";
    print "The repeat parameter is only necessary (and required) for repetition mode\n";
    exit;
}

my $frames = "";
my $length = 8;
my $framesString = "";

if ( $result == 2 )
{
    # parse the repeat string
    my @frameList = split( /,/, $repeatString );

    if ( @frameList == 0 )
    {
        print "Usage: ./TrainTisEndOfDownload.pl --trainId=X  --result=Ok|Failed|Repetition --repeat=<frame1,frame2,...> --file=<Output File Name>\n";
        print "The repeat parameter is required for repetition mode\n";
        exit;
    }

    $frames .= "// Frames\nResp = ";
    $framesString = "Repeated Frames:";

    foreach my $frame (@frameList)
    {
        # frame numbers are 2 bytes long
        $length = $length + 2;
	my $frameLSB = $frame % 0xFF;
	my $frameMSB = $frame / 0xFF;

        $frames .= sprintf( "%02X", $frameLSB ) . sprintf( "%02X", $frameMSB ) . " ";
        $framesString .= "${frame},"
    }

    $frames .= "\n";
}

my $count = $result + 1;

my $trainIdDec = sprintf( "%02d", $trainId );
my $trainIdHex = sprintf( "%02X", $trainId );

my $resultHex = sprintf( "%02X", $result );

my $lengthHex = sprintf( "%02X", $length );

my $template = <<TEMPLATE_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

Name = UC-TIS-06.$count (/39) End Download Result:$resultString $framesString T:$trainIdDec
Data = 

//24:  Length   Session  TransID  Status   Cmd 24   SDSref
Resp = 19010000 01000000 00000000 00000000 02A00000 D2040000 
Resp = "28${trainIdDec}1-7-525"/32
Resp = "10000-7-525"/32
Resp = 00000000 ${lengthHex}000000
//08:  Flag	Message	Train	Length	Origin	stat10
Resp = 07	27	$trainIdHex	$lengthHex	00	$resultHex
$frames//	CHKSUM
Resp =	****
RespSize = 281
Ovar = #:-2:2
Wait = 500

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

