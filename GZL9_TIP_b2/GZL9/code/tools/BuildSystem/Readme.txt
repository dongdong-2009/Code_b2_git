The autobuild.pl script checks out everything in the 'code' module of the
repository in the CVSROOT environment variable. It then runs build.pl on the
checked out code, with the specified build file.

nightlyBuild.pl is a modified autobuild.pl
run it as a scheduled task on windows with runNightlyBuild.bat
run it as a cron job on solaris with runNightlyBuild.sh


It is a modified version of the ITS autobuild.pl script

Instructions
1. Ensure the CVSROOT environment variable is set
	eg :pserver:username@cvs:/cvs/1TS-9999-T
	This can be done in windows by running config.bat,
	This also runs vcvars32.bat
	
2. To autobuild:
	on solaris
	perl autobuild.pl -r -c -b CODE_4669 SolarisBuildfile.txt
	
	on windows
	perl autobuild.pl -r -c -b CODE_4669 WindowsBuildfile.txt
	
	Or more generic
	perl autobuild.pl -r -c -b <TAG_NAME> <build file>
	
5. The repostory will be checked out to the CWD under 'code' and built

6. An email will be sent on completion to the email address specified in
   the top of the autobuild.pl file
   
The Build.pl script is a slightly version of the ITS Build.pl file.
Instead of using fixed directory and file locations, its all set in one file,
the buildfile. The buildfile specifies these things, in order.

1. ProjectInformation
	This just sets the code base directory (where app, bus, core, cots,
	and tools are)
2. A list of the relevant BuildEnvironment declarations.
	It is advised to split the buildfile into each target environment,
	and only have one of these in each (same format as the ITS .cfg files)
3. A list of the Make targets
	This is the same as the ITS system. Although, only the top level
	apps/agents need to be specified here, since the new makefile system
	recursively satisfies dependencies, order is therefore not important.
	there is now also the option to specify a build target.
