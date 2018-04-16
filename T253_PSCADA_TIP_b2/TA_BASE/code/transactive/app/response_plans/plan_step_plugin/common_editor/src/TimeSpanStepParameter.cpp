/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TimeSpanStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents a generic time span parameter (e.g. as used by TIS and PA steps for message timeSpan).
  * The supported time resolutions are: days, hours, minutes, seconds.
  *
  **/

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "TimeSpanStepParameter.h"


namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// TimeSpanStepParameter

TimeSpanStepParameter::TimeSpanStepParameter(PlanStep& parent, const std::string& name, unsigned long& number, ETimeResolution resolution, bool isHidden) :
NumericStepParameter(parent, name, number, isHidden),
m_resolution(resolution)
{
    FUNCTION_ENTRY( "TimeSpanStepParameter" );
    FUNCTION_EXIT;
}

TimeSpanStepParameter::~TimeSpanStepParameter()
{
    FUNCTION_ENTRY( "~TimeSpanStepParameter" );
    FUNCTION_EXIT;
}

std::vector<std::string> TimeSpanStepParameter::getStepParameterValues()
{
	std::ostringstream timeSpanStr;

	if (m_resolution != TR_UNDEFINED)
	{
		// Convert to text based on the resolution.
		// 
		// The general format is: "D days H hours M minutes S seconds"
		// 
		// If the value of any given unit is 0 then that unit is omitted.

		int timeSpanInUnits = static_cast<int>(m_refSrcValue);
		int days(m_resolution == TR_DAYS? timeSpanInUnits : 0);
		int hours(m_resolution == TR_HOURS? timeSpanInUnits : 0);
		int minutes(m_resolution == TR_MINUTES? timeSpanInUnits : 0);
		int seconds(m_resolution == TR_SECONDS? timeSpanInUnits : 0);

		timeSpanStr.str("");

		if (days > 0 || timeSpanInUnits == 0 && m_resolution == TR_DAYS)
		{
			timeSpanStr << days << " " << (days == 1? "day" : "days") << " ";
		}

		if (hours > 0 || timeSpanInUnits == 0 && m_resolution == TR_HOURS)
		{
			timeSpanStr << hours << " " << (hours == 1? "hour" : "hours") << " ";
		}

		if (minutes > 0 || timeSpanInUnits == 0 && m_resolution == TR_MINUTES)
		{
			timeSpanStr << minutes << " " << (minutes == 1? "minute" : "minutes") << " ";
		}

		if (seconds > 0 || timeSpanInUnits == 0 && m_resolution == TR_SECONDS)
		{
			timeSpanStr << seconds << " " << (seconds == 1? "second" : "seconds");
		}
	}

	std::vector<std::string> parametersVect;
	parametersVect.push_back(timeSpanStr.str());
	return parametersVect;
}

}