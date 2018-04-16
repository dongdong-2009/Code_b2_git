
#The source code in this file is the property of
#Ripple Systems and is not for redistribution
#in any form.
#
#Source:   $File: //depot/4669_T01271350/TA_BASE/tools/build/build.sh $ 
#Author:  Grace Koh
#Version: $Revision: #1 $
#
#Last modification: $DateTime: 2008/11/28 16:26:01 $
#Last modified by:  $Author: builder $

#!/bin/bash

# Modify these as required.
export ROOT_BASE="/disk2/tabuild1/Nightly_Build/DDBase_Build/Base"

# Prepare the environment.
source /disk2/tabuild1/.profile

# Execute the base build.
cd $ROOT_BASE
ant clean_all p4checkoutCode version build stage
