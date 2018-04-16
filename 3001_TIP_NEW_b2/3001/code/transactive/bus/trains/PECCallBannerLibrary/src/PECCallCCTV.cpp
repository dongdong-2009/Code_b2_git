/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Lucky Cuizon
  * @version: $Revision: #2 $
  *
  * Last modification: $Date: 2013/26/06 $
  * Last modified by:  $Author: $
  *
  */


#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallCCTV.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    PECCallCCTV::PECCallCCTV( PECCallEntry& pecCallEntry )
            : m_PECCallEntry( pecCallEntry )
    {
        FUNCTION_ENTRY( "PECCallCCTV( PECCallEntry& )" );
        FUNCTION_EXIT;
    }



    PECCallCCTV::~PECCallCCTV()
    {
        FUNCTION_ENTRY( "~PECCallCCTV" );
        FUNCTION_EXIT;
    }


    void PECCallCCTV::run()
    {
        FUNCTION_ENTRY( "run" );

        // call threadedAnswer on the call entry
        m_PECCallEntry.threadedCCTV();

        FUNCTION_EXIT;
    }


    void PECCallCCTV::terminate()
    {
        FUNCTION_ENTRY( "terminate" );
        FUNCTION_EXIT;
    }

}