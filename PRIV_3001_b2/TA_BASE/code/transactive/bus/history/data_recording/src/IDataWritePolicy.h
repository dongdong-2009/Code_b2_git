#ifndef IDATA_WRITE_POLICY_H
#define IDATA_WRITE_POLICY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/history/data_recording/src/IDataWritePolicy.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
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
