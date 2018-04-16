/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsPSDDataFactory.cpp $
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

#include "core/data_access_interface/ats/src/AtsPSDDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPSDData.h"
#include "core/data_access_interface/ats/src/ConfigAtsPSDData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{	
	static const std::string KEY_COL					= "ATPEMA_ID";
    static const std::string STATIONID_COL				= "STATION_ID";
	static const std::string PLATFORMKEY_COL			= "PLATFORM_KEY";
	static const std::string PLATFORMID_COL				= "PLATFORM_ID";
    static const std::string PLATFORMNAME_COL			= "PLATFORM_NAME";
    static const std::string TIS_PLATFORMID_COL         = "TIS_PLATFORM_ID";
	static const std::string MSSZONE_COL				= "MSS_ZONE";
	static const std::string PSDNUMBER_COL				= "PSD_NUMBER";
	static const std::string DATAPOINTENTITYKEY_COL		= "DATA_POINT_ENTITY_KEY";
	static const std::string DATAPOINTTYPE_COL			= "DATA_POINT_TYPE";
	static const std::string DATAPOINTNAMEOFTYPE_COL	= "DATA_POINT_NAMEOF_TYPE";

	AtsPSDDataFactory& AtsPSDDataFactory::getInstance()
	{
		static AtsPSDDataFactory factory;

        return factory;
	}

	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataInMSSZone(const std::string &aMSSZone, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << MSSZONE_COL << " = upper('" << aMSSZone << "')";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}               				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}


	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataInMSSZoneOfGivenDataPointNameOfType(const std::string &aMSSZone, const std::string &aDataPointNameOfType, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << MSSZONE_COL << " = upper('" << aMSSZone << "') ";
		sql << "and " << DATAPOINTNAMEOFTYPE_COL << " = '" << aDataPointNameOfType << "'";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}                 				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}


	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataInMSSZoneOfGivenDataPointType(const std::string &aMSSZone, const std::string &aDataPointType, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << MSSZONE_COL << " = upper('" << aMSSZone << "') ";
		sql << "and " << DATAPOINTTYPE_COL << " = '" << aDataPointType << "'";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}                 				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}


	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataOfGivenDataPointType(const std::string &aDataPointType, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << DATAPOINTTYPE_COL << " = '" << aDataPointType << "'";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}               				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}

	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataOfGivenDataPointNameOfType(const std::string &aDataPointNameOfType, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << DATAPOINTNAMEOFTYPE_COL << " = '" << aDataPointNameOfType << "'";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

					if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}
                	
					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}

	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataAtGivenStationOfGivenDataPointType(unsigned long aStationId, const std::string &aDataPointType, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << DATAPOINTTYPE_COL << " = '" << aDataPointType << "' ";
		sql << "and " << STATIONID_COL << " = " << aStationId;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}                				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}

	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataAtGivenStationOfGivenDataPointNameOfType(unsigned long aStationId, const std::string &aDataPointNameOfType, const bool readWrite)
	{
			// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << DATAPOINTNAMEOFTYPE_COL << " = '" << aDataPointNameOfType << "' ";
		sql << "and " << STATIONID_COL << " = " << aStationId;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}                 				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}
	
	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDDataAtGivenStation(unsigned long aStationId, const bool readWrite)
	{
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where " << STATIONID_COL << " = " << aStationId;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

                    if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}                   				 												

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
	}


	std::vector<IAtsPSDData*> AtsPSDDataFactory::getAllPSDData(const bool readWrite)
    {
		// initialise our return variable
		std::vector<IAtsPSDData *> PSDDataVector;

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create an SQL statement to retrieve all records from the view
		std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back(KEY_COL);
        columnNames.push_back(STATIONID_COL);
        columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

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
			return PSDDataVector;
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
                    IAtsPSDData* atsPSDData = NULL;

					if (readWrite)
					{
						atsPSDData = new ConfigAtsPSDData(i, *data);
					}
					else
					{
						atsPSDData = new AtsPSDData(i, *data);
					}

					// add this object to the vector
                    PSDDataVector.push_back(atsPSDData);
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

        return PSDDataVector;
    }

	IConfigAtsPSDData* AtsPSDDataFactory::createAtsPSDData()
    {            
        return new ConfigAtsPSDData();    
    }


    IConfigAtsPSDData* AtsPSDDataFactory::copyAtsPSDData(const IConfigAtsPSDData* atsPSDDataToCopy)
    {
        FUNCTION_ENTRY("copyAtsPSDData");

        TA_ASSERT(atsPSDDataToCopy !=  NULL, "The AtsPSDData to copy was NULL");
        
        const ConfigAtsPSDData* cast = dynamic_cast<const ConfigAtsPSDData*>(atsPSDDataToCopy);

        TA_ASSERT(cast != NULL, "AtsPSDData passed could not be converted into a ConfigAtsPSDData");

        FUNCTION_EXIT;
        return new ConfigAtsPSDData(*cast);

    }

} // TA_Core
