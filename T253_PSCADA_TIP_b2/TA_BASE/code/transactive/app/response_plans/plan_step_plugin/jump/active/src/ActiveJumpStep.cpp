
#include "core/utilities/src/DebugUtil.h"
#include "ActiveJumpStep.h"

namespace TA_Base_App
{
    ActiveJumpStep::ActiveJumpStep(const BasicParameter& detail, IActivePlan& flowControl) :  ActiveStep(m_params, flowControl),
    m_params(detail)
    {
    }

    ActiveJumpStep::~ActiveJumpStep()
    {
    }

    bool ActiveJumpStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: jumping to step %lu", dumpContext().c_str(), m_params.stepId );
        return true;
    }

	std::list<ActiveStep*> ActiveJumpStep::getNextStep()
    {
		std::list<ActiveStep*> outgoingStepList;

		for( std::list<ActiveStep*>::iterator itLoop = m_lstOutgoingStep.begin(); itLoop != m_lstOutgoingStep.end(); ++itLoop )
		{
			if( m_params.stepId == (*itLoop)->id() )
			{
				outgoingStepList.push_back( *itLoop );
				break;
			}
		}

	    return outgoingStepList;
	}
}