/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSStationModeAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"

#include "core/data_access_interface/ecs/src/ECSStationMode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ecs/src/ConfigECSStationMode.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    ECSStationModeAccessFactory* ECSStationModeAccessFactory::m_instance = 0;

    ECSStationModeAccessFactory& ECSStationModeAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ECSStationModeAccessFactory();
        }
        return *m_instance;
    }


    void ECSStationModeAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IECSStationMode* ECSStationModeAccessFactory::getECSStationMode(const unsigned long key,const bool readWrite)
    {
		//zhangjunsheng++ 2008.09.27

        /*// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the ECSStationMode
        // this is a check to ensure an ECSStationMode with the specified
        // ECSMOD_ID actually exists.
        char sql[256];
		sprintf(sql,"select ECSMOD_ID from EC_STATION_MODE where ECSMOD_ID = %lu",key);	

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "ECSMOD_ID";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;*/

		//++zhangjunsheng 2008.09.27

        // Create the ECSStationMode object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IECSStationMode* theECSStationMode;
        
        try
		{
			// decide which ECSStationMode object to return
			if (readWrite) // need a config ECSStationMode
			{
				theECSStationMode = new ConfigECSStationMode(key);
			}
			else // need a standard ECSStationMode
			{
				theECSStationMode = new ECSStationMode(key);
			}
		}
		catch ( ... )
		{
			throw;
		}

        // Return the ECSStationMode pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theECSStationMode;
    }

    IECSStationModes ECSStationModeAccessFactory::getECSStationModesByMasterModeKey( unsigned long key, const bool readWrite )
	{
		// Get all the StationModes assocaited with a MasterMode
		LOG( SourceInfo, DebugUtil::FunctionEntry, "StationModeAccessFactory::getStationModesByMasterMode( key )");

		// Get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		// Create the query
     /*   std::ostringstream query;
		query << "select ECSMOD_ID from EC_MASTER_MODE_HELPER "
              << "where ECMMOD_ID = " << key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_HELPER_STD_SELECT_51002, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_HELPER_SELECT_51002, key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_SELECT_51002, key);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string StationModeCol( "ECSMOD_ID" );
        columnNames.push_back( StationModeCol );

		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

	    // Create the vector to return
        IECSStationModes StationModes;

 	    if (data->getNumRows() == 0) // No station modes found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            //char reasonMessage[256];
			//sprintf(reasonMessage, "No data found for MasterModeKey = %lu",key);			
            //    TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
            return StationModes;

        }

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        StationModes.push_back( new ConfigECSStationMode(data->getUnsignedLongData( i, StationModeCol )) );
                    }
                    else
                    {
                        StationModes.push_back( new ECSStationMode( data->getUnsignedLongData( i, StationModeCol ) ) );
                    }
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
				catch ( ...)
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

		LOG( SourceInfo, DebugUtil::FunctionExit, "StationModeAccessFactory::getStationModesByMasterMode( key )");

        // Return the vector
        return StationModes;
	}

    
    std::vector<unsigned long> ECSStationModeAccessFactory::getStationModeKeysByMasterModeKey(unsigned long key)
	{
		// Get all the StationModes assocaited with a MasterMode
		LOG( SourceInfo, DebugUtil::FunctionEntry, "StationModeAccessFactory::getStationModeKeysByMasterModeKey( key )");

		// Get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		// Create the query
       /* std::ostringstream query;
		query << "select ECSMOD_ID from EC_MASTER_MODE_HELPER "
              << "where ECMMOD_ID = " << key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_HELPER_STD_SELECT_51002, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_HELPER_SELECT_51002, key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_SELECT_51002, key);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string StationModeCol( "ECSMOD_ID" );
        columnNames.push_back( StationModeCol );

		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // create the vector of keys
        std::vector<unsigned long> stationModeKeys;

 	    if (data->getNumRows() == 0) // No station modes found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            //char reasonMessage[256];
			//sprintf(reasonMessage, "No data found for MasterModeKey = %lu",key);			
            //    TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
            return stationModeKeys;
        }

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                stationModeKeys.push_back(data->getUnsignedLongData(i,"ECSMOD_ID"));
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

		LOG( SourceInfo, DebugUtil::FunctionExit, "StationModeAccessFactory::getStationModeKeysByMasterModeKey( key )");

        // Return the vector
        return stationModeKeys;
	}


    IECSStationModes ECSStationModeAccessFactory::getAllECSStationModes( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the ECSStationMode
        // based upon the key
        /*std::ostringstream sql;
		sql << "select ECSMOD_ID, STATION_MODE, LOCATIONKEY, OPERATION, TRACK, CONDITION, CONGESTION_LOGIC,"
            << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from EC_STATION_MODE";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_Oracle_SELECT_52051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52051);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52051);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");
        columnNames.push_back("STATION_MODE");
        columnNames.push_back("LOCATIONKEY");
		columnNames.push_back("OPERATION");
		columnNames.push_back("TRACK");
		columnNames.push_back("CONDITION");
        columnNames.push_back("CONGESTION_LOGIC");
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");
        

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IECSStationModes ECSStationModePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i,"ECSMOD_ID");
                if (key != 0)
                {
					try
					{
						if (readWrite)
						{
							ECSStationModePointerVector.push_back( new ConfigECSStationMode(i, *data));
						}
						else
						{
							ECSStationModePointerVector.push_back( new ECSStationMode(i, *data));
						}
					}
					catch ( ... )
					{
						delete data;
						data = NULL;
						throw;
					}
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return ECSStationModePointerVector;
    }
    

    IECSStationModes ECSStationModeAccessFactory::getECSStationModesWhere( const bool readWrite,
                                                                           const unsigned long locationKey, 
                                                                           const unsigned long stationMode, 
                                                                           const std::string& operation, 
                                                                           const std::string& track, 
                                                                           const std::string& condition,
                                                                           const std::string& congestionLogic)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		int nDbType = databaseConnection->getDbServerType();

        // create the SQL string to retrieve the data of the ECSStationMode
        // based upon the where clauses
        std::ostringstream sql;
		/*sql << "select ECSMOD_ID, STATION_MODE, LOCATIONKEY, OPERATION, TRACK, CONDITION, CONGESTION_LOGIC,"
            << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from EC_STATION_MODE where 1=1 ";*/
		TA_Base_Core::SQLTypeAdapter sqlParam;

        if (locationKey != -1)
        {
            sql << "and LOCATIONKEY = " << locationKey << " ";
        }
        if (stationMode != -1)
        {
            sql << "and STATION_MODE = " << stationMode << " "; 
        }
        if (!operation.empty())
        {
            sql << "and OPERATION = '" 
                << (databaseConnection->escapeInsertString(operation)).c_str()
                << "' ";
        }
        if (!track.empty())
        {
            sql << "and TRACK = '" 
                << (databaseConnection->escapeInsertString(track)).c_str()
                << "' ";
        }
		sqlParam.add(sql.str());
		sql.str("");
        if (!condition.empty())
        {
			/*switch ( nDbType )
			{
			case enumOracleDb:
				sql << "and CONDITION = '" 
                << (databaseConnection->escapeInsertString(condition)).c_str()
                << "' ";
				break;
			case enumMysqlDb:
				sql << "and `CONDITION` = '" 
					<< (databaseConnection->escapeInsertString(condition)).c_str()
					<< "' ";
				break;
			}*/
			sqlParam.add(std::string("and "));
			sqlParam.add(TA_Base_Core::enumSyntax_Format_5);
			sqlParam.add(std::string(" = '") + databaseConnection->escapeInsertString(condition) + std::string("' "));
            
        }
        if (!congestionLogic.empty())
        {
            sql << "and CONGESTION_LOGIC = '" 
                << (databaseConnection->escapeInsertString(congestionLogic)).c_str()
                << "' ";
        }
		sqlParam.add(sql.str());
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_Oracle_SELECT_52052, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52052, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52052, sqlParam);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");
        columnNames.push_back("STATION_MODE");
        columnNames.push_back("LOCATIONKEY");
		columnNames.push_back("OPERATION");
		columnNames.push_back("TRACK");
		columnNames.push_back("CONDITION");
        columnNames.push_back("CONGESTION_LOGIC");
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");
        

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IECSStationModes ECSStationModePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i,"ECSMOD_ID");
                if (key != 0)
                {
					try
					{
						if (readWrite)
						{
							ECSStationModePointerVector.push_back( new ConfigECSStationMode(i, *data));
						}
						else
						{
							ECSStationModePointerVector.push_back( new ECSStationMode(i, *data));
						}
					}
					catch ( ... )
					{
						delete data;
						data = NULL;
						throw;
					}
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return ECSStationModePointerVector;
    }

	IECSStationModes ECSStationModeAccessFactory::getECSStationModeDetails(std::vector< unsigned long > keylist)
	{
		 TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the ECSStationMode
        // based upon the key
		 std::ostringstream keys;
		 std::vector<unsigned long>::iterator keyIt = keylist.begin();
		 keys << *keyIt;
		 for (keyIt = keylist.begin()+1;keyIt !=keylist.end(); keyIt++)
		 {
			 keys <<",";
			 keys<<*keyIt;
		 }
	//	 std::ostringstream sql;
	//sql << "select ECSMOD_ID, STATION_MODE, LOCATIONKEY, OPERATION, TRACK, CONDITION, CONGESTION_LOGIC,"
 //           << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
 //           << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
 //           << "from EC_STATION_MODE where ECSMOD_ID in (" << keys.str() << ")";
//	std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_Oracle_SELECT_52053, keys.str());
//	std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52053, keys.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52053, keys.str());

		std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");
        columnNames.push_back("STATION_MODE");
        columnNames.push_back("LOCATIONKEY");
		columnNames.push_back("OPERATION");
		columnNames.push_back("TRACK");
		columnNames.push_back("CONDITION");
        columnNames.push_back("CONGESTION_LOGIC");
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");
        

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IECSStationModes ECSStationModePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i,"ECSMOD_ID");
                if (key != 0)
                {
					try
					{
						ECSStationModePointerVector.push_back( new ECSStationMode(i, *data));
					}
					catch ( ... )
					{
						delete data;
						data = NULL;
						throw;						
					}
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return ECSStationModePointerVector;
	}
    std::vector<unsigned long> ECSStationModeAccessFactory::getECSStationModeKeysWhere
        ( const unsigned long locationKey, 
          const unsigned long stationMode, 
          const std::string& operation, 
          const std::string& track, 
          const std::string& condition,
          const std::string& congestionLogic)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		//int nDbType = databaseConnection->getDbServerType();

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select ECSMOD_ID from EC_STATION_MODE where 1=1 ";
		TA_Base_Core::SQLTypeAdapter sqlParam;

        if (locationKey != -1)
        {
            sql << "and LOCATIONKEY = " << locationKey << " ";
        }
        if (stationMode != -1)
        {
            sql << "and STATION_MODE = " << stationMode << " "; 
        }
        if (!operation.empty())
        {
            sql << "and OPERATION = '" 
                << (databaseConnection->escapeInsertString(operation)).c_str()
                << "' ";
        }
        if (!track.empty())
        {
            sql << "and TRACK = '" 
                << (databaseConnection->escapeInsertString(track)).c_str()
                << "' ";
        }
		sqlParam.add(sql.str());
		sql.str("");
        if (!condition.empty())
        {
			/*switch ( nDbType )
			{
			case enumOracleDb:
				sql << "and CONDITION = '" 
                << (databaseConnection->escapeInsertString(condition)).c_str()
                << "' ";
				break;
			case enumMysqlDb:
				sql << "and `CONDITION` = '" 
					<< (databaseConnection->escapeInsertString(condition)).c_str()
					<< "' ";
				break;
			}*/
			sqlParam.add(std::string("and "));
			sqlParam.add(TA_Base_Core::enumSyntax_Format_5);
			sqlParam.add(std::string(" = '") + databaseConnection->escapeInsertString(condition) + std::string("' "));
            
        }
        if (!congestionLogic.empty())
        {
            sql << "and CONGESTION_LOGIC = '" 
                << (databaseConnection->escapeInsertString(congestionLogic)).c_str()
                << "' ";
        }
		sqlParam.add(sql.str());
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_STD_SELECT_52002, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52002, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52002, sqlParam);

        std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);


        // create the vector of keys
        std::vector<unsigned long> stationModeKeys;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                stationModeKeys.push_back(data->getUnsignedLongData(i,"ECSMOD_ID"));
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return stationModeKeys;
    }

    
    IConfigECSStationMode* ECSStationModeAccessFactory::createECSStationMode()
    { 
		try
		{
			return new ConfigECSStationMode();
		}
		catch ( ... )
		{
			throw;
		}
            
    }


    IConfigECSStationMode* ECSStationModeAccessFactory::copyECSStationMode(const IConfigECSStationMode* ECSStationModeToCopy)
    {
        FUNCTION_ENTRY("copyECSStationMode");

		if (NULL == ECSStationModeToCopy)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationMode to copy was NULL" );
			return NULL;
		}
        
        const ConfigECSStationMode* cast = dynamic_cast<const ConfigECSStationMode*>(ECSStationModeToCopy);

		if (NULL == cast)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ECSStationMode passed could not be converted into a ConfigECSStationMode" );
			return NULL;
		}

        FUNCTION_EXIT;
		try
		{
			return new ConfigECSStationMode(*cast);
		}
		catch ( ... )
		{
			
		}
    }

    std::vector< std::pair<unsigned long, std::string> > ECSStationModeAccessFactory::getOrderedLocations()
	{
		FUNCTION_ENTRY("getOrderedLocations()");

		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		// create the SQL string to retrieve the data of the ordered locations
        // based upon the join of the LOCATION and EC_STATION_MODE
		/*std::ostringstream sql;
		sql << "select distinct l.pkey, l.name "
            << "from location l, ec_station_mode e "
            << "where l.pkey = e.locationkey "
			<< "order by l.pkey asc";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_STD_SELECT_52003);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52003);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52003);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
        columnNames.push_back( "PKEY" );
        columnNames.push_back( "ORDER_ID" );
		columnNames.push_back( "DISPLAY_NAME" );
	
		// Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

		std::vector< std::pair<unsigned long, std::string> > orderedlocations;

		do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                std::pair<unsigned long, std::string> station;
				station.first = data->getUnsignedLongData( i, "PKEY" );
				station.second = data->getStringData( i, "DISPLAY_NAME" );

				orderedlocations.push_back(station);
            }
			
            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

		FUNCTION_EXIT;
		return orderedlocations;
	}
} // closes TA_Core


