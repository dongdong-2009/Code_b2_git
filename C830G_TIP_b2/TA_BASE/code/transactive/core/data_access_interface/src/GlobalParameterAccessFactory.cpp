/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/GlobalParameterAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * GlobalParameterAccessFactory is a singleton that is used to retrieve GlobalParameter objects either from the
  * database or newly created. All GlobalParameter objects returned will adhear to the IGlobalParameter interface.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>

#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/data_access_interface/src/GlobalParameter.h"
#include "core/data_access_interface/src/ConfigGlobalParameter.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	GlobalParameterAccessFactory* GlobalParameterAccessFactory::m_instance = 0;

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

    GlobalParameterAccessFactory& GlobalParameterAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new GlobalParameterAccessFactory();
        }
        return *m_instance;
    }

    void GlobalParameterAccessFactory::getGlobalParameterBySql(const std::string& sql, std::vector< IGlobalParameter* >& globalParameters, const bool readWrite)
    {
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("GPARAM_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("VALUE");
        columnNames.push_back("SUBSYSTEMKEY");
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
						globalParameters.push_back( new ConfigGlobalParameter(i, *data) );
                    }
                    else
                    {
						globalParameters.push_back( new GlobalParameter(i, *data) );
                    }
                }
                catch (TA_Base_Core::DataException&)
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
    }

    std::vector<IGlobalParameter*> GlobalParameterAccessFactory::getAllGlobalParameters(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllGlobalParameters");

        // create the SQL string to retrieve all the GlobalParameters of the alarm
        std::ostringstream sql;
        sql << "select GPARAM_ID, NAME, VALUE, SUBSYSTEMKEY, "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM GLOBAL_PARAMETER "
            << "order by NAME";

		std::vector< IGlobalParameter* > globalParameters;
		getGlobalParameterBySql(sql.str(), globalParameters, readWrite);

        FUNCTION_EXIT;
		return globalParameters;
    }

    std::vector<IGlobalParameter*> GlobalParameterAccessFactory::getAllGlobalParametersForSubsystem(unsigned long subsystemKey, const bool readWrite)
    {
		FUNCTION_ENTRY("getAllGlobalParametersForSubsystem");

        // create the SQL string to retrieve all the GlobalParameters of the alarm
        std::ostringstream sql;
        sql << " select GPARAM_ID, NAME, VALUE, SUBSYSTEMKEY,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM GLOBAL_PARAMETER"
            << " where SUBSYSTEMKEY="<< subsystemKey
            << " order by NAME";

		std::vector< IGlobalParameter* > globalParameters;
		getGlobalParameterBySql(sql.str(), globalParameters, readWrite);

        FUNCTION_EXIT;
		return globalParameters;
    }
    
    IGlobalParameter* GlobalParameterAccessFactory::getGlobalParameterByKey( const unsigned long GlobalParameterKey, const bool readWrite )
    {
		FUNCTION_ENTRY("getGlobalParameterByKey");

        // create the SQL string to retrieve all the GlobalParameters of the alarm
        std::ostringstream sql;
        sql << " select GPARAM_ID, NAME, VALUE, SUBSYSTEMKEY,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM GLOBAL_PARAMETER"
            << " where GPARAM_ID="<< GlobalParameterKey;

		std::vector< IGlobalParameter* > globalParameters;
		getGlobalParameterBySql(sql.str(), globalParameters, readWrite);

        if ( 0 == globalParameters.size())
        {
            std::ostringstream message;
			message << "No data found for GroupParameter with key " << GlobalParameterKey;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == globalParameters.size(), "GlobalParameters key unique constraint violated");

        FUNCTION_EXIT;
		return globalParameters[0];
    }

    IGlobalParameter* GlobalParameterAccessFactory::getGlobalParameterByNameAndSubsystem( 
		const std::string& name, 
		unsigned long subsystemKey, 
		const bool readWrite)
    {
		FUNCTION_ENTRY("getGlobalParameterByNameAndSubsystem");

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		
        // create the SQL string to retrieve all the GlobalParameters of the alarm
        std::ostringstream sql;
        sql << " select GPARAM_ID, NAME, VALUE, SUBSYSTEMKEY,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM GLOBAL_PARAMETER"
            << " where UPPER(NAME) = '" << databaseConnection->escapeInsertString(toUpper(name)) << "'"
			<< " and SUBSYSTEMKEY=" << subsystemKey;

		std::vector< IGlobalParameter* > globalParameters;
		getGlobalParameterBySql(sql.str(), globalParameters, readWrite);

        if ( 0 == globalParameters.size())
        {
            std::ostringstream message;
			message << "No data found for GlobalParameter with Name = " << name << " and SubsystemKey " << subsystemKey;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == globalParameters.size(), "GlobalParameters name and subsystem unique constraint violated");

        FUNCTION_EXIT;
		return globalParameters[0];
    }


    unsigned long GlobalParameterAccessFactory::getGlobalParameterKeyFromNameAndSubsystem(
            const std::string& name, 
            unsigned long subsystemKey)
    {
        IGlobalParameter* parameter = NULL;
        unsigned long key = 0;
        try
        {
            parameter = getGlobalParameterByNameAndSubsystem(name, subsystemKey);
            key = parameter->getKey();
            delete parameter;
        }
        catch(...)
        {
            delete parameter;
            throw;
        }

        return key;
    }


    std::string GlobalParameterAccessFactory::getGlobalParameterValueFromNameAndSubsystem(
        const std::string& name, 
        unsigned long subsystemKey)
    {
        IGlobalParameter* parameter = NULL;
        std::string value;
        try
        {
            parameter = getGlobalParameterByNameAndSubsystem(name, subsystemKey);
            value = parameter->getValue();
            delete parameter;
        }
        catch(...)
        {
            delete parameter;
            throw;
        }

        return value;
    }


    IConfigGlobalParameter* GlobalParameterAccessFactory::createGlobalParameter()
    {
        return new ConfigGlobalParameter();
    }


    IConfigGlobalParameter* GlobalParameterAccessFactory::copyGlobalParameter(const IConfigGlobalParameter* GlobalParameterToCopy)
    {
        FUNCTION_ENTRY("copyGlobalParameter");

        TA_ASSERT(GlobalParameterToCopy !=  NULL, "The GlobalParameter to copy was NULL");
        
        const ConfigGlobalParameter* cast = dynamic_cast<const ConfigGlobalParameter*>(GlobalParameterToCopy);

        TA_ASSERT(cast != NULL, "GlobalParameter passed could not be converted into a ConfigGlobalParameter");

        FUNCTION_EXIT;
        return new ConfigGlobalParameter(*cast);
    }


} // closes TA_Base_Core


