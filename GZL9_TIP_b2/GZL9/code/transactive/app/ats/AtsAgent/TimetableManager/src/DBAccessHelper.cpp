/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: g:\depot\GZL6_TIP\3002\transactive\app\ats\AtsAgent\SignallingProcessors\src\DBAccessHelper.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/17 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/ats/AtsAgent/TimetableManager/src/DBAccessHelper.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	DBAccessHelper::DBAccessHelper()
	{
		FUNCTION_ENTRY("DBAccessHelper");
		FUNCTION_EXIT;
	}

	DBAccessHelper::~DBAccessHelper()
	{
		FUNCTION_ENTRY("DBAccessHelper");
		FUNCTION_EXIT;
	}

	void DBAccessHelper::createPlannedTrainData( PlannedTrainDataInfo* plannedData )
	{
		FUNCTION_ENTRY("createPlannedTrainData");

		AtsPlannedDataFactory::getInstance().createPlannedTrainData(*plannedData);

		FUNCTION_EXIT;
	}

	void DBAccessHelper::createPraticalTrainData( PracticalTrainDataInfo* practicalData )
	{
		FUNCTION_ENTRY("createPraticalTrainData");

		AtsPracticalDataFactory::getInstance().createPracticalTrainData(*practicalData);

		FUNCTION_EXIT;
	}
}