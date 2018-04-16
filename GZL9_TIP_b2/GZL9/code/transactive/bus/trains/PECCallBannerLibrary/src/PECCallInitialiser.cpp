/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2012/06/12 $
  * Last modified by:  $Author: builder $
  *
  */


#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallInitialiser.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    PECCallInitialiser::PECCallInitialiser( PECCallEntry& pecCallEntry )
            : m_PECCallEntry( pecCallEntry )
    {
        FUNCTION_ENTRY( "PECCallInitialiser( PECCallEntry& )" );
        FUNCTION_EXIT;
    }



    PECCallInitialiser::~PECCallInitialiser()
    {
        FUNCTION_ENTRY( "~PECCallInitialiser" );
        FUNCTION_EXIT;
    }


    void PECCallInitialiser::run()
    {
        FUNCTION_ENTRY( "run" );

        // call threadedAnswer on the call entry
        m_PECCallEntry.threadedAnswer();

        FUNCTION_EXIT;
    }


    void PECCallInitialiser::terminate()
    {
        FUNCTION_ENTRY( "terminate" );
        FUNCTION_EXIT;
    }

}