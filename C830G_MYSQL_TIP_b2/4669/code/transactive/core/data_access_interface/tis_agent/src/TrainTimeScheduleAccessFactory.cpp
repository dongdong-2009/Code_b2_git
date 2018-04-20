/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleAccessFactory.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * TrainTimeScheduleAccessFactory is a singleton that is used to generate and retrieve 
  * ITrainTimeSchedule objects.
  */

#pragma warning ( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleData.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleTable.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    TrainTimeScheduleAccessFactory* TrainTimeScheduleAccessFactory::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable TrainTimeScheduleAccessFactory::m_singletonLock;

	TrainTimeScheduleAccessFactory& TrainTimeScheduleAccessFactory::getInstance()
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
			
            m_theClassInstance = new TrainTimeScheduleAccessFactory();
		}
		
		return *m_theClassInstance;
	}

    //
    // getTrainTimeSchedules
    //
	std::vector<ITrainTimeSchedule*> TrainTimeScheduleAccessFactory::getTrainTimeSchedules()
    {
         return getTrainTimeSchedulesWhere();
    }

    ITrainTimeSchedule* TrainTimeScheduleAccessFactory::createNewTrainTimeSchedule()
    {
        // create a new schedule and return it
        TrainTimeScheduleData* newSchedule = new TrainTimeScheduleData();
        return newSchedule;
    }


    ITrainTimeSchedule* TrainTimeScheduleAccessFactory::copyTrainTimeSchedule(ITrainTimeSchedule* originalSchedule)
    {
        TrainTimeScheduleData* oldSchedule = dynamic_cast<TrainTimeScheduleData*>(originalSchedule);

        TA_ASSERT(oldSchedule != NULL, "Copying objects other than TrainTimeScheduleData not supported");

        TrainTimeScheduleData* newSchedule = new TrainTimeScheduleData(*oldSchedule);

        return newSchedule;
    }


	ITrainTimeSchedule* TrainTimeScheduleAccessFactory::getTrainTimeSchedule( unsigned long key )
	{
		std::stringstream whereSQL;
        whereSQL << "where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN
            << " = " << key;

		std::vector<ITrainTimeSchedule*> trainTimeSchedules; // There should be no more than one

		try
		{
			trainTimeSchedules = getTrainTimeSchedulesWhere(whereSQL.str());
		}
		catch ( const TA_Base_Core::DataException& )
		{
			std::stringstream error;
            error << "No train time schedules exist in the database with the specified key";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
		}

		return trainTimeSchedules[0];		
	}

	std::vector<ITrainTimeSchedule*> TrainTimeScheduleAccessFactory::getTrainTimeSchedulesWhere( std::string whereSQL /* = "" */ )
	{
     TA_Base_Core::IDatabase* databaseConnection = 
         TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // Get all the TrainTimeSchedule primary keys that exist in the table.

        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;
		selectSql << " " << whereSQL;

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( selectSql.str(), columnNames );
        
        if ( 0 == data->getNumRows() )
        {
            std::stringstream error;
            error << "There are no Train Time Schedules in the database table ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;

            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to ITrainTimeSchedules.
        std::vector< ITrainTimeSchedule* > trainTimeSchedules;

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        trainTimeSchedules.push_back( new TrainTimeScheduleData( 
				        data->getUnsignedLongData( i, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN ) ) );
		        }
            
                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return trainTimeSchedules;
	}

} //end namespace TA_IRS_Core


