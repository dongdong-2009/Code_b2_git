#
# The source code in this file is the property of 
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $Source: /cvs/1TS-9999-T/code/tools/code_generation/header_gen.pl,v $
# @author:  Daniel Hoey
# @version: $Revision: 1.1.2.2 $
#
# Last modification: $Date: 2003/05/02 07:32:23 $
# Last modified by:  $Author: danielh $
#
# This script generates a basic c++ header file for a giving class 
#
# Usage: header_gen.pl <class name> [<namespace1>]
#
#

use CPlusPlusTools;

GenerateIncludeFile(@ARGV);
