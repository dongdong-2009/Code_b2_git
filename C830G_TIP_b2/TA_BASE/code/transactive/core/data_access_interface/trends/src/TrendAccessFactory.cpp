/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/trends/src/TrendAccessFactory.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * TrendAccessFactory is a singleton that is used to retrieve Trend objects either from the
  * database or newly created. All Trend objects returned will adhere to the ITrend interface.
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable:4786)
#endif // _MSC_VER

#include <sstream>

#include "core/data_access_interface/trends/src/TrendAccessFactory.h"
#include "core/data_access_interface/trends/src/Trend.h"
#include "core/data_access_interface/trends/src/ConfigTrend.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    TrendAccessFactory::TrendAccessFactory()
    {
    }


    TrendAccessFactory& TrendAccessFactory::getInstance()
    {
        static TrendAccessFactory factory;
        return factory;
    }


    ITrend* TrendAccessFactory::getTrend(const std::string& name, const bool readWrite)
    {
		// Get a specific Trend
		FUNCTION_ENTRY("TrendAccessFactory::getTrend");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

		// Create the query
        std::ostringstream query;
		query << "SELECT NAME FROM TREND_TEMPLATE where NAME = '" << name << "'";

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string trendName("NAME");
        columnNames.push_back(trendName);

		IData* data = databaseConnection->executeQuery(query.str(), columnNames);

	    // Create the vector to return
        std::vector<ITrend*> trends;

 	    if (data->getNumRows() == 0) // No trends found
        {
            // clean up the pointer
            delete data;
            data = NULL;

			TA_THROW( DataException( "Trend not found", DataException::NO_VALUE, query.str().c_str() ) );
        }	
		if (data->getNumRows() > 1)
		{
			delete data;
			data = NULL;

			TA_THROW( DataException( "More than one Trend found", DataException::NOT_UNIQUE, query.str().c_str() ) );
		}

		delete data;
		data = NULL;

		FUNCTION_EXIT;

        if (readWrite)
        {
            return new ConfigTrend( name );
        }

		return new Trend(name); // TODO
    }
 

	std::vector<ITrend*> TrendAccessFactory::getAllTrends(const bool readWrite)
    {
		FUNCTION_ENTRY("TrendAccessFactory::getAllTrends");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

		// Create the query
		std::string sql = "SELECT NAME FROM TREND_TEMPLATE";

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string trendName("NAME");
        columnNames.push_back(trendName);

		IData* data = databaseConnection->executeQuery(sql, columnNames);

	    // Create the vector to return
        std::vector<ITrend*> trends;

 	    if (data->getNumRows() == 0) // No trends found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return trends;
        }

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getStringData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        trends.push_back( new ConfigTrend(data->getStringData( i, trendName )) );
                    }
                    else
                    {
                        trends.push_back( new Trend( data->getStringData( i, trendName ) ) );
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
        while (databaseConnection->moreData(data));

		FUNCTION_EXIT;

        // Return the vector
        return trends;
    }


    std::map<unsigned long, std::string> TrendAccessFactory::getAllSamplePeriods()
    {
        FUNCTION_ENTRY("TrendAccessFactory::getAllSamplePeriods");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

		// Create the query
		std::string sql = "SELECT DISTINCT interval,interval_seconds FROM DR_TREND_CONFIG ORDER BY interval_seconds";

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string samplePeriodName("NAME");
        std::string samplePeriodValue("VALUE");
        columnNames.push_back(samplePeriodName);
        columnNames.push_back(samplePeriodValue);

		IData* data = databaseConnection->executeQuery(sql, columnNames);

	    // Create the map to return
        std::map<unsigned long, std::string> samplePeriods;

 	    if (data->getNumRows() == 0) // No sample periods found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return samplePeriods;
        }

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getStringData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    samplePeriods.insert(std::map<unsigned long, std::string>::value_type(data->getUnsignedLongData(i, samplePeriodValue), data->getStringData(i, samplePeriodName)));
                    //samplePeriods[data->getUnsignedLongData(i, samplePeriodValue)] = data->getUnsignedLongData(i, samplePeriodName);
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
        while (databaseConnection->moreData(data));

		FUNCTION_EXIT;

        // Return the vector
        return samplePeriods;
    }


    std::map<unsigned long, std::string> TrendAccessFactory::getAllSelectableItems()
    {
        FUNCTION_ENTRY("TrendAccessFactory::getAllSelectableItems");

        // Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

        // Retrieve the data points

        std::string sql("SELECT pkey, name FROM DATAPOINT_EV WHERE datapointtype NOT IN ('Boolean', 'Text')");
        
        // Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string pkey("PKEY");
        std::string name("NAME");
        columnNames.push_back(pkey);
        columnNames.push_back(name);

		IData* data = databaseConnection->executeQuery(sql, columnNames);

        int count = 1;
            
	    // Create the map to return
        std::map<unsigned long, std::string> items;

 	    if (data->getNumRows() == 0) // No datapoints found
        {
            // clean up the pointer
            delete data;
            data = NULL;
        
        }
        else
        {
            // Loop for each row returned
            do
            {
			    unsigned int numRows = data->getNumRows();
            
			    for (unsigned long i = 0; i < numRows; i++)
                {
                    // The getStringData() call can throw an exception. Need to catch
                    // it to do pointer clean up.
                    try
                    {
                        items.insert(std::map<unsigned long, std::string>::value_type(count++, data->getStringData(i, name)));
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
            while (databaseConnection->moreData(data));
        }

        // Retrieve the live properties

        sql = "SELECT e.pkey, e.name||'.'||drlp_name \
            FROM DR_LIVE_PROPERTY drlp, ENTITY e WHERE drlp_status = 'AVAILABLE' AND drlp_typekey = e.typekey \
            AND drlp_typekey IN (SELECT DISTINCT drlp_typekey FROM DR_LIVE_PROPERTY WHERE \
            drlp_category != 'DataPoint') ORDER BY 1";

        data = databaseConnection->executeQuery(sql, columnNames);

	    if (data->getNumRows() == 0) // No live properties found
        {
            // clean up the pointer
            delete data;
            data = NULL;
        
        }
        else
        {
            // Loop for each row returned
            do
            {
			    unsigned int numRows = data->getNumRows();
            
			    for (unsigned long i = 0; i < numRows; i++)
                {
                    // The getStringData() call can throw an exception. Need to catch
                    // it to do pointer clean up.
                    try
                    {
                        items.insert(std::map<unsigned long, std::string>::value_type(count++, data->getStringData(i, name)));
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
            while (databaseConnection->moreData(data));
        }

        FUNCTION_EXIT;
        return items;
    }

    
    std::string TrendAccessFactory::getSamplePeriodTableName(const std::string& typeIdentifier, unsigned long samplePeriod)
    {
        FUNCTION_ENTRY("TrendAccessFactory::getAllSamplePeriods");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Trend_Ad, Read);

		// Create the query
        std::ostringstream sql;
        sql << "SELECT object_name FROM DR_TREND_CONFIG where property_identifier = '";
        sql << typeIdentifier;
        sql << "' AND interval_seconds = ";
        sql << samplePeriod;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string tableName("NAME");
        columnNames.push_back(tableName);
        
		IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

	    if (data->getNumRows() != 1)
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return "";
        }
        else
        {
            std::string name = data->getStringData(0, tableName);

            delete data;
			data = NULL;

            return name;
        }
    }


    IConfigTrend* TrendAccessFactory::createTrend()
    {            
        return new ConfigTrend();    
    }


    IConfigTrend* TrendAccessFactory::copyTrend(const IConfigTrend* trendToCopy)
    {
        FUNCTION_ENTRY("copyTrend");

        TA_ASSERT(trendToCopy !=  NULL, "The Trend to copy was NULL");
        
        const ConfigTrend* cast = dynamic_cast<const ConfigTrend*>(trendToCopy);

        TA_ASSERT(cast != NULL, "Trend passed could not be converted into a ConfigTrend");

        FUNCTION_EXIT;
        return new ConfigTrend(*cast);
    }

} // closes TA_Base_Core


