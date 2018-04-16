/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TisPidListStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the PID List parameter of a Station Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "TisPidListStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "PlanStepParameterTisPidListEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TisPidListStepParameter
TisPidListStepParameter::TisPidListStepParameter(PlanStep& parent, const std::string& name, std::vector<std::string>& pids) :
    PlanStepParameter(parent, name, pids)
{
    FUNCTION_ENTRY( "TisPidListStepParameter" );
    FUNCTION_EXIT;
}

TisPidListStepParameter::~TisPidListStepParameter()
{
    FUNCTION_ENTRY( "~TisPidListStepParameter" );
    FUNCTION_EXIT;
}

void TisPidListStepParameter::resetPids()
{
    m_refSrcValue.clear();
}

bool TisPidListStepParameter::editParameter(const std::string& stationName)
{
    FUNCTION_ENTRY( "editParameter" );

    CPlanStepParameterTisPidListEditDlg editDlg(stationName, *this, true);

    if (editDlg.DoModal() == IDOK)
    {
        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


IPlanStepParameterForm* TisPidListStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );

    TA_ASSERT(false, "Function not supported, use editParameter(std::string stationName) instead");

    FUNCTION_EXIT;
    return NULL;
}

std::vector<std::string> TisPidListStepParameter::getStepParameterValues()
{
	std::vector<std::string> parameterValueVect;
    if (m_refSrcValue.size() == 0)
    {
		parameterValueVect.push_back("");
		return parameterValueVect;
    }

    // The list of PIDs may be extensive, so don't insert it as a single row as it's
    // going to be impossible to read. Instead break up the PIDs into multiple rows, with
    // a couple PIDs per each row. Only the first row is labelled with the parameter name.
    const unsigned long PIDS_PER_ROW(3);

	for (std::vector<std::string>::iterator itPid = m_refSrcValue.begin(); itPid != m_refSrcValue.end(); ++itPid )
    {
        //std::string label(i < PIDS_PER_ROW? getName() : "");
        std::string pidStr("");

        for (unsigned long cnt(0); cnt < PIDS_PER_ROW; cnt++)
        {
            if (!pidStr.empty())
            {
                pidStr += "  "; // separate the PIDs with a white space
            }

            pidStr += (*itPid);
        }

        parameterValueVect.push_back(pidStr);
    }

	return parameterValueVect;
}

const std::string TisPidListStepParameter::displayValue() const
{
	return "";
}