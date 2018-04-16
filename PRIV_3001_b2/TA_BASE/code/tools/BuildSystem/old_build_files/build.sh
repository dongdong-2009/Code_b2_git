
#The source code in this file is the property of
#Ripple Systems and is not for redistribution
#in any form.
#
#Source:   $File: //depot/PRIV_3001/TA_BASE/tools/BuildSystem/old_build_files/build.sh $ 
#Author:  Grace Koh
#Version: $Revision: #1 $
#
#Last modification: $DateTime: 2018/03/14 13:51:20 $
#Last modified by:  $Author: lim.cy $

#!/bin/bash

# Modify these as required.
export ROOT_BASE="/disk2/tabuild1/Nightly_Build/DDBase_Build/Base"

# Prepare the environment.
source /disk2/tabuild1/.profile

# Execute the base build.
cd $ROOT_BASE
ant clean_all p4checkoutCode version build stage
