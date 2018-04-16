/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/ml_algorithm/active/src/ActivePCAStep.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/25 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Active step class derived from based class MLActiveStep for PCA ML algorithm.
*/


#include "app/response_plans/plan_step_plugin/ml_algorithm/active/src/ActivePCAStep.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"

#include "bus/response_plans/PlanDataAccess/src/DataMatrixPara.h"
#include "bus/response_plans/PlanDataAccess/src/DataPointVectorPara.h"

namespace TA_Base_App
{
    ActivePCAStep::ActivePCAStep( const BasicParameter& detail, IActivePlan& plan ) 
		: MLActiveStep(m_params, plan), m_params(detail)
    {
    }

    ActivePCAStep::~ActivePCAStep()
    {
    }

    bool ActivePCAStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "PCA step %lu", id() );

        bool bExecSucceed = true;

		// TODO: execute ML algorithm and passing the required input parameter(s), return output parameter(s)
		std::vector<std::string> vecParameters = m_params.serialize();

        return bExecSucceed;
    }

	void ActivePCAStep::initImpl()
	{
		// prepare input data
		// get required input data from Start step output parameter(s) if next step after Start step.
	}

}