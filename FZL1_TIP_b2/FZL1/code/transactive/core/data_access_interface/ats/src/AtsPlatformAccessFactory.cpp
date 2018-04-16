/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsPlatformAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsPlatform.h"
#include "core/data_access_interface/ats/src/ConfigAtsPlatform.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string KEY_COL			= "ATPLAT_ID";
	static const std::string PLATFORMID_COL		= "PLATFORM_ID";
    static const std::string STATIONID_COL		= "STATION_ID";
	static const std::string PLATFORMNAME_COL	= "PLATFORM_NAME";
    static const std::string TIS_PLATFORMID_COL	= "TIS_PLATFORM_ID";

    AtsPlatformAccessFactory* AtsPlatformAccessFactory::m_instance = 0;

    AtsPlatformAccessFactory& AtsPlatformAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AtsPlatformAccessFactory();
        }
        return *m_instance;
    }


    void AtsPlatformAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IAtsPlatform* AtsPlatformAccessFactory::getAtsPlatform(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the AtsPlatform
        // this is a check to ensure an AtsPlatform with the specified
        // PKEY actually exists.
        /*char sql[256] = {0};
		sprintf(sql,"select %s from AT_PLATFORM where %s = %lu",KEY_COL.c_str(), KEY_COL.c_str(), idKey);*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_SELECT_49501, idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_SELECT_49501, idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49501, idKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the AtsPlatform object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IAtsPlatform* theAtsPlatform;
        
        // decide which AtsPlatform object to return
        if (readWrite) // need a config AtsPlatform
        {
            theAtsPlatform = new ConfigAtsPlatform(idKey);
        }
        else // need a standard AtsPlatform
        {
            theAtsPlatform = new AtsPlatform(idKey);
        }

        // Return the AtsPlatform pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theAtsPlatform;
    }
    
    IAtsPlatforms AtsPlatformAccessFactory::getAllAtsPlatforms( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the AtsPlatform
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << PLATFORMID_COL << ", "<< STATIONID_COL << ", " 
			<< PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << " from AT_PLATFORM";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_SELECT_49502);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_SELECT_49502);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49502);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(PLATFORMID_COL);
		columnNames.push_back(STATIONID_COL);	
		columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsPlatforms AtsPlatformPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsPlatformPointerVector.push_back( new ConfigAtsPlatform(i, *data));
                    }
                    else
                    {
                        AtsPlatformPointerVector.push_back( new AtsPlatform(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsPlatformPointerVector;
    }
    
  
    IAtsPlatforms AtsPlatformAccessFactory::getAtsPlatformsWhere( const bool readWrite,
																  long platformId,
																  long stationId, 
																  const std::string& platformName )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		/*sql << "select " << KEY_COL << ", " << PLATFORMID_COL << ", "<< STATIONID_COL << ", " 
			<< PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << " from AT_PLATFORM where 1=1 ";*/
            
		if (platformId != -1)
        {
            sql << "and " << PLATFORMID_COL << " = " 
                << platformId
                << " ";
        }
		if (stationId != -1)
        {
            sql << "and " << STATIONID_COL << " = " 
                << stationId
                << " ";
        }
        if (!platformName.empty())
        {
            sql << "and " << PLATFORMNAME_COL << " = '" 
                << (databaseConnection->escapeInsertString(platformName)).c_str()
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_SELECT_49503, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_SELECT_49503, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49503, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(PLATFORMID_COL);
		columnNames.push_back(STATIONID_COL);
		columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsPlatforms AtsPlatformPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsPlatformPointerVector.push_back( new ConfigAtsPlatform(i, *data));
                    }
                    else
                    {
                        AtsPlatformPointerVector.push_back( new AtsPlatform(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsPlatformPointerVector;
    }


    std::vector<int> AtsPlatformAccessFactory::getAtsPlatformIdsWhere
        ( long platformId, long stationId, const std::string& platformName  )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select " << KEY_COL << " from AT_PLATFORM where 1=1 ";

        if (platformId != -1)
        {
            sql << "and " << PLATFORMID_COL << " = " 
                << platformId
                << " ";
        }
		if (stationId != -1)
        {
            sql << "and " << STATIONID_COL << " = " 
                << stationId
                << " ";
        }
        if (!platformName.empty())
        {
            sql << "and " << PLATFORMNAME_COL << " = '" 
                << (databaseConnection->escapeInsertString(platformName)).c_str()
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_SELECT_49504, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_SELECT_49504, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49504, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);


        // create the vector of keys
        std::vector<int> zoneIds;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                zoneIds.push_back(data->getUnsignedLongData(i, KEY_COL));
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return zoneIds;
    }




	unsigned long AtsPlatformAccessFactory::getAtsStationIdWhere(long platformId)
	{
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create the SQL string to retrieve the guis
		//std::ostringstream sql;
		//sql << "select " << STATIONID_COL << " from AT_PLATFORM where ";
		//sql  << " and " <<  PLATFORMID_COL << " = " << platformId;

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49506, platformId);


		std::vector<std::string> columnNames;
		columnNames.push_back(STATIONID_COL);
				
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

		unsigned long StationId;

		if(data->getNumRows() > 1)
		{
			delete data;
			data = NULL;

			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for platform id = %lu",platformId);			
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
		}
		else if(data->getNumRows() == 0)
		{
			// clean up the pointer
			delete data;
			data = NULL;

			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for  platform id = %lu",platformId);	
			TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
		}
		else
		{
			StationId = data->getUnsignedLongData(0,STATIONID_COL);
		}
	
		delete data;
		data = NULL;

		while( databaseConnection->moreData(data) );

		return StationId;

	}


    IConfigAtsPlatform* AtsPlatformAccessFactory::createAtsPlatform()
    {            
        return new ConfigAtsPlatform();    
    }


    IConfigAtsPlatform* AtsPlatformAccessFactory::copyAtsPlatform(const IConfigAtsPlatform* AtsPlatformToCopy)
    {
        FUNCTION_ENTRY("copyAtsPlatform");

        TA_ASSERT(AtsPlatformToCopy !=  NULL, "The AtsPlatform to copy was NULL");
        
        const ConfigAtsPlatform* cast = dynamic_cast<const ConfigAtsPlatform*>(AtsPlatformToCopy);

        TA_ASSERT(cast != NULL, "AtsPlatform passed could not be converted into a ConfigAtsPlatform");

        FUNCTION_EXIT;
        return new ConfigAtsPlatform(*cast);

    }


} // closes TA_Core


