/**
  * The source code in this file is the property of 
  * MI Consulting Group (Australia) and is not for 
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/HistoryConfig.cpp $
  * @author Craig Grant
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * This class loads the info from the database for the History Configuration.
  * This class is a singleton.
  */

#pragma warning(disable:4786)   // 255 character limit on mangled name

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/HistoryConfig.h"
#include "app/history/history_viewer/src/DataBaseConnection.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/database/src/DBException.h"
#ifdef USE_OCCI
#include <occi.h>
#endif // USE_OCCI

using TA_Core::DebugUtil;

namespace TA_App
{
    //
    // LoadConfig
    //
    void HistoryConfig::LoadConfig()
    {
        if(m_Loaded == true)
        {
	        return;
        }
        m_Loaded = true;

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Loading Historian Configuration");
        // *** IMPORTANT ***
        // This may be replaced by calls to the historyconfig corba objects later...

        // SQL statement to get the required history parameters.
        std::string sql( "SELECT TableNames.pkey EntityKey, TableNames.name EntityName, TableNames.value TableName, "
	        "ShortDescription.value ShortDescription, LongDescription.value LongDescription, "
	        "TimePeriods.value TimePeriod, BaseDatas.value BaseData FROM "
	        "(SELECT e.pkey pkey, e.name name, ep.pkey parpkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey "
	        "AND ep.pkey = epv.parameterkey AND et.name='HistoryConfig' "
	        "AND ep.name='TableName') TableNames, "
	        "(SELECT e.pkey pkey, e.name name, ep.pkey parpkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey "
	        "AND ep.pkey = epv.parameterkey AND et.name='HistoryConfig' "
	        "AND ep.name='ShortDescription') ShortDescription, "
	        "(SELECT e.pkey pkey, e.name name, ep.pkey parpkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey "
	        "AND ep.pkey = epv.parameterkey AND et.name='HistoryConfig' "
	        "AND ep.name='TimePeriod') TimePeriods, "
	        "(SELECT e.pkey pkey, e.name name, ep.pkey parpkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey "
	        "AND ep.pkey = epv.parameterkey AND et.name='HistoryConfig' "
	        "AND ep.name='BaseData') BaseDatas, "
	        "(SELECT e.pkey pkey, e.name name, ep.pkey parpkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey "
	        "AND ep.pkey = epv.parameterkey AND et.name='HistoryConfig' "
	        "AND ep.name='LongDescription') LongDescription "
	        "WHERE TableNames.pkey=ShortDescription.pkey	AND TableNames.pkey=TimePeriods.pkey "
	        "AND TableNames.pkey=BaseDatas.pkey AND TableNames.pkey=LongDescription.pkey "
	        "ORDER BY TableNames.pkey" );

        HistConfig tempHistConfig;

#ifdef USE_OCCI
        // Get the oracle connection.  As errors in the oracle connection will have already
        // resulted in an error being displayed, don't display another one here.
        oracle::occi::Connection *conn;
        try
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Creating OCCI database connection");

	        conn = DataBaseConnection::getInstance().GetConnection();
        }
        catch(...)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not create database connection.");
	        return;
        }

        oracle::occi::Statement *stmt = NULL;
        oracle::occi::ResultSet *resultSet = NULL;
        try
        {
	        // Create the statement
	        stmt = conn->createStatement( sql );

	        // Execute the statement and put the results into ther result set
	        resultSet = stmt->executeQuery();

	        // Go through the results
	        while( resultSet->next() )
	        {
		        // Add configuration
		        tempHistConfig.entityKey = resultSet->getUInt( 1 );
		        tempHistConfig.entityName = resultSet->getString( 2 );
		        tempHistConfig.tableName = resultSet->getString( 3 );
		        tempHistConfig.shortDesc = resultSet->getString( 4 );
		        tempHistConfig.longDesc = resultSet->getString( 5 );
		        tempHistConfig.samplePeriod = atol(resultSet->getString( 6 ).c_str());
		        tempHistConfig.baseData = resultSet->getString( 7 );

		        m_HistConfigMap[tempHistConfig.entityKey] = tempHistConfig;
	        }

	        stmt->closeResultSet( resultSet );
	        conn->terminateStatement( stmt );
        }
        catch ( oracle::occi::SQLException &sqlException )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "SQLException", "Could not load datapoints");

	        if( resultSet )
	        {
		        stmt->closeResultSet( resultSet );
	        }
	        if( stmt )
	        {
		        conn->terminateStatement( stmt );
	        }

	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not load datapoints.");

	        // Log the Oracle Error Message (includes error number)
	        LOGMORE(SourceInfo, sqlException.getMessage().c_str());

	        // Throw exception
	        CouldNotLoadException e;
	        e.SetErrorMsg( sqlException.getMessage() ); // Copy, not reference
	        throw e;
        }
#else // USE_OCCI

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Creating SimpleDB database connection");

        SimpleDb* db = SimpleDBConnection::getInstance().GetConnection();
        SimpleDb::Buffer buffer;
        int columns = 7;

		try
		{
			db->Exec( sql.c_str(), columns, buffer );
		}
		catch( const DBException& e )
		{
			LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
		}

        if (buffer.empty())
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No History Configuration found");
	        return;
        }

        int rows = buffer.size();
        for (int i=0; i<rows; ++i)
        {
	        // Add configuration
	        tempHistConfig.entityKey = atol(buffer[i][0].c_str());
	        tempHistConfig.entityName = buffer[i][1];
	        tempHistConfig.tableName = buffer[i][2];
	        tempHistConfig.shortDesc = buffer[i][3];
	        tempHistConfig.longDesc = buffer[i][4];
	        tempHistConfig.samplePeriod = atol(buffer[i][5].c_str());
	        tempHistConfig.baseData = buffer[i][6];

	        m_HistConfigMap[tempHistConfig.entityKey] = tempHistConfig;
        }
#endif // USE_OCCI

        if (m_HistConfigMap.empty())
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No History Configuration found");
        }
    }


    // 
    // GetHistConfigMap
    //
    const HistConfigMap& HistoryConfig::GetHistConfigMap()
    {
        FUNCTION_ENTRY("GetHistConfigMap");

        return m_HistConfigMap;
    }

    
    // 
    // GetTableName
    //
    const std::string HistoryConfig::GetTableName( const unsigned long entityKey )
    {
        FUNCTION_ENTRY("GetTableName");

        std::string tableName;
        HistConfigMap::iterator it = m_HistConfigMap.find( entityKey );
        if( it != m_HistConfigMap.end() )
        {
	        tableName = it->second.tableName;
        }

        return( tableName );
    }

    
    // 
    // GetSamplePeriod
    //
    const long HistoryConfig::GetSamplePeriod( const unsigned long entityKey )
    {
        FUNCTION_ENTRY("GetSamplePeriod");

        long samplePeriod = 60;
        HistConfigMap::iterator it = m_HistConfigMap.find( entityKey );
        if( it != m_HistConfigMap.end() )
        {
	        samplePeriod = it->second.samplePeriod;
        }

        return( samplePeriod );
    }

    
    // 
    // AddShortDesc
    //
    void HistoryConfig::AddShortDesc(CComboBox& historyTypeCtrl)
    {
        FUNCTION_ENTRY("AddShortDesc");

        HistConfigMap::const_iterator it;
        for ( it = m_HistConfigMap.begin(); it != m_HistConfigMap.end(); it++ )
        {
	        int lastIndex = historyTypeCtrl.AddString( it->second.shortDesc.c_str() );
	        historyTypeCtrl.SetItemData( lastIndex, it->first );
        }

        FUNCTION_EXIT;
    }

} // TA_History


