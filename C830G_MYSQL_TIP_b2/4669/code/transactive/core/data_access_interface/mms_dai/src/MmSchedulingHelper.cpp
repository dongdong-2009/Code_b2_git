/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/mms_dai/src/MmSchedulingHelper.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * MmSchedulingHelper is an object that is held by all type-specific alarm plan association objects.
  * It contains all data common across alarm plan associations, and manipulation
  * methods for the data. It helps avoid re-writing code for each object.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <vector>

#include "core/data_access_interface/mms_dai/src/MmSchedulingHelper.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.h"
#include "ace/OS.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"



#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{



    MmSchedulingHelper::MmSchedulingHelper( 
		unsigned long key, 
		unsigned long subsystemKey,
		time_t scheduledTime,
		time_t lastRunDate,
		const std::string& subName)
		: m_isValidData(true)

    {
		m_key = key; 
		m_subsystemKey = subsystemKey;
		m_scheduledTime = scheduledTime;
		m_lastRunDate = lastRunDate;
		m_subsystemName = subName;		

		m_subsystemKeyOrig = subsystemKey;
		m_subsystemNameOrig = subName;
		m_scheduledTimeOrig = scheduledTime;
        m_lastRunDateOrig = lastRunDate;
    }




	MmSchedulingHelper::MmSchedulingHelper( const MmSchedulingHelper& rhs)
		: m_isValidData(true)
    {
		m_key = rhs.m_key; 
		m_subsystemKey = rhs.m_subsystemKey;
		m_scheduledTime = rhs.m_scheduledTime;
		m_lastRunDate = rhs.m_lastRunDate;
		m_subsystemName = rhs.m_subsystemName;
    }


    MmSchedulingHelper::~MmSchedulingHelper()
    {
    }


    unsigned long MmSchedulingHelper::getKey()
    {
        return m_key;
    }

    
    std::string MmSchedulingHelper::getName()
    {
        if (!m_isValidData)  
        {            
            reload();
        }
        
        std::string name = "MMS scheduling for subsystem ";
		name += m_subsystemName;

        return name;
    }

    void MmSchedulingHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

	std::string MmSchedulingHelper::getSubsystemName()
    {
        FUNCTION_ENTRY("getSubsystemName");

        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_subsystemName;
    }

	unsigned long MmSchedulingHelper::getSubsystemKey()
    {
        FUNCTION_ENTRY("getSubsystemKey");

        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_subsystemKey;
    }

	time_t MmSchedulingHelper::getLastReported()
    {
        FUNCTION_ENTRY("getLastReported");

        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_lastRunDate;
    }

	time_t MmSchedulingHelper::getScheduledTime()
    {
        FUNCTION_ENTRY("getScheduledTime");

        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_scheduledTime;


    }


    void MmSchedulingHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // note that the api does not provide a way to create new records, nor does it allow the
		//entry of invalid data. Hence, no validation is required
     
        modifyExisting();

		m_subsystemKeyOrig = m_subsystemKey;
		m_subsystemNameOrig = m_subsystemName;
		m_scheduledTimeOrig = m_scheduledTime;
        m_lastRunDateOrig = m_lastRunDate;

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void MmSchedulingHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::MmsScheduling_Cd, TA_Base_Core::Read);

       /* std::ostringstream sql;
        sql << "select PHYSICALKEY, TO_CHAR(TIME_OF_DAY,'YYYYMMDDHH24MISS'), ";
		sql << " TO_CHAR(LAST_SENT,'YYYYMMDDHH24MISS')";
        sql << " from MM_SCHEDULING WHERE MMSCHE_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), MM_SCHEDULING_Oracle_SELECT_55052, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(MM_SCHEDULING_SELECT_55052, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, MM_SCHEDULING_SELECT_55052, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string subKeyColumn = "SUB_KEY";
        std::string timeColumn = "TIME_OF_DAY";
        std::string dateColumn = "LAST_SENT";


        std::vector<std::string> columnNames;
        columnNames.push_back(subKeyColumn);
        columnNames.push_back(timeColumn);
        columnNames.push_back(dateColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No record found for key = " << m_key;
			TA_THROW( TA_Base_Core::DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one record found for key = " << m_key;
			TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_subsystemKey = data->getUnsignedLongData( 0, subKeyColumn );
        m_scheduledTime = data->getDateData( 0, timeColumn );
        m_lastRunDate = data->getDateData( 0, dateColumn );
		m_subsystemName = MmSchedulingAccessFactory::getInstance().retrieveSubName(m_subsystemKey);
 
		m_subsystemKeyOrig = m_subsystemKey;
		m_subsystemNameOrig = m_subsystemName;
		m_scheduledTimeOrig = m_scheduledTime;
        m_lastRunDateOrig = m_lastRunDate;
		
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;        

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void MmSchedulingHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "record %lu already exists. It's details will be updated.", m_key );

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::MmsScheduling_Cd, TA_Base_Core::Write);

		//convert the m_scheduledTime to a string
		struct tm scheduledTimeStruct;
		char scheduledTimeString[22];

        if (ACE_OS::localtime_r(&m_scheduledTime, &scheduledTimeStruct) != NULL)
        {
	        sprintf(scheduledTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
				scheduledTimeStruct.tm_mday,
				scheduledTimeStruct.tm_mon + 1,
		        scheduledTimeStruct.tm_year + 1900,
				scheduledTimeStruct.tm_hour,
				scheduledTimeStruct.tm_min,
				scheduledTimeStruct.tm_sec);
        }

		//convert the m_lastRunDate to a string
		struct tm lastRunStruct;
		char lastRunString[22];

        if (ACE_OS::localtime_r(&m_lastRunDate, &lastRunStruct) != NULL)
        {
	        sprintf(lastRunString, "%02d/%02d/%04d %02d:%02d:%02d",
				lastRunStruct.tm_mday,
				lastRunStruct.tm_mon + 1,
		        lastRunStruct.tm_year + 1900,
				lastRunStruct.tm_hour,
				lastRunStruct.tm_min,
				lastRunStruct.tm_sec);
        }

       /* std::ostringstream sql;
        sql << "update MM_SCHEDULING set PHYSICALKEY = " << m_subsystemKey 
            << ", TIME_OF_DAY = TO_DATE('" << scheduledTimeString << "','dd/mm/yyyy hh24:mi:ss')"
            << ", LAST_SENT = TO_DATE('" << lastRunString << "','dd/mm/yyyy hh24:mi:ss')"
            << " where MMSCHE_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), MM_SCHEDULING_Oracle_UPDATE_55201, m_subsystemKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(MM_SCHEDULING_UPDATE_55201, m_subsystemKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, MM_SCHEDULING_UPDATE_55201, m_subsystemKey,
			scheduledTimeString, lastRunString, m_key);

        databaseConnection->executeModification(strSql);


        FUNCTION_EXIT;
    }


   


    void MmSchedulingHelper::setScheduledTime(time_t newTime)
	{
		FUNCTION_ENTRY("setScheduledTime");

		m_scheduledTime = newTime;

		FUNCTION_EXIT;
	}


    void MmSchedulingHelper::setLastReportedTime(time_t newTime)
	{
		FUNCTION_ENTRY("setLastReportedTime");

		m_lastRunDate = newTime;

		FUNCTION_EXIT;
	}

	bool MmSchedulingHelper::hasChanged()
	{
		FUNCTION_ENTRY("hasChanged");

		if(
			(m_subsystemKeyOrig != m_subsystemKey) ||
			(m_subsystemNameOrig != m_subsystemName) ||
			(m_scheduledTimeOrig != m_scheduledTime) ||
			(m_lastRunDateOrig != m_lastRunDate)
			)
		{
            // Something is different
			return true;
		}
        
        // Nothing has changed
		FUNCTION_EXIT;
		return false;
	}


     

} // closes TA_IRS_Core


