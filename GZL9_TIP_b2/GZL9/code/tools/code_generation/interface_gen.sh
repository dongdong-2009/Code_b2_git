#!/bin/sh

#
# The source code in this file is the property of 
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $Source: /cvs/1TS-9999-T/code/tools/code_generation/interface_gen.sh,v $
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

perl -I /cygdrive/c/9999_Sandbox/code/tools/code_generation c:/9999_Sandbox/code/tools/code_generation/interface_gen.pl $*

