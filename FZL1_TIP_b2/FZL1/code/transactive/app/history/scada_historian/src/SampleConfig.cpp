/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/SampleConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include <vector>
#include "app/history/scada_historian/src/SampleConfig.h"
#include "app/history/scada_historian/src/DataPointCache.h"
#include "app/history/scada_historian/src/DataPointData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/database/src/SimpleDb.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryConfigData.h"
#include "core/database/src/DBException.h"

using namespace TA_Core;

// Cannot include full ACE header because of conflicting typedef between
// fstream.h (which ACE includes) and oratypes.h (which SimpleDb imports).
// We're going to trick the OS.h file that we have already included fstream.h
#define _INC_FSTREAM
#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"


namespace TA_App
{
    
    //
    // SampleConfig
    //
    SampleConfig::SampleConfig(IEntityData& entityData)
    throw(HistorianCfgErrorException)
    :HistoryConfig(entityData)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "constructor");

        ScadaHistoryConfigData& historyEntityData = dynamic_cast <ScadaHistoryConfigData&> (entityData);

	    m_procedureName = historyEntityData.getProcedureName();

	    if(m_procedureName == "")
	    {
            logError("ProcedureName", MISSING);
		    throw HistorianCfgErrorException("Missing Configuration Parameter", 
                HistorianCfgErrorException::MissingParam, m_entityKey);
	    }

	    std::string dbConnection = RunParams::getInstance().get(RPARAM_DBCONNECTION);
	    std::vector<std::string> components;
	    SimpleDb::ParseConnectStr(dbConnection, components);

		try
		{
			m_database = new SimpleDb(components[0].c_str(), components[1].c_str(), components[2].c_str()); 
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );

			// there doesn't seem to be an appropriate Historian exception. This is the closest thing:
			throw HistorianCfgErrorException("Unable to create db connection", 
                HistorianCfgErrorException::InvalidParam, m_entityKey);
		}

        
        
        //
	    // Create a data inserter
        //
	    m_dataInserter = new ArrayInserter(m_tableName.c_str(), components[0].c_str(), components[1].c_str(), components[2].c_str());
        m_dataInserter->Open();

	    LOG(SourceInfo, DebugUtil::FunctionExit, "constructor");
    }


    //
    // ~SampleConfig
    //
    SampleConfig::~SampleConfig()
    {
	    m_dataInserter->Close();

        if(m_dataInserter != NULL)
        {
	        delete m_dataInserter;
            m_dataInserter = NULL;
        }

		if( m_database != NULL )
		{
			try
			{
				delete m_database;
			}
			catch( const DBException& e )
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
			}
		}

        LOG(SourceInfo, DebugUtil::FunctionExit, "~SampleConfig...");
    }


    //
    // process
    //
    void SampleConfig::process(time_t timestamp)
    {
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm, "Sample Processing...");

        //
	    // Get all the data points stored in the cache
	    // that have data collection turned on
        //
	    std::vector<DataPointData> dataPoints;
	    DataPointCache::getInstance().getAll(dataPoints, true);

        //
	    // Only insert if there is data to insert
        //
	    if(dataPoints.size() == 0)
	    {
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "No data with Trending Enabled in cache.");
		    return;
	    }
	    
        //
	    // Open database and insert the data
        //
	    if(!m_dataInserter->insert(dataPoints, timestamp))
	    {
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
			    "Database Communication Problem. Could not insert all data points into database.");
	    }
	    
        //
	    // Convert current time to a string
        //
	    struct tm t;
	    char str[22] = {0};
	    if (ACE_OS::gmtime_r(&timestamp, &t) != NULL)
	    {
		    sprintf(str, "%02d/%02d/%04d %02d:%02d:%02d", t.tm_mday, t.tm_mon + 1, 
			    t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
	    }

	    std::string timeString = str;

        //
	    // Call the archiving procedure
        // Anita: 5/09/03: Not for C830
        //
        // Anita: 9/09/03: This is called to delete data from the table,
        // but the archiving call has been removed from the procedure
        //
        // Anita: 30/09/03: Archiving and deleting will be done independently
        // for C830 so the archiving procedure call has been commented out.
        // Other projects may require this call.
        //
        // Anita: 8/10/03: Database guy (MattC) has decided that the procedure
        // WILL be used to do the deletion, but will not be doing any archiving.
        //
	    std::string sql("Call ");
	    sql += m_procedureName + "('" + timeString + "')";
	    
		try
		{
			m_database->Exec(sql.c_str());
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
			    "Database Communication Problem. Could not call archiving procedure for Base table.");
		}
    }

} // TA_App

