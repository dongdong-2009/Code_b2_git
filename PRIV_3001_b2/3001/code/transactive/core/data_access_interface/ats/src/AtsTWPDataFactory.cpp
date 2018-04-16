/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsTWPDataFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <sstream>
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/ats/src/AtsTWPDataFactory.h"
#include "core/data_access_interface/ats/src/AtsTWPData.h"
#include "core/data_access_interface/ats/src/ConfigAtsTWPData.h"

namespace TA_IRS_Core
{

	AtsTWPDataFactory& AtsTWPDataFactory::getInstance()
	{
		static AtsTWPDataFactory factory;

        return factory;
	}

	std::vector<IAtsTWPData*> AtsTWPDataFactory::getAllTWPData(const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsTWPData *> TWPDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
       /* std::ostringstream sql;
		sql << "select ATTEMA_ID, COMMAND, DATA_POINT_ENTITY_KEY, DATA_POINT_ENTITY_TYPE ";
		sql << "from AT_TWP_ENTITY_MAP";*/	
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_STD_SELECT_50001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_SELECT_50001);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_SELECT_50001);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string primaryKeyColumn			= "ATTEMA_ID";
        std::string CommandIdColumn				= "COMMAND";
        std::string DataPointEntityIdColumn		= "DATA_POINT_ENTITY_KEY";
        std::string DataPointEntityTypeIdColumn	= "DATA_POINT_ENTITY_TYPE";
		
        std::vector<std::string> columnNames;
        columnNames.push_back(primaryKeyColumn);
        columnNames.push_back(CommandIdColumn);
        columnNames.push_back(DataPointEntityIdColumn);
        columnNames.push_back(DataPointEntityTypeIdColumn);
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
                
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No records are found
        {
            // clean up the pointer
            delete data;
            data = NULL;
            
			// will be empty
			return TWPDataVector;
        }

        // retrieve information in the database
		do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. 
                try
                {
                    // Create a shiny new AtsPSDData interface which will have all the information pre-loaded!
                    IAtsTWPData* atsTWPData = NULL;

					if (readWrite)
					{
						atsTWPData = new ConfigAtsTWPData(i, *data);
					}
					else
					{
						atsTWPData = new AtsTWPData(i, *data);
					}

					// add this object to the vector
                    TWPDataVector.push_back(atsTWPData);
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
        
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return TWPDataVector;
	}

	std::vector<IAtsTWPData*> AtsTWPDataFactory::getAllTWPDataOfDataPointType(const std::string& aDataPointType, const bool readWrite)
	{
				// initialise our return variable
		std::vector<IAtsTWPData *> TWPDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
       /* std::ostringstream sql;
		sql << "select ATTEMA_ID, COMMAND, DATA_POINT_ENTITY_KEY, DATA_POINT_ENTITY_TYPE ";
		sql << "from AT_TWP_ENTITY_MAP where upper(DATA_POINT_ENTITY_TYPE) = upper('" << aDataPointType << "')";*/	
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_TWP_ENTITY_MAP_STD_SELECT_50002, aDataPointType);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_TWP_ENTITY_MAP_SELECT_50002, aDataPointType);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_TWP_ENTITY_MAP_SELECT_50002, aDataPointType);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string primaryKeyColumn			= "ATTEMA_ID";
        std::string CommandIdColumn				= "COMMAND";
        std::string DataPointEntityIdColumn		= "DATA_POINT_ENTITY_KEY";
        std::string DataPointEntityTypeIdColumn	= "DATA_POINT_ENTITY_TYPE";
		
        std::vector<std::string> columnNames;
        columnNames.push_back(primaryKeyColumn);
        columnNames.push_back(CommandIdColumn);
        columnNames.push_back(DataPointEntityIdColumn);
        columnNames.push_back(DataPointEntityTypeIdColumn);
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
                
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No records are found
        {
            // clean up the pointer
            delete data;
            data = NULL;
            
			// will be empty
			return TWPDataVector;
        }

        // retrieve information in the database
		do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. 
                try
                {
                    // Create a shiny new AtsPSDData interface which will have all the information pre-loaded!
                    IAtsTWPData* atsTWPData = NULL;

                    if (readWrite)
					{
						atsTWPData = new ConfigAtsTWPData(i, *data);
					}
					else
					{
						atsTWPData = new AtsTWPData(i, *data);
					}

					// add this object to the vector
                    TWPDataVector.push_back(atsTWPData);
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
        
        // Now that we're finished with the TA_Base_Core::IData object, we need to delete it.
        delete data;
        data = NULL;

        return TWPDataVector;
	}

	IConfigAtsTWPData* AtsTWPDataFactory::createAtsTWPData()
    {            
        return new ConfigAtsTWPData();    
    }


    IConfigAtsTWPData* AtsTWPDataFactory::copyAtsTWPData(const IConfigAtsTWPData* atsPowerDataToCopy)
    {
        FUNCTION_ENTRY("copyAtsTWPData");

        TA_ASSERT(atsPowerDataToCopy !=  NULL, "The AtsTWPData to copy was NULL");
        
        const ConfigAtsTWPData* cast = dynamic_cast<const ConfigAtsTWPData*>(atsPowerDataToCopy);

        TA_ASSERT(cast != NULL, "AtsTWPData passed could not be converted into a ConfigAtsTWPData");

        FUNCTION_EXIT;
        return new ConfigAtsTWPData(*cast);

    }

} // TA_Core
