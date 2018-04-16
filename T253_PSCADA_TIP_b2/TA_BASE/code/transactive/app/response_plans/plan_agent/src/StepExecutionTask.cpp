// StepExecutionTask.cpp: implementation of the StepExecutionTask class.
//
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/StepExecutionTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{

//StepExecutionTask::StepExecutionTask( ActiveStep& refExecution ) : ACE_Method_Request(),
//m_refExecution( refExecution )
//{
//}

StepExecutionTask::StepExecutionTask( ActivePlan& refExecution ) : ACE_Method_Request(),
m_refExecution( refExecution )
{
}

int StepExecutionTask::call()
{
    //LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Step [%d] started", m_refExecution.id() );

    return m_refExecution.runStep();

    try
    {
        //while ( !m_refExecution.completed() )
        //{
        //    m_refExecution.execute();
        //}

        //m_refExecution.complete();
    }
    catch( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when execute step" );
    }

    //LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Step [%d] completed", m_refExecution.id() );

	return 0;
}

}
