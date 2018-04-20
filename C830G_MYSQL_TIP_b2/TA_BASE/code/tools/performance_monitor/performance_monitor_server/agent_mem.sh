#!/usr/local/bin/perl

# verison 1.1
# to add oracle process monitoring
# date 2010-03-31

use Sys::Hostname;

use POSIX(strftime); 

use strict;

$ENV{HOME}="/u01/transactive";

my $opt = shift @ARGV;

use vars qw( %agents $count $version);

$version="";
    
# get the apps we are looking for
my $name=hostname;
open( VERSION, "$ENV{HOME}/bin/versions.$name" );

my @allPrograms = <VERSION>;
$count = 0;
foreach my $prog (@allPrograms)
{
    # if its the major version tag
	if($prog=~/.*Version.*/)
	{
		$prog=~/.*:(.*)/;
        $version=$1;
        $version=~s/ //g;
    }
    else
    {
        # its an agent
        # get the information and add it to the list of agents
        my @row = split(/:/, $prog);
        if (@row == 3)
        {
            $agents{$count}{name} = $row[0];
            $agents{$count}{version} = $version.".".$row[1];
            $agents{$count}{pattern} = $row[2];
            $agents{$count}{pattern} =~ s/\r//g;
            $agents{$count}{pattern} =~ s/\n//g;
            $agents{$count}{pid} = "";
            $agents{$count}{runningSince} = "Not Running";
                            
            $count++;
        }
    }
}

close ( VERSION );

#
# Display list of all Agents and there Process ID if running
#
open( ALLPROG, "/usr/ucb/ps -auxww | egrep '(transact)|(root )' |" );

my @allPrograms = <ALLPROG>;
foreach my $prog (@allPrograms)
{
	my $temp;
	my $tempTime;
	my $tempPid;
	my $tempMem;
	my $cpu;

	#we need the 2 column of ps for the PID and the 5th column for the "running since"
	my @psinfo = split(/\s+/,$prog);
	$tempPid = $psinfo[1];
	$cpu = $psinfo[2];
	$tempMem = $psinfo[3];
	$tempTime = $psinfo[6]; 

	#--- Memory Usage Start
	# The rest of this script makes the assumption that mem%, sz, and rss are merged so I will to!

	# $tempMem originally contains <MEM%><SZ><RSS>.

	$tempMem =~ s/^[0-9]+\.[0-9]//;
	# $tempMem now contains <SZ><RSS>.

	$temp = length( $tempMem );
	if ( $temp%2 == 1 )
	{
		# Uneven number of chars so give the extra one to the SZ as it is
		# always > RSS
		$temp = ( $temp/2 )+1;
	}
	else
	{
		$temp = $temp/2;
	}
	# $temp now contains the length of SZ.
	
	$tempMem = substr( $tempMem, 0, $temp );
	# $tempMem now contains <SZ>

	# Convert to nicer numbers.
	if ( $tempMem >= 1024 )
	{
		$tempMem = int( $tempMem/1024 );
		$tempMem = $tempMem . " Mb";
	}
	else
	{
		$tempMem = $tempMem . " Kb";
	}

	#--- Memory Usage End

	#if the time doesnt contain ':' then it has run for 
	#longer than 24hrs and is contained in two words
	if($tempTime !~ /.*:.*/)
	{
		my $temp;
		my $intermediateTempTime = $psinfo[7];
		$tempTime = "$tempTime $intermediateTempTime";
	}
	else
	{
		my $nowString = localtime;
		my @nowArray = split(/\s+/,$nowString);
		shift @nowArray;
		shift @nowArray;
		shift @nowArray;
		$nowString = shift @nowArray;
		#to tell if the time is yesterday or today we must compare hours to hours etc
		@nowArray = split (/:/,$nowString);
		my @psTime = split (/:/,$tempTime);
		my $day="Today";
		my $i;
		for ($i=0; $i < 3; ++$i)
		{
			my $timeComponent = pop @nowArray;
			my $psTimeComponent = pop @psTime;
			
			if ($timeComponent > $psTimeComponent)
			{
				$day = "Today";
			}
			elsif ($timeComponent < $psTimeComponent)
			{
				$day = "YesterDay"
			}
		}
		
		$tempTime = "$day $tempTime";
		
	}

	#
	# See if it matches any of our processes
	#
	for (my $key = 0; $key < $count; ++$key)
	{
	    my $regex = $agents{$key}{pattern};
	    if( $prog =~ $regex)
	    {
		$agents{$key}{pid} = "$tempPid";
		$agents{$key}{runningSince} = "$tempTime";
		$agents{$key}{mem} = "$tempMem";
		$agents{$key}{cpu} = "$cpu";
	    }
	}	
}

# use the POSIX module to access only the function
# strftime - to format a date nicely
# and use it right-away to format current time
# as MM/DD/YY HH:MI
# $today=strftime("%m/%d/%y %H:%M",localtime());

