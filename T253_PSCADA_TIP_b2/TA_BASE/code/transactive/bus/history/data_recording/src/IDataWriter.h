#ifndef IDATA_WRITER_H
#define IDATA_WRITER_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/history/data_recording/src/IDataWriter.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 * 
 * Interface for an object used by the DataRecorder to
 * persist cached data.
 *
 */

#include "bus/history/data_recording/src/DataCache.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    class IDataWriter
    {
    
    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~IDataWriter() {}


        /**
         * write
         *
         * Persists the data stored by a DataCache
         *
         * @param   data    the data to persist
         *
         * @exception   DataRecordingInternalException,
         *              if the Writer fails to persist 
         *              the data
         *
         */
        virtual void write( DataCachePtr& data ) = 0;

    };

    typedef boost::shared_ptr<IDataWriter> IDataWriterPtr;
}

#endif // IDATA_WRITER_H
