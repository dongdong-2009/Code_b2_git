#!/usr/bin/perl

use Sys::Hostname;

use strict;

# get original profile

##open( PROFILE, "/u01/transactive/.profile" );
open( PROFILE, "$ENV{HOME}/.profile" );

my @allPrograms = <PROFILE>;

# write to new profile

open ( MYFILE, ">>$ENV{HOME}/profile.new" );
my $deli = "\n";

foreach my $prog (@allPrograms) {
	print MYFILE $prog
}
close ( PROF );

{
	print MYFILE $deli;
	print MYFILE "export LD_PRELOAD=libumem.so.1";
	print MYFILE $deli;
	print MYFILE "export UMEM_DEBUG=default";
	print MYFILE $deli;
	print MYFILE "export UMEM_LOGGING=transaction";
	print MYFILE $deli;
}
close (MYFILE);

# rename profile
system("mv /u01/transactive/.profile /u01/transactive/profile.old");
system("mv /u01/transactive/profile.new /u01/transactive/.profile");

system("chown transactive:scada /u01/transactive/.profile");