my $p2 = "/tmp/log";
if (-d $p2){
}
else {
	mkdir $p2 or die;
	system ("chmod go+rwx /tmp/log");
}
##my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);
##printf "%4d-%02d-%02d %02d:%02d:%02d\n",$year+1900,$mon+1,$mday,$hour,$min,$sec;
my $dt = strftime("%Y%m%d",localtime());
open (MYFILE, ">>/tmp/log/$name.$dt.csv");

my $currdate=localtime;
print MYFILE $currdate;
print MYFILE "\n";
##my $deli = "\t";
my $deli = ",";

for (my $key = 0; $key < $count; ++$key)
{
  	print MYFILE $agents{$key}{name};
	##my $name=$agents{$key}{name};
	##my $namelen = length( $name );
	##my $i=$namelen%8;
	print MYFILE $deli;
	print MYFILE $agents{$key}{pid};
	print MYFILE $deli;
	print MYFILE $agents{$key}{version};
	print MYFILE $deli;
	print MYFILE $agents{$key}{cpu};
	print MYFILE $deli;
	print MYFILE $agents{$key}{mem};
	print MYFILE $deli;
	print MYFILE $agents{$key}{runningSince};
	print MYFILE "\n";
}

open ( ORAP, "/usr/ucb/ps -auxww | egrep 'ora_' |" );

my @allOras = <ORAP>;
foreach my $ora (@allOras){
	
	my $oraName;
	my $oraTime;
	my $oraPid;
	my $oraMem;
	my $oraCpu;

	my $temp;
	
   if($ora !~ /.*egrep.*/)
   {

	#we need the 2 column of ps for the PID and the 5th column for the "running since"
	my @oraPsinfo = split(/\s+/,$ora);
	
	$oraPid = $oraPsinfo[1];
	$oraCpu = $oraPsinfo[2];
	$oraMem = $oraPsinfo[3];
	$oraTime = $oraPsinfo[6]; 
	if($oraTime !~ /.*:.*/)
	{
		my $otherTime = $oraPsinfo[7];
		$oraTime = "$oraTime $otherTime";
		$oraName = $oraPsinfo[9];
	}
	else {
		$oraName = $oraPsinfo[8];
	}

	#--- Memory Usage Start
	# The rest of this script makes the assumption that mem%, sz, and rss are merged so I will to!

	# $oraMem originally contains <MEM%><SZ><RSS>.
	
	$oraMem =~ s/^[0-9]+\.[0-9]//;
	# $tempMem now contains <SZ><RSS>.

	$temp = length( $oraMem );
	if ( $temp%2 == 1 )
	{
		# Uneven number of chars so give the extra one to the SZ as it is
		# always > RSS
		$temp = ( $temp/2 )+1;
	}
	else
	{
		$temp = $temp/2;
	}
	# $temp now contains the length of SZ.
	
	$oraMem = substr( $oraMem, 0, $temp );
	# $oraMem now contains <SZ>

	# Convert to nicer numbers.
	if ( $oraMem >= 1024 )
	{
		$oraMem = int( $oraMem/1024 );
		$oraMem = $oraMem . " Mb";
	}
	else
	{
		$oraMem = $oraMem . " Kb";
	}

	#--- Memory Usage End

	print MYFILE $oraName;
	print MYFILE $deli;
	print MYFILE $oraPid;
	print MYFILE $deli;
	print MYFILE " ";
	print MYFILE $deli;
	print MYFILE $oraCpu;
	print MYFILE $deli;
	print MYFILE $oraMem;
	print MYFILE $deli;
	print MYFILE $oraTime;
	print MYFILE "\n";
    }
}

## free memory and cpu

open ( FREE_MEM, "/usr/local/bin/top 0 |egrep 'Memory' |" );
my @freeMems = <FREE_MEM>;
my $freeMem = @freeMems[0];

my @freeMemInfos = split(',', $freeMem);
my $freeMemInfo = @freeMemInfos[1];

my $freeMemNum = substr $freeMemInfo, 0, 4;
my $freeMemFinal = $freeMemNum . " Mb";

open ( FREE_CPU, "vmstat" );
my @freeCpus = <FREE_CPU>;
my $freeCpu = @freeCpus[2];

my @freeCpuInfos = split(/\s+/ , $freeCpu);
my $freeCpuFinal = (@freeCpuInfos[length(@freeCpuInfos) - 1])/100;

my $datetime = strftime("%H:%M:%S",localtime());

print MYFILE "Free";
print MYFILE $deli;
print MYFILE "0000";
print MYFILE $deli;
print MYFILE " ";
print MYFILE $deli;
print MYFILE $freeCpuFinal;
print MYFILE $deli;
print MYFILE $freeMemFinal;
print MYFILE $deli;
print MYFILE $datetime;
print MYFILE "\n";

close (MYFILE);

system("chmod go+rw /tmp/log/*.csv");

