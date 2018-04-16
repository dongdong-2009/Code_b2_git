#ifndef IDATA_RECORDER_DB_ACCESS_H
#define IDATA_RECORDER_DB_ACCESS_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/history/data_recording/src/IDataRecorderDbAccess.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Interface for an object used by the DataRecorder to
 * retrieve data recording objects from the database.
 *
 */

#include <vector>

#include "core/data_access_interface/data_recording_dai/src/IRecordableData.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableItem.h"


namespace TA_Base_Bus
{
    class IDataRecorderDbAccess
    {
    
    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~IDataRecorderDbAccess() {}


        /**
         * getRecordableDataByType
         *
         * Retrieves data recording objects from the database.
         *
         * @param   typeKey    Type key of the entities that form the data recording objects.
         *
         */
        virtual std::vector<TA_Base_Core::IRecordableDataPtr> getRecordableDataByType( unsigned long typeKey ) = 0;

		
		/**
         * getRecordableItemByType
         *
         * Retrieves the items from the database that have trending enabled.
         *
         * @param   typeKey    Type key of the entities that form the data recording objects.
         *
         */
        virtual std::vector<TA_Base_Core::IRecordableItemPtr> getRecordableItemsByType( unsigned long typeKey ) = 0;

    };
    
    typedef boost::shared_ptr<IDataRecorderDbAccess> IDataRecorderDbAccessPtr;
}

#endif // IDATA_RECORDER_DB_ACCESS_H
