/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/history/data_recording/src/ExceptionOnWriteErrorPolicy.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation of IDataWritePolicy that throws
 * a DataRecordingException when an error occurs.
 *
 */

#include <sstream>
#include <vector>

#include "bus/history/data_recording/src/ExceptionOnWriteErrorPolicy.h"
#include "bus/history/data_recording/src/DataRecordingException.h"
#include "bus/history/data_recording/src/Datum.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
    ExceptionOnWriteErrorPolicy::ExceptionOnWriteErrorPolicy()
    {
        // Nothing
    }


    ExceptionOnWriteErrorPolicy::~ExceptionOnWriteErrorPolicy()
    {
        // Nothing
    }


    void ExceptionOnWriteErrorPolicy::notifyDataWriteSuccess()
    {
        // Nothing
    }


    void ExceptionOnWriteErrorPolicy::notifyDataWriteError( DataCachePtr& unwrittenCache )
    {
        FUNCTION_ENTRY( "notifyDataWriteError" );
        std::stringstream errorMessage;
        errorMessage << "Failed to write " << unwrittenCache->getSize() 
                     << " items from DataCache '" << unwrittenCache->getType() << "'";

        TA_THROW( TA_Base_Core::DataRecordingException( errorMessage.str() ) );
        FUNCTION_EXIT;
    }
}
