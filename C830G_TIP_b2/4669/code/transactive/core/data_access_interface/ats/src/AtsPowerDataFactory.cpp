/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsPowerDataFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <sstream>
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/ats/src/AtsPowerDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPowerData.h"
#include "core/data_access_interface/ats/src/ConfigAtsPowerData.h"

namespace TA_IRS_Core
{

	AtsPowerDataFactory& AtsPowerDataFactory::getInstance()
	{
		static AtsPowerDataFactory factory;

        return factory;
	}

	AtsPowerDataFactory::~AtsPowerDataFactory()
	{

	}

	std::vector<IAtsPowerData*> AtsPowerDataFactory::getAllPowerDataAtLocation(unsigned long aLocationKey, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPowerData *> PowerDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Column names
		std::string primaryKeyColumn		= "ATPOEM_ID";
        std::string SubSectionIdColumn		= "SUBSECTION_ID";
		std::string BitPositionColumn		= "BIT_POSITION";
		std::string DataPointEntityIdColumn	= "DATA_POINT_ENTITY_KEY";
		std::string LocationIdColumn		= "STATION_ID";
		std::string MSSZoneIdColumn			= "MSS_ZONE";

		// create an SQL statement to retrieve all records from the view
        std::ostringstream sql;
		sql << "select " << primaryKeyColumn << ", " << SubSectionIdColumn << ", ";
		sql << BitPositionColumn << ", " << DataPointEntityIdColumn << ", " << LocationIdColumn << ", ";
		sql << MSSZoneIdColumn;
		sql << " from AT_POWER_ENTITY_MAP_V where ";
		sql << LocationIdColumn << " = " << aLocationKey;		

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(primaryKeyColumn);
        columnNames.push_back(SubSectionIdColumn);
		columnNames.push_back(BitPositionColumn);
        columnNames.push_back(DataPointEntityIdColumn);
        columnNames.push_back(LocationIdColumn);
		columnNames.push_back(MSSZoneIdColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
                
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No records are found
        {
            // clean up the pointer
            delete data;
            data = NULL;
            
			// will be empty
			return PowerDataVector;
        }

        // retrieve information in the database
		do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. 
                try
                {
                    // Create a shiny new AtsPowerData interface which will have all the information pre-loaded!
                    IAtsPowerData* atsPowerData = NULL;

					if (readWrite)
					{
						atsPowerData = new ConfigAtsPowerData(i, *data);
					}
					else
					{
						atsPowerData = new AtsPowerData(i, *data);
					}   

					// add this object to the vector
                    PowerDataVector.push_back(atsPowerData);
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

        return PowerDataVector;
	}

	std::map<unsigned long, std::vector<IAtsPowerData*> > AtsPowerDataFactory::getAllPowerDataAtLocationBySubsection(unsigned long aLocationKey, const bool readWrite)
	{
		std::vector<IAtsPowerData*> powerData = getAllPowerDataAtLocation(aLocationKey, readWrite);

		return orderPowerData(powerData);
	}

	std::vector<IAtsPowerData*> AtsPowerDataFactory::getAllPowerDataAtMSSZone(const std::string &aMSSZone, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPowerData *> PowerDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Column names
		std::string primaryKeyColumn		= "ATPOEM_ID";
        std::string SubSectionIdColumn		= "SUBSECTION_ID";
		std::string BitPositionColumn		= "BIT_POSITION";
		std::string DataPointEntityIdColumn	= "DATA_POINT_ENTITY_KEY";
		std::string LocationIdColumn		= "STATION_ID";
		std::string MSSZoneIdColumn			= "MSS_ZONE";

		// create an SQL statement to retrieve all records from the view
        std::ostringstream sql;
		sql << "select " << primaryKeyColumn << ", " << SubSectionIdColumn << ", ";
		sql << BitPositionColumn << ", " << DataPointEntityIdColumn << ", " << LocationIdColumn << ", ";
		sql << MSSZoneIdColumn;
		sql << " from AT_POWER_ENTITY_MAP_V where upper(";
		sql << MSSZoneIdColumn << ") = upper('" <<  aMSSZone << "')";		

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(primaryKeyColumn);
        columnNames.push_back(SubSectionIdColumn);
		columnNames.push_back(BitPositionColumn);
        columnNames.push_back(DataPointEntityIdColumn);
        columnNames.push_back(LocationIdColumn);
		columnNames.push_back(MSSZoneIdColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
                
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No records are found
        {
            // clean up the pointer
            delete data;
            data = NULL;
            
			// will be empty
			return PowerDataVector;
        }

        // retrieve information in the database
		do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. 
                try
                {
                    // Create a shiny new AtsPowerData interface which will have all the information pre-loaded!
                    IAtsPowerData* atsPowerData = NULL;

                    if (readWrite)
					{
						atsPowerData = new ConfigAtsPowerData(i, *data);
					}
					else
					{
						atsPowerData = new AtsPowerData(i, *data);
					}       				 												

					// add this object to the vector
                    PowerDataVector.push_back(atsPowerData);
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

        return PowerDataVector;
	}

	std::map<unsigned long, std::vector<IAtsPowerData*> > AtsPowerDataFactory::getAllPowerDataAtMSSZoneBySubsection(const std::string &aMSSZone, const bool readWrite)
	{
		std::vector<IAtsPowerData*> powerData = getAllPowerDataAtMSSZone(aMSSZone, readWrite);

		return orderPowerData(powerData);
	}

	std::vector<IAtsPowerData*> AtsPowerDataFactory::getAllPowerData(const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPowerData *> PowerDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Columns
		std::string primaryKeyColumn		= "ATPOEM_ID";
        std::string SubSectionIdColumn		= "SUBSECTION_ID";
		std::string BitPositionColumn		= "BIT_POSITION";
		std::string DataPointEntityIdColumn	= "DATA_POINT_ENTITY_KEY";
		std::string LocationIdColumn		= "STATION_ID";
		std::string MSSZoneIdColumn			= "MSS_ZONE";

		// create an SQL statement to retrieve all records from the view
        std::ostringstream sql;
		sql << "select " << primaryKeyColumn << ", " << SubSectionIdColumn << ", ";
		sql << BitPositionColumn << ", " << DataPointEntityIdColumn << ", " << LocationIdColumn << ", ";
		sql << MSSZoneIdColumn;
		sql << " from AT_POWER_ENTITY_MAP_V";		

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(primaryKeyColumn);
        columnNames.push_back(SubSectionIdColumn);
		columnNames.push_back(BitPositionColumn);
        columnNames.push_back(DataPointEntityIdColumn);
        columnNames.push_back(LocationIdColumn);
		columnNames.push_back(MSSZoneIdColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
                
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No records are found
        {
            // clean up the pointer
            delete data;
            data = NULL;
            
			// will be empty
			return PowerDataVector;
        }

        // retrieve information in the database
		do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. 
                try
                {
                    // Create a shiny new AtsPowerData interface which will have all the information pre-loaded!
                    IAtsPowerData* atsPowerData = NULL;

                    if (readWrite)
					{
						atsPowerData = new ConfigAtsPowerData(i, *data);
					}
					else
					{
						atsPowerData = new AtsPowerData(i, *data);
					}             				 												

					// add this object to the vector
                    PowerDataVector.push_back(atsPowerData);
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

        return PowerDataVector;
	}

	std::map<unsigned long, std::vector<IAtsPowerData*> > AtsPowerDataFactory::getAllPowerDataBySubsection(const bool readWrite)
	{
		std::vector<IAtsPowerData*> powerData = getAllPowerData(readWrite);

		return orderPowerData(powerData);
	}

	std::map<unsigned long, std::vector<IAtsPowerData*> > AtsPowerDataFactory::orderPowerData (std::vector<IAtsPowerData*>& powerData)
	{
		std::vector<IAtsPowerData*>::iterator it;

		std::map<unsigned long, std::vector<IAtsPowerData*> > orderedPowerData;

		for (it = powerData.begin(); it != powerData.end(); it++)
		{
			std::map<unsigned long, std::vector<IAtsPowerData*> >::iterator orderedIt;

			orderedIt = orderedPowerData.find((*it)->getElectricalSubSectionId());

			if (orderedIt == orderedPowerData.end())
			{
				std::vector<IAtsPowerData*> newVector;

				newVector.push_back(*it);

				orderedPowerData[(*it)->getElectricalSubSectionId()] = newVector;
			}
			else
			{
				// Check that this datapoint isn't already in the list
				std::vector<IAtsPowerData*>::iterator dupIt;

				for (dupIt = (*orderedIt).second.begin();
				     dupIt != (*orderedIt).second.end();
					 dupIt++)
				{
					if ((*dupIt)->getTractionPowerDataPointEntityKey() == 
							(*it)->getTractionPowerDataPointEntityKey())
					{
						// duplicate - break;
						break;
					}
				}

				if (dupIt == (*orderedIt).second.end())
				{
					// Not a duplicate - add
					(*orderedIt).second.push_back(*it);
				}
				else
				{
					delete (*it);
					(*it) = NULL;
				}
			}
		}

		return orderedPowerData;
	}

	IConfigAtsPowerData* AtsPowerDataFactory::createAtsPowerData()
    {            
        return new ConfigAtsPowerData();    
    }


    IConfigAtsPowerData* AtsPowerDataFactory::copyAtsPowerData(const IConfigAtsPowerData* atsPowerDataToCopy)
    {
        FUNCTION_ENTRY("copyAtsPowerData");

        TA_ASSERT(atsPowerDataToCopy !=  NULL, "The AtsPowerData to copy was NULL");
        
        const ConfigAtsPowerData* cast = dynamic_cast<const ConfigAtsPowerData*>(atsPowerDataToCopy);

        TA_ASSERT(cast != NULL, "AtsPowerData passed could not be converted into a ConfigAtsPowerData");

        FUNCTION_EXIT;
        return new ConfigAtsPowerData(*cast);

    }
}
