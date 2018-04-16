/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/SubsystemAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * SubsystemAccessFactory is a singleton that is used to retrieve Subsystem objects either from the
  * database or newly created. All Subsystem objects returned will adhear to the ISubsystem interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/SubsystemAccessFactory.h"

#include "core/data_access_interface/src/ConfigSubsystem.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	SubsystemAccessFactory* SubsystemAccessFactory::m_instance = 0;

    SubsystemAccessFactory& SubsystemAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new SubsystemAccessFactory();
        }
        return *m_instance;
    }

    void SubsystemAccessFactory::getSubsystemBySql(const std::string& sql, std::vector<ISubsystem*>& subsystems, const bool readWrite)
    {
        FUNCTION_ENTRY("getSubsystemBySql");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("IS_PHYSICAL");
        columnNames.push_back("EXCLUSIVE_CONTROL");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        if (0 == data->getUnsignedLongData( i, columnNames[0] ))
                        {
                            // if we have the subsystem where the key is 0 then we are not interested in it
                            // The user cannot configure this particular location
                            continue;
                        }

                        subsystems.push_back( new ConfigSubsystem(i, *data) );
                    }
                    else
                    {
                        subsystems.push_back( new Subsystem(i, *data) );
                    }
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
            delete data;
            data = NULL;
        }
        while ( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
    }


    std::vector<ISubsystem*> SubsystemAccessFactory::getAllSubsystems(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllSubsystems");

        // create the SQL string to retrieve all the Subsystems of the alarm
        std::ostringstream sql;
        sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS') "
			<< "from SUBSYSTEM";

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(sql.str(), subsystems, readWrite);

        FUNCTION_EXIT;
		return subsystems;
    }


    std::vector<ISubsystem*> SubsystemAccessFactory::getAllPhysicalSubsystems( const bool readWrite/*=false*/)
    {
        FUNCTION_ENTRY("getAllPhysicalSubsystems");

        // create the SQL string to retrieve all the Subsystems of the alarm
		std::ostringstream sql;
        sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS') "
			<< "from SUBSYSTEM where IS_PHYSICAL = 1";

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(sql.str(), subsystems, readWrite);

        FUNCTION_EXIT;
		return subsystems;
    }

    
    ISubsystem* SubsystemAccessFactory::getSubsystemByKey( const unsigned long key, const bool readWrite )
    {
        FUNCTION_ENTRY("getSubsystemByKey");

        // create the SQL string to retrieve all the Subsystems of the alarm
		std::ostringstream sql;
        sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS') "
			<< "from SUBSYSTEM where PKEY = " << key;

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(sql.str(), subsystems, readWrite);

        if ( 0 == subsystems.size())
        {
            std::ostringstream message;
			message << "No data found for Subsystem with Key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }
		
		TA_ASSERT(1 == subsystems.size(), "Subsystem key unique constraint violated");

        FUNCTION_EXIT;
		return subsystems[0];
    }

    std::vector<ISubsystem*> SubsystemAccessFactory::getSubsystemsByRegion( unsigned long regionKey, const bool readWrite )
    {
        FUNCTION_ENTRY("getSubsystemsAtLocation");

        // create the SQL string to retrieve all the Subsystems of the alarm
        std::ostringstream sql;

		sql << " select a.PKEY, a.NAME, a.IS_PHYSICAL, a.EXCLUSIVE_CONTROL,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
		    << " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from SUBSYSTEM a, SE_REG_SUB_V b"
		   	<< " where a.PKEY = b.PHYSICAL_SUBSYSTEM_KEY and b.SEREGI_ID = " << regionKey;

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(sql.str(), subsystems, readWrite);

        FUNCTION_EXIT;
		return subsystems;
    }


    IConfigSubsystem* SubsystemAccessFactory::createSubsystem()
    {
        return new ConfigSubsystem();
    }


    IConfigSubsystem* SubsystemAccessFactory::copySubsystem(
        const IConfigSubsystem* subsystemToCopy)
    {
        FUNCTION_ENTRY("copySubsystem");

        TA_ASSERT(subsystemToCopy !=  NULL, "The subsystem to copy was NULL");
        
        const ConfigSubsystem* cast =
            dynamic_cast<const ConfigSubsystem*>(subsystemToCopy);

        TA_ASSERT(cast != NULL,
                  "Subsystem passed could not be converted into a "
                  "ConfigSubsystem");

        FUNCTION_EXIT;
        return new ConfigSubsystem(*cast);
    }



} // closes TA_Base_Core


