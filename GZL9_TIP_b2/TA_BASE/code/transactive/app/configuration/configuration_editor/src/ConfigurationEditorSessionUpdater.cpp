/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_TA_BASE_DDB/transactive/app/configuration/configuration_editor/src/ConfigurationEditorSessionUpdater.cpp $
  * @author:  Marc Oliver Castaneda
  * @version: $Revision: 0 $
  *
  * 
  * This class is used by the worker thread in ConfigurationEditor.cpp to update
  * the timestamp in the database
  * 
  */

#include <string>
#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorSessionUpdater.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DataTypeEnums.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

	ConfigurationEditorSessionUpdater::ConfigurationEditorSessionUpdater()
			/*:sql("update config_editor_timestamp set last_timestamp = current_timestamp")*/
	{
		databaseConnection =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, 
				TA_Base_Core::Write);

	}

	ConfigurationEditorSessionUpdater::~ConfigurationEditorSessionUpdater()
	{
	}
	
	bool ConfigurationEditorSessionUpdater::updateConfigSessionTimeStamp()
	{
        FUNCTION_ENTRY("updateConfigSessionTimeStamp");
		OutputDebugString("> updateConfigSessionTimeStamp");

		try
		{

            //databaseConnection->executeModification(sql);

            //waitforcheck delete by lin		
            TA_Base_Core::SQLStatement strSql;
            databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_UPDATE_20507);
            databaseConnection->executeModification(strSql);     
		}
		catch(...)
		{
			//If there was an exception, handle it quietly.
			OutputDebugString("Error updating session in database");
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Error updating timestamp in config_editor_timestamp table ");
			return false;

		}

		return true;
	}
}


