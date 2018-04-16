/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/MinimumConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include <vector>
#include "app/history/scada_historian/src/MinimumConfig.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryConfigData.h"
#include "core/database/src/DBException.h"

using namespace TA_Core;


namespace TA_App
{
    //
    // MinimumConfig
    //
    MinimumConfig::MinimumConfig(IEntityData& entityData)
    throw(HistorianCfgErrorException)
    :HistoryConfig(entityData), m_database(NULL)
    {
		FUNCTION_ENTRY("MinimumConfig");
        LOG(SourceInfo, DebugUtil::FunctionEntry, "constructor");

        ScadaHistoryConfigData& historyEntityData = dynamic_cast <ScadaHistoryConfigData&> (entityData);

	    m_procedureName = historyEntityData.getProcedureName();

	    if(m_procedureName == "")
	    {
		    logError("ProcedureName", MISSING);
            throw HistorianCfgErrorException("Missing Configuration Parameter", 
                HistorianCfgErrorException::MissingParam, m_entityKey);
	    }

	    std::string dbStr = RunParams::getInstance().get(RPARAM_DBCONNECTION);
	    std::vector<std::string> dbComps;
	    SimpleDb::ParseConnectStr(dbStr, dbComps);

		try
		{
			m_database = new SimpleDb(dbComps[0].c_str(), dbComps[1].c_str(), dbComps[2].c_str()); 
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );

			// there doesn't seem to be an appropriate Historian exception. This is the closest thing:
			throw HistorianCfgErrorException("Unable to create db connection", 
                HistorianCfgErrorException::InvalidParam, m_entityKey);
		}


		FUNCTION_EXIT;
    }


    //
    // ~MinimumConfig
    //
    MinimumConfig::~MinimumConfig()
    {
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
            m_database = NULL;
        }
    }


    //
    // process
    //
    void MinimumConfig::process(time_t timestamp)
    {
        //
        // This is commented out for C830 because we only need sample data.
        // If other projects require minimums to be calculated, ensure that
        // there are database procedures to do this, and uncomment the
        // code below.
        //

        /*
	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Minimum Processing...");

	    std::string sql("Call ");
	    sql += m_procedureName + "()";

		try
		{
			m_database->Open();
			m_database->Exec(sql.c_str());
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary.c_str() );

			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				"Database Communication Problem. Could not insert averages into database.");
		}

		try
		{
			m_database->Close();
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary.c_str() );
		}
	    
        */
    }

} // TA_App
