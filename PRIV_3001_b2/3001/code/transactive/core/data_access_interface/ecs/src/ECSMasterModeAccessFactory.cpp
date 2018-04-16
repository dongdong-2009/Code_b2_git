/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.cpp $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/03/14 13:51:20 $
* Last modified by:  $Author: lim.cy $
*
*/
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif

#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/ecs/src/ECSMasterMode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ecs/src/ConfigECSMasterMode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	
    static const std::string KEY_COL = "ECMMOD_ID";
    static const std::string MASTERMODE_COL = "MASTER_MODE";
    static const std::string CONDITION_COL = "CONDITION";
    static const std::string ECSZONEID_COL = "ECZONE_ID";
    static const std::string EVACDIR_COL = "EVACUATION_DIRECTION";
    static const std::string AIRDIR_COL = "AIRFLOW_DIRECTION";
	static const std::string SITUATION_COL = "SITUATION";
    static const std::string DISPDIR_COL = "DISPLAY_DIRECTION";
	static const std::string VERIFMSG_COL = "VERIFICATION_MSG";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string MASTERMODEKEY_COL = "ECMMOD_ID";
    static const std::string STATIONMODEKEY_COL = "ECSMOD_ID";
	
	
    ECSMasterModeAccessFactory* ECSMasterModeAccessFactory::m_instance = 0;
	
    ECSMasterModeAccessFactory& ECSMasterModeAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ECSMasterModeAccessFactory();
        }
        return *m_instance;
    }
	
	
    void ECSMasterModeAccessFactory::removeInstance( )
    {
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}
    }
	
	
    IECSMasterMode* ECSMasterModeAccessFactory::getECSMasterMode(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		
        // create the SQL string to retrieve the name of the ECSMasterMode
        // this is a check to ensure an ECSMasterMode with the specified
        // PKEY actually exists.
       /* char sql[256];
		sprintf(sql,"select ECMMOD_ID from EC_MASTER_MODE where ECMMOD_ID = %lu",key);	*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_STD_SELECT_50501, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_SELECT_50501, key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50501, key);
		
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("ECMMOD_ID");
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
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
        data = NULL;
		
        // Create the ECSMasterMode object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IECSMasterMode* theECSMasterMode;
        
		try
		{
			// decide which ECSMasterMode object to return
			if (readWrite) // need a config ECSMasterMode
			{
				theECSMasterMode = new ConfigECSMasterMode(key);
			}
			else // need a standard ECSMasterMode
			{
				theECSMasterMode = new ECSMasterMode(key);
			}
		}
		catch ( ... )
		{
			throw;
		}

        // Return the ECSMasterMode pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theECSMasterMode;
    }
	
	
    IECSMasterModes ECSMasterModeAccessFactory::getAllECSMasterModes( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		
        // create the SQL string to retrieve the data of the ECSMasterMode
        // based upon the key
  //      std::ostringstream sql;
		//sql << "select " 
  //          << KEY_COL << ","
  //          << MASTERMODE_COL << ","
  //          << CONDITION_COL << ","
  //          << "nvl(" << ECSZONEID_COL << ",-1) as " << ECSZONEID_COL << ","
  //          << EVACDIR_COL << ","
  //          << AIRDIR_COL << ","
  //          << DISPDIR_COL << ","
		//	<< VERIFMSG_COL << ","
		//	//<< "nvl(" << VERIFMSG_COL << ",'NA'), "
  //          << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
  //          << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
  //          << "from EC_MASTER_MODE";
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_Oracle_SELECT_50551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_SELECT_50551);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50551);
		
        // Set up the columnNames vector to be passed to executeQuery()
		
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(MASTERMODE_COL);
		columnNames.push_back(CONDITION_COL);
		columnNames.push_back(ECSZONEID_COL);
		columnNames.push_back(EVACDIR_COL);
		columnNames.push_back(AIRDIR_COL);
		columnNames.push_back(SITUATION_COL);
        columnNames.push_back(DISPDIR_COL);
		columnNames.push_back(VERIFMSG_COL);
        columnNames.push_back(DATEMODIFIED_COL);
        columnNames.push_back(DATECREATED_COL);
		
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IECSMasterModes ECSMasterModePointerVector;
		
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i,KEY_COL);
                if (key != 0)
                {
					try
					{
						if (readWrite)
						{
							ECSMasterModePointerVector.push_back( new ConfigECSMasterMode(i, *data));
						}
						else
						{
							ECSMasterModePointerVector.push_back( new ECSMasterMode(i, *data));
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
		
		// distill the SQL query statement from the do-while(because the do-while is also a SQL query)
		// so as to set the every helper member of the ECSMasterMode and initialize objects completely
		for (IECSMasterModesIt itMaster = ECSMasterModePointerVector.begin(); itMaster != ECSMasterModePointerVector.end(); ++itMaster)
		{
			// Now using the MasterModeKey to load the associated StationModeKeys.
			std::vector<unsigned long> stationModeKeys = ECSStationModeAccessFactory::getInstance().getStationModeKeysByMasterModeKey( (*itMaster)->getKey() );
			
			for(std::vector<unsigned long>::iterator iter = stationModeKeys.begin(); iter != stationModeKeys.end(); ++iter)
			{
				(*itMaster)->setAssociatedStationMode(*iter,true);		//add the associated station mode key to the helper of master mode	
			}
			stationModeKeys.clear();					
		}
		
        return ECSMasterModePointerVector;
    }
	
	IECSMasterModes ECSMasterModeAccessFactory::getECSMasterModeDetails(std::vector< unsigned long > keylist)
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
		 //std::ostringstream sql;

		//sql << "select " 
  //          << KEY_COL << ","
  //          << MASTERMODE_COL << ","
  //          << CONDITION_COL << ","
  //          << "nvl(" << ECSZONEID_COL << ",-1) as " << ECSZONEID_COL << ","
  //          << EVACDIR_COL << ","
  //          << AIRDIR_COL << ","
  //          << DISPDIR_COL << ","
		//	<< VERIFMSG_COL << ","
		//	//<< "nvl(" << VERIFMSG_COL << ",'NA'), "
  //          << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
  //          << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
  //          << "from EC_MASTER_MODE where ECMMOD_ID in (" << keys.str() << ")";
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_Oracle_SELECT_50552, keys.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_SELECT_50552, keys.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50552, keys.str());

		std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(MASTERMODE_COL);
		columnNames.push_back(CONDITION_COL);
		columnNames.push_back(ECSZONEID_COL);
		columnNames.push_back(EVACDIR_COL);
		columnNames.push_back(AIRDIR_COL);
		columnNames.push_back(SITUATION_COL);
        columnNames.push_back(DISPDIR_COL);
		columnNames.push_back(VERIFMSG_COL);
        columnNames.push_back(DATEMODIFIED_COL);
        columnNames.push_back(DATECREATED_COL);
        

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IECSMasterModes ECSMasterModePointerVector;
		
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i,KEY_COL);
                if (key != 0)
                {   
					try
					{
						ECSMasterModePointerVector.push_back( new ECSMasterMode(i, *data));
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
		
		// distill the SQL query statement from the do-while(because the do-while is also a SQL query)
		// so as to set the every helper member of the ECSMasterMode and initialize objects completely
		for (IECSMasterModesIt itMaster = ECSMasterModePointerVector.begin(); itMaster != ECSMasterModePointerVector.end(); ++itMaster)
		{
			// Now using the MasterModeKey to load the associated StationModeKeys.
			std::vector<unsigned long> stationModeKeys = ECSStationModeAccessFactory::getInstance().getStationModeKeysByMasterModeKey( (*itMaster)->getKey() );
			
			for(std::vector<unsigned long>::iterator iter = stationModeKeys.begin(); iter != stationModeKeys.end(); ++iter)
			{
				(*itMaster)->setAssociatedStationMode(*iter,true);		//add the associated station mode key to the helper of master mode	
			}
			stationModeKeys.clear();					
		}
		
        return ECSMasterModePointerVector;
	}
	
    std::vector<unsigned long> ECSMasterModeAccessFactory::getECSMasterModeKeysWhere
		(	unsigned long masterMode, 
		const std::string& condition,
		int zoneID, 
		const std::string& evacuationDirection,
		const std::string& airflowDirection,
		const std::string& displayDirection )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		//int nDbType = databaseConnection->getDbServerType();
		
        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select ECMMOD_ID from EC_MASTER_MODE where 1=1 ";
		TA_Base_Core::SQLTypeAdapter sqlParam;
		
        if (masterMode != -1)
        {
            sql << "and MASTER_MODE = " << masterMode << " ";	
			sqlParam.add(sql.str());
			sql.str("");
        }
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
        if (zoneID != -1)
        {
            sql << "and ECZONE_ID = " << zoneID << " "; 			
        }
        if (!evacuationDirection.empty())
        {
            sql << "and EVACUATION_DIRECTION = '" 
                << (databaseConnection->escapeInsertString(evacuationDirection)).c_str()
                << "' ";			
        }
        if (!airflowDirection.empty())
        {
            sql << "and AIRFLOW_DIRECTION = '" 
                << (databaseConnection->escapeInsertString(airflowDirection)).c_str()
                << "' ";
        }
        if (!displayDirection.empty())
        {
            sql << "and DISPLAY_DIRECTION = '" 
                << (databaseConnection->escapeInsertString(displayDirection)).c_str()
                << "' ";
        }
		sqlParam.add(sql.str());
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_STD_SELECT_50502, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_SELECT_50502, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50502, sqlParam);
		
        std::vector<std::string> columnNames;
        columnNames.push_back("ECMMOD_ID");
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		
        // create the vector of keys
        std::vector<unsigned long> masterModeKeys;
		
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                masterModeKeys.push_back(data->getUnsignedLongData(i,"ECMMOD_ID"));
            }
			
            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );
		
        return masterModeKeys;
    }
    
	
    std::vector<unsigned long> ECSMasterModeAccessFactory::getECSMasterModeKeysWhereNot
		(	unsigned long masterMode, 
		const std::string& condition,
		int zoneID, 
		const std::string& evacuationDirection,
		const std::string& airflowDirection,
		const std::string& displayDirection )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		//int nDbType = databaseConnection->getDbServerType();
		
        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select ECMMOD_ID from EC_MASTER_MODE where 1=1 ";
		TA_Base_Core::SQLTypeAdapter sqlParam;
		
        if (masterMode != -1)
        {
            sql << "and MASTER_MODE <> " << masterMode << " ";

			sqlParam.add(sql.str());
			sql.str("");
        }
        if (!condition.empty())
        {
			/*switch ( nDbType )
			{
			case enumOracleDb:
				sql << "and CONDITION <> '"
				<< (databaseConnection->escapeInsertString(condition)).c_str() 
                << "' ";
				break;
			case enumMysqlDb:
				sql << "and `CONDITION` <> '"
					<< (databaseConnection->escapeInsertString(condition)).c_str() 
					<< "' ";
				break;
			}*/
			sqlParam.add(std::string("and "));
			sqlParam.add(TA_Base_Core::enumSyntax_Format_5);
			sqlParam.add(std::string(" <> '") + databaseConnection->escapeInsertString(condition) + std::string("' "));
            
        }
        if (zoneID != -1)
        {
            sql << "and ECZONE_ID <> " << zoneID << " "; 
        }
        if (!evacuationDirection.empty())
        {
            sql << "and EVACUATION_DIRECTION = '" 
                << (databaseConnection->escapeInsertString(evacuationDirection)).c_str()
                << "' ";
        }
        if (!airflowDirection.empty())
        {
            sql << "and AIRFLOW_DIRECTION <> '" 
                << (databaseConnection->escapeInsertString(airflowDirection)).c_str()
                << "' ";
        }
        if (!displayDirection.empty())
        {
            sql << "and DISPLAY_DIRECTION <> '" 
                << (databaseConnection->escapeInsertString(displayDirection)).c_str()
                << "' ";
        }
		sqlParam.add(sql.str());
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_MASTER_MODE_STD_SELECT_50502, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_SELECT_50502, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50502, sqlParam);
		
        std::vector<std::string> columnNames;
        columnNames.push_back("ECMMOD_ID");
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		
        // create the vector of keys
        std::vector<unsigned long> masterModeKeys;
		
        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                masterModeKeys.push_back(data->getUnsignedLongData(i,"ECMMOD_ID"));
            }
			
            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );
		
        return masterModeKeys;
    }
	
    
    IConfigECSMasterMode* ECSMasterModeAccessFactory::createECSMasterMode()
    {  
		try
		{
			return new ConfigECSMasterMode(); 
		}
		catch ( ... )
		{
			throw;
		}  
    }
	
	
    IConfigECSMasterMode* ECSMasterModeAccessFactory::copyECSMasterMode(const IConfigECSMasterMode* ECSMasterModeToCopy)
    {
        FUNCTION_ENTRY("copyECSMasterMode");
		
		if (NULL == ECSMasterModeToCopy)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterMode to copy was NULL." );
			return NULL;
		}
        
        const ConfigECSMasterMode* cast = dynamic_cast<const ConfigECSMasterMode*>(ECSMasterModeToCopy);
		
		if (NULL == cast)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ECSMasterMode passed could not be converted into a ConfigECSMasterMode" );
			return NULL;
		}
		
        FUNCTION_EXIT;
		try
		{
			return new ConfigECSMasterMode(*cast);
		}
		catch ( ... )
		{
			throw;
		}		
    }
	
	
} // closes TA_Core


