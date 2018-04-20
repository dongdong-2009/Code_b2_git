/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/tis_agent/src/StisTimeScheduleAccessFactory.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  * 
  * StisTimeScheduleAccessFactory is a singleton that is used to generate and retrieve 
  * StisTimeScheduleData objects.
  */

#pragma warning ( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/StisTimeScheduleData.h"
#include "core/data_access_interface/tis_agent/src/StisTimeScheduleAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/StisTimeScheduleTable.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    StisTimeScheduleAccessFactory* StisTimeScheduleAccessFactory::m_theClassInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable StisTimeScheduleAccessFactory::m_singletonLock;

	StisTimeScheduleAccessFactory& StisTimeScheduleAccessFactory::getInstance()
	{
        TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if ( 0 == m_theClassInstance )
		{
			// Create the one & only object
			
            m_theClassInstance = new StisTimeScheduleAccessFactory();
		}
		
		return *m_theClassInstance;
	}

	void StisTimeScheduleAccessFactory::removeInstance()
	{
        TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if ( 0 != m_theClassInstance )
		{
			delete m_theClassInstance;
			m_theClassInstance = 0;
		}
	}
    //
    // getStisTimeSchedules
    //
	std::vector<StisTimeScheduleData*> StisTimeScheduleAccessFactory::getStisTimeSchedules()
    {
         return getStisTimeSchedulesWhere();
    }

	StisTimeScheduleData* StisTimeScheduleAccessFactory::getStisTimeSchedule( unsigned long key )
	{
		std::stringstream whereSQL;
        whereSQL << "where " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_ID_COLUMN << " = " << key;

		std::vector<StisTimeScheduleData*> stisTimeSchedules; // There should be no more than one

		try
		{
			stisTimeSchedules = getStisTimeSchedulesWhere(whereSQL.str());
		}
		catch ( const TA_Base_Core::DataException& )
		{
			std::stringstream error;
            error << "No stis time schedules exist in the database with the specified key";
            error << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
		}

		return stisTimeSchedules[0];		
	}

	std::vector<StisTimeScheduleData*> StisTimeScheduleAccessFactory::getStisTimeSchedulesWhere( std::string whereSQL /* = "" */ )
	{
     TA_Base_Core::IDatabase* databaseConnection = 
         TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // Get all the stisTimeSchedule primary keys that exist in the table.
		//delete by lin
        //std::stringstream selectSql;
        //selectSql << "select " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_ID_COLUMN;
        //selectSql << " from " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME;
		//selectSql << " " << whereSQL;

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it a std::auto_ptr so it is automatically cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_ID_COLUMN );

        //TA_Base_Core::IData* data = databaseConnection->executeQuery( selectSql.str(), columnNames );
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		
		databaseConnection->prepareSQLStatement(strSql, TI_STIS_TIME_SCHEDULE_SELECT_80001, whereSQL);
			
		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames );

        
        if ( 0 == data->getNumRows() )
        {
            std::stringstream error;
            error << "There are no stis Time Schedules in the database table ";
            error << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME;

            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to StisTimeScheduleDatas.
        std::vector< StisTimeScheduleData* > stisTimeSchedules;

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        stisTimeSchedules.push_back( new StisTimeScheduleData( 
				        data->getUnsignedLongData( i, TA_TISAgentDAI::STIS_TIME_SCHEDULE_ID_COLUMN ) ) );
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

        return stisTimeSchedules;
	}

} //end namespace TA_IRS_Core

