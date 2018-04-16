/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StationStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the Station parameter of a Station TIS/PA step.
  *
  **/

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "StationStepParameter.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// StationStepParameter

StationStepParameter::StationStepParameter(PlanStep& parent, const std::string& name, unsigned long& locationKey, const std::string& station, const bool allStations) :
PlanStepParameter( parent, name, locationKey ),
m_station( station ),
m_allStations( allStations )
{
    FUNCTION_ENTRY( "StationStepParameter" );
    FUNCTION_EXIT;
}

StationStepParameter::~StationStepParameter()
{
    FUNCTION_ENTRY( "~StationStepParameter" );
    FUNCTION_EXIT;
}

void StationStepParameter::setStation(const std::string& station, unsigned long ulLocationkey )
{
    FUNCTION_ENTRY( "setStation" );

    m_station = station;
    m_refSrcValue = ulLocationkey;
    m_allStations = false;

    FUNCTION_EXIT;
}

void StationStepParameter::setAllStations()
{
    FUNCTION_ENTRY( "setAllStations" );

    m_station = "";
    m_refSrcValue = 0;
    m_allStations = true;

    FUNCTION_EXIT;
}

IPlanStepParameterForm* StationStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg(*this, canEdit);
}

std::vector<std::string> StationStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(m_allStations? "All stations" : m_station);
	return parametersVect;
}

std::string StationStepParameter::getStation() const
{
	return m_station;
}

bool StationStepParameter::getAllStations() const
{
	return m_allStations;
}

const std::string StationStepParameter::displayValue() const
{
    return m_allStations? "All stations" : m_station;
}

}