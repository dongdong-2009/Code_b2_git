#
# The source code in this file is the property of 
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $Source: /cvs/1TS-9999-T/code/tools/code_generation/interface_gen.pl,v $
# @author:  Daniel Hoey
# @version: $Revision: 1.1.2.2 $
#
# Last modification: $Date: 2003/05/02 07:32:23 $
# Last modified by:  $Author: danielh $
#
# This script generates a header file for a class that realises (implements) any number of interfaces. Each interface
# must exist in a seperate file named "<interface_name>.h". The generated header file will be called "<class_name>.h".
#
# Usage: interface_gen.pl <class name> <namespace1> <path1><interface1> [<path2><interface2> .... <pathN><interfaceN>]
#


use CPlusPlusTools;

# Generate basic class' header file 
GenerateIncludeFile(@ARGV) or exit;

# Add each interface to the class' header file
my $i = 1;
while($i < @ARGV-1)
{
   AddInterface($ARGV[0], $ARGV[$i+1]);
   $i += 1;
}
    
