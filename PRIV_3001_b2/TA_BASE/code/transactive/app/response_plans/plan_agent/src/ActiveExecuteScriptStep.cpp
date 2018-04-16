
#include "app/response_plans/common/src/ProjectDefinition.h"
#if defined (STEPTYPE_EXECUTE_SCRIPT)
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveLogStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2009/12/29 14:17:48 $
  *  Last modified by:  $Author: grace.koh $
  *
  *  <description>
  */
#include <iostream>

#include "core/uuid/src/TAuuid.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveExecuteScriptStep.h"


namespace TA_Base_App
{
    const static ACE_Time_Value timeToSleep(0, 300);

    ActiveExecuteScriptStep::ActiveExecuteScriptStep( const TA_Base_Core::StepDetail& stepDetail, const TA_Base_Core::ExecuteScriptParameters& params, IActivePlanFlow& flowControl ) :
        ActiveStep(stepDetail, flowControl),
        m_params(params),
        m_flowControl(flowControl)
    {

    }

    TA_Base_Core::EActiveStepState ActiveExecuteScriptStep::executeImpl( void )
    {
        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        std::string commandline = m_params.commands.in();
        std::string fileName = TA_Base_Core::TA_Uuid::generateUUID();

        std::string commandlineRefined = commandline + " 1>" + fileName + " 2>&1"; 

        if ( 0 != WEXITSTATUS(ACE_OS::system(commandlineRefined.c_str())))
        {
            std::ifstream fileOpen(fileName.c_str(), ios::in|ios::binary);
            std::stringstream fileContent;

            fileContent << fileOpen.rdbuf();

            m_remark = fileContent.str();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan step execute script error: command '%s' error msg: %s", commandline.c_str(), fileContent.str().c_str());

            fileOpen.close();
            
            if ( 0 == remove(fileName.c_str()))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Del temp file  %s", fileContent.str().c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Del temp file  %s fail", fileContent.str().c_str());
            }

            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
            return  stepState;
        }

        m_flowControl.setNextStep();
        return stepState;
    }
}


#endif // STEPTYPE_PLAN_START_TIME_CHECK