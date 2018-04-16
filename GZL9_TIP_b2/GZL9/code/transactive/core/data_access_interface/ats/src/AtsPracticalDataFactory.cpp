/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: \\depot\GZL6_TIP\3002\transactive\core\data_access_interface\ats\src\AtsPracticalDataFactory.cpp $
 * @author:  Lucky CUizon
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/06/14 09:50:03 $
 * Last modified by:  $Author: Lucky Cuizon$
 * 
 * AtsPracticalDataFactory is a singleton that is used to set/retrieve planned objects from the database.
 */

#include "core/data_access_interface/ats/src/AtsPracticalDataFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>

namespace TA_IRS_Core
{
	AtsPracticalDataFactory* AtsPracticalDataFactory::m_Instance = 0;

	AtsPracticalDataFactory& AtsPracticalDataFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");

		if ( m_Instance == 0 )
		{
			m_Instance = new AtsPracticalDataFactory;
		}

		FUNCTION_EXIT;
		return *m_Instance;
	}

	void AtsPracticalDataFactory::createPracticalTrainData(const PracticalTrainDataInfo& info)
	{
		FUNCTION_ENTRY("createPlannedTrainData");

		//get a connection to the Database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		/*std::ostringstream sqlProcedureCall;
		sqlProcedureCall << "call INSERT_ATS_PRACTICAL_DATA('" << info.strTrainID;
		sqlProcedureCall << "', '" << info.strServiceID; 
 		sqlProcedureCall << "', '" << info.strDestinationID; 
		sqlProcedureCall << "', '" << info.strStationID;
		sqlProcedureCall << "', '" << info.strPlatformID;
		sqlProcedureCall << "', TO_DATE('" << info.strATime;
		sqlProcedureCall << "','DD-MM-YYYY HH24:MI:SS'), TO_DATE('" << info.strDTime;
		sqlProcedureCall << "','DD-MM-YYYY HH24:MI:SS'), '" << info.strType;
		sqlProcedureCall << "', '" << info.strFlag;
		sqlProcedureCall << "', '" << info.strReserved;
 		sqlProcedureCall << "', TO_DATE('" << info.strDate << "','DD-MM-YYYY HH24:MI:SS')  ) ";*/

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"\n\n\tInsert ATS Practical Data to Database :"
			"\n\t\t TrainID        : %s"
			"\n\t\t ServiceID      : %s"
			"\n\t\t DestinationID  : %s"
			"\n\t\t StationID      : %s"
			"\n\t\t PlatformID     : %s"
			"\n\t\t Arrival Time   : %s"
			"\n\t\t Departure Time : %s"
			"\n\t\t Type           : %s"
			"\n\t\t Flag           : %s"
			"\n\t\t Reserved       : %s"
			"\n\t\t DATE           : %s\n",info.strTrainID.c_str(),info.strServiceID.c_str(),
			info.strDestinationID.c_str(),info.strStationID.c_str(),info.strPlatformID.c_str(),
			info.strATime.c_str(),info.strDTime.c_str(),info.strType.c_str(),info.strFlag.c_str(),
			info.strReserved.c_str(), info.strDate.c_str() );

		TA_Base_Core::SQLStatement sqlProcedureCall;
		databaseConnection->prepareSQLStatement(sqlProcedureCall,AT_PSD_ENTITY_MAP_INSERT_49053,info.strTrainID,info.strServiceID,info.strDestinationID,
			info.strStationID,info.strPlatformID,info.strATime,info.strDTime,info.strType,info.strFlag,info.strReserved,info.strDate);
		//Execute the stored procedure
		databaseConnection->executeProcedure(sqlProcedureCall);

		FUNCTION_EXIT;
	}
}