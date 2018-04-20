#!/usr/bin/perl
#
# This will generate a Train PEC messages simulator cases that can be loaded in the radio simulator
# It is used to customise the train id, pec id etc.

use strict;
use Switch;
use Getopt::Long;

my $trainId = 0;
my $carNumber = 0;
my $pecNumber = 0;
my $outputFile = "";


GetOptions( "trainId=i"=>\$trainId,
            "carNumber=i"=>\$carNumber,
            "pecNumber=i"=>\$pecNumber,
            "file=s"=>\$outputFile );


if ( ( $trainId == 0 ) ||
     ( $carNumber == 0 ) ||
     ( $pecNumber == 0 ) )
{
    print "Usage: ./TrainPec.pl --trainId=X --carNumber=X --pecNumber=X --file=<Output File Name>";
    exit;
}

my $trainIdDec = sprintf( "%02d", $trainId );
my $trainIdHex = sprintf( "%02X", $trainId );

my $template = <<TEMPLATE_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

Name = UC-PEC-01.1 (/53) PEC Activated T:$trainIdDec PEC:${carNumber}/${pecNumber}
//                           1                      2                     3          
//     0 1 2 3  4 5 6 7  8 9 0 1  2 3 4 5  6 7 8 9  0 1 2 3  4 5 6 7  8 9 0 1   2 3 4 5 6 7 8 9
Data = 

//24:  Length   Session  TransID  Status   Cmd 24   SDSref
Resp = 19010000 01000000 00000000 00000000 02A00000 D2040000 
Resp = "28${trainIdDec}1-7-525"/32
Resp = "10000-7-525"/32
Resp = 00000000 08000000
//08:  Flag	Message	Train	Length	Origin	PEC	CHKSUM
Resp = 07	35	$trainIdHex	08	00	${carNumber}${pecNumber}	****
RespSize = 281
Ovar = #:-2:2
Wait = 0


Name = UC-PEC-02.2 (/54) PEC Driver Selection T:$trainIdDec PEC:${carNumber}/${pecNumber}
//                           1                      2                     3          
//     0 1 2 3  4 5 6 7  8 9 0 1  2 3 4 5  6 7 8 9  0 1 2 3  4 5 6 7  8 9 0 1   2 3 4 5 6 7 8 9
Data = 

//24:  Length   Session  TransID  Status   Cmd 24   SDSref
Resp = 19010000 01000000 00000000 00000000 02A00000 D2040000 
Resp = "28${trainIdDec}1-7-525"/32
Resp = "10000-7-525"/32
Resp = 00000000 08000000
//08:  Flag	Message	Train	Length	Origin	PEC	CHKSUM
Resp = 07	36	$trainIdHex	08	00	${carNumber}${pecNumber}	****
RespSize = 281
Ovar = #:-2:2
Wait = 0


Name = UC-PEC-02.3 (/52) PEC Reset by Driver T:$trainIdDec PEC:${carNumber}/${pecNumber}
//                           1                      2                     3          
//     0 1 2 3  4 5 6 7  8 9 0 1  2 3 4 5  6 7 8 9  0 1 2 3  4 5 6 7  8 9 0 1   2 3 4 5 6 7 8 9
Data = 

//24:  Length   Session  TransID  Status   Cmd 24   SDSref
Resp = 19010000 01000000 00000000 00000000 02A00000 D2040000 
Resp = "28${trainIdDec}1-7-525"/32
Resp = "10000-7-525"/32
Resp = 00000000 08000000
//08:  Flag	Message	Train	Length	Origin	PEC	CHKSUM
Resp = 07	34	$trainIdHex	08	00	${carNumber}${pecNumber}	****
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

