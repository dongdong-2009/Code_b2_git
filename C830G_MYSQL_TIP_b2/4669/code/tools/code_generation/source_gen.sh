#!/bin/sh

#
# The source code in this file is the property of 
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $Source: /cvs/1TS-9999-T/code/tools/code_generation/source_gen.sh,v $
# @author:  Daniel Hoey
# @version: $Revision: 1.1.2.2 $
#
# Last modification: $Date: 2003/05/02 07:32:23 $
# Last modified by:  $Author: danielh $
#
# This script generates method stubs for a c++ source file from a c++ header file.
#
# Usage: source_gen.pl <class name>
#    note: a header file with the name <class name>.h must exist
#

perl -I /cygdrive/c/9999_Sandbox/code/tools/code_generation c:/9999_Sandbox/code/tools/code_generation/source_gen.pl $1

