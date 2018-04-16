/**
* The source code in this file is the property of 
* MI Consulting Group (Australia) and is not for 
* redistribution in any form.
*
# Source:  $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/versioning/src/Version.h $ 
# Author:  Ripple
# Version: $Revision: #3 $
#
# Last modification: $DateTime: 2018/01/10 10:59:06 $
# Last modified by:  $Author: limin.zhu $
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
