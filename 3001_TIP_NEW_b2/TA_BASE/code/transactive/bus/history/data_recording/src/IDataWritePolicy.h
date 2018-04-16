#ifndef IDATA_WRITE_POLICY_H
#define IDATA_WRITE_POLICY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/history/data_recording/src/IDataWritePolicy.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Defines actions to take when the DataRecorder 
 * writes data.
 *
 */

#include "bus/history/data_recording/src/DataCache.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    class IDataWritePolicy
    {
    public:

        /**
         * Destructor
         */
        virtual ~IDataWritePolicy() {}


        /**
         * notifyDataWriteSucess
         *
         * Callback from the DataRecorder when data is written
         * successfully.
         *
         */
        virtual void notifyDataWriteSuccess() = 0;


        /**
         * notifyDataWriteError
         *
         * Callback from the DataRecorder when an error is
         * detected attempting to write data.
         *
         * @param   unwrittenCache     the data that the DataRecorder failed to write
         *
         */
        virtual void notifyDataWriteError( DataCachePtr& unwrittenCache ) = 0;

    };

    typedef boost::shared_ptr<IDataWritePolicy> IDataWritePolicyPtr;
}

#endif // IDATA_WRITE_POLICY_H
