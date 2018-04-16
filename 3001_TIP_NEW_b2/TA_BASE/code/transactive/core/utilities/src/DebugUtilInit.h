#ifndef DEBUGUTILINIT_H_INCLUDED
#define DEBUGUTILINIT_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/utilities/src/DebugUtilInit.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/11/19 17:09:45 $
  * Last modified by:  $Author: limin.zhu $
  * 
  */

#if defined( WIN32 )
    #pragma warning( disable : 4786 )
#endif

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{

    // Uses the following RunParams to set the corresponding DebugUtil settings:
    // RPARAM_DEBUGFILE
    // RPARAM_DEBUGLEVEL
    // RPARAM_DEBUGFILEMAXSIZE
    // RPARAM_DEBUGMAXFILES
    // RPARAM_DEBUGPIDFILENAMES
    // RPARAM_DEBUGLEVELFILE
    // RPARAM_DEBUGLEVELOFF

    void gSetDebugUtilFromRunParams();

} 

#endif // DEBUGUTILINIT_H_INCLUDED
