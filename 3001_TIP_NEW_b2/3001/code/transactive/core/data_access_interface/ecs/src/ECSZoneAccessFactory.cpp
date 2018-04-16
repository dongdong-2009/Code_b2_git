/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/ECSZoneAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/10/07 08:59:52 $
  * Last modified by:  $Author: huangjian $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSZone.h"
#include "core/data_access_interface/ecs/src/ConfigECSZone.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    ECSZoneAccessFactory* ECSZoneAccessFactory::m_instance = 0;

    ECSZoneAccessFactory& ECSZoneAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ECSZoneAccessFactory();
        }
        return *m_instance;
    }


    void ECSZoneAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IECSZone* ECSZoneAccessFactory::getECSZone(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the ECSZone
        // this is a check to ensure an ECSZone with the specified
        // PKEY actually exists.
        /*char sql[256];
		sprintf(sql,"select ECZONE_ID from EC_ZONE where ECZONE_ID = %lu",idKey);*/	
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_STD_SELECT_51501, idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51501, idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51501, idKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("ECZONE_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the ECSZone object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IECSZone* theECSZone;
        
		try
		{
			// decide which ECSZone object to return
			if (readWrite) // need a config ECSZone
			{
				theECSZone = new ConfigECSZone(idKey);
			}
			else // need a standard ECSZone
			{
				theECSZone = new ECSZone(idKey);
			}
		}
		catch ( ... )
		{
			throw;
		}

        // Return the ECSZone pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theECSZone;
    }

    
    IECSZones ECSZoneAccessFactory::getAllECSZones( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the ECSZone
        // based upon the key
       /* std::ostringstream sql;
		sql << "select ECZONE_ID, ECS_LOCATION, TRACK, OCC_EQUIPMENT_ENTITYKEY, STN_EQUIPMENT_ENTITYKEY, IS_IGNORING_CONGESTION, "
            << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from EC_ZONE";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_Oracle_SELECT_51551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51551);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51551);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("ECZONE_ID");
        columnNames.push_back("ECS_LOCATION");
		columnNames.push_back("TRACK");
        columnNames.push_back("OCC_EQUIPMENT_ENTITYKEY");
        columnNames.push_back("STN_EQUIPMENT_ENTITYKEY");
        columnNames.push_back("IS_IGNORING_CONGESTION");
		columnNames.push_back("DATE_MODIFIED");
		columnNames.push_back("DATE_CREATED");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IECSZones ECSZonePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,"ECZONE_ID");
                if (id != 0)
                {
					try
					{
						if (readWrite)
						{
							ECSZonePointerVector.push_back( new ConfigECSZone(i, *data));
						}
						else
						{
							ECSZonePointerVector.push_back( new ECSZone(i, *data));
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

        return ECSZonePointerVector;
    }
    
  
    IECSZones ECSZoneAccessFactory::getECSZonesWhere( const bool readWrite,
                                                      const std::string& ecsLocation,
                                                      const std::string& track, 
                                                      const unsigned long occEquipmentEntityKey,
                                                      const unsigned long stnEquipmentEntityKey )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		/*sql << "select ECZONE_ID, ECS_LOCATION, TRACK, OCC_EQUIPMENT_ENTITYKEY, STN_EQUIPMENT_ENTITYKEY, IS_IGNORING_CONGESTION, "
            << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from EC_ZONE where 1=1 ";*/
            
        if (!ecsLocation.empty())
        {
            sql << "and ECS_LOCATION = '" 
                << (databaseConnection->escapeInsertString(ecsLocation)).c_str()
                << "' ";
        }
        if (!track.empty())
        {
            sql << "and TRACK = '" 
                << (databaseConnection->escapeInsertString(track)).c_str()
                << "' ";
        }
        if (occEquipmentEntityKey != -1)
        {
            sql << "and OCC_EQUIPMENT_ENTITYKEY = " << occEquipmentEntityKey << " ";
        }
        if (stnEquipmentEntityKey != -1)
        {
            sql << "and STN_EQUIPMENT_ENTITYKEY = " << stnEquipmentEntityKey << " ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_Oracle_SELECT_51552, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51552, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51552, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back("ECZONE_ID");
        columnNames.push_back("ECS_LOCATION");
		columnNames.push_back("TRACK");
        columnNames.push_back("OCC_EQUIPMENT_ENTITYKEY");
        columnNames.push_back("STN_EQUIPMENT_ENTITYKEY");
        columnNames.push_back("IS_IGNORING_CONGESTION");
		columnNames.push_back("DATE_MODIFIED");
		columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        
        // create the vector of pointers to IEntityData
        IECSZones ECSZonePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,"ECZONE_ID");
                if (id != 0)
                {
					try
					{
						if (readWrite)
						{
							ECSZonePointerVector.push_back( new ConfigECSZone(i, *data));
						}
						else
						{
							ECSZonePointerVector.push_back( new ECSZone(i, *data));
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

        return ECSZonePointerVector;
    }


    std::vector<int> ECSZoneAccessFactory::getECSZoneIdsWhere
        ( const std::string& ecsLocation,
          const std::string& track, 
          const unsigned long occEquipmentEntityKey,
          const unsigned long stnEquipmentEntityKey )    
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select ECZONE_ID from EC_ZONE where 1=1 ";

        if (!ecsLocation.empty())
        {
            sql << "and ECS_LOCATION = '" 
                << (databaseConnection->escapeInsertString(ecsLocation)).c_str()
                << "' ";
        }
        if (!track.empty())
        {
            sql << "and TRACK = '" 
                << (databaseConnection->escapeInsertString(track)).c_str()
                << "' ";
        }
        if (occEquipmentEntityKey != -1)
        {
            sql << "and OCC_EQUIPMENT_ENTITYKEY = " << occEquipmentEntityKey << " ";
        }
        if (stnEquipmentEntityKey != -1)
        {
            sql << "and STN_EQUIPMENT_ENTITYKEY = " << stnEquipmentEntityKey << " ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_STD_SELECT_51502, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51502, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51502, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back("ECZONE_ID");

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
                zoneIds.push_back(data->getUnsignedLongData(i,"ECZONE_ID"));
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return zoneIds;
    }



    IConfigECSZone* ECSZoneAccessFactory::createECSZone()
    { 
		try
		{
			return new ConfigECSZone();
		}
		catch ( ... )
		{
			throw;
		}  
    }


    IConfigECSZone* ECSZoneAccessFactory::copyECSZone(const IConfigECSZone* ECSZoneToCopy)
    {
        FUNCTION_ENTRY("copyECSZone");

		if (NULL == ECSZoneToCopy)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZone to copy was NULL" );
			return NULL;
		}
        
        const ConfigECSZone* cast = dynamic_cast<const ConfigECSZone*>(ECSZoneToCopy);

		if (NULL == cast)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ECSZone passed could not be converted into a ConfigECSZone" );
			return NULL;
		}

        FUNCTION_EXIT;
		try
		{
			return new ConfigECSZone(*cast);
		}
		catch ( ... )
		{
			throw;			
		}
    }


} // closes TA_Core


