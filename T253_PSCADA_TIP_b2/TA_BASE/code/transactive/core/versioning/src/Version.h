/**
* The source code in this file is the property of 
* MI Consulting Group (Australia) and is not for 
* redistribution in any form.
*
# Source:  $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/versioning/src/Version.h $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2018/02/09 09:07:12 $
# Last modified by:  $Author: Ouyang $
* 
* Main source file for MiVersion information
*
* NOTE: The variables declared external in Version.cpp
* are defined in the file VersionGen.cpp which is located in
* all component src directories. The variables are resolved
* at link time.
*/
#ifndef VERSION_H
#define VERSION_H

namespace TA_Base_Core
{
    class Version
    {
        public:

        /**
          * displayVersionInfo()
          *
          * This is a static method that will print out versionInformation to stdout.
          */
         static void displayVersionInfo();
         static void checkCommandLine(int argc, char* argv[]);
    };
}

#endif //#ifndef VERSION_H
