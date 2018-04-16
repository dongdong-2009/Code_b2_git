/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/history/data_recording/src/ExceptionOnWriteErrorPolicy.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
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
