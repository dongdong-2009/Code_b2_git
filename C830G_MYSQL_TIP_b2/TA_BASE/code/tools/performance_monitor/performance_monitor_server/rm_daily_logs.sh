#!/usr/local/bin/perl

use Sys::Hostname;

use POSIX(strftime); 

use strict;


my $name=hostname;
my $dt = strftime("%Y%m%d",localtime(time()-86400));

unlink(glob("/tmp/log/*.$dt.csv"));
system("/usr/bin/find /tmp/log -type f \\( -name '*.csv' \\) -mtime 1 -exec rm -f {} \\;");
