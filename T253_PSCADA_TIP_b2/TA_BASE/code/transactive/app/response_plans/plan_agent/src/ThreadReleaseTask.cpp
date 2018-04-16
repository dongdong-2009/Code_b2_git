// ThreadReleaseTask.cpp: implementation of the ThreadReleaseTask class.
//
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ThreadReleaseTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{

ThreadReleaseTask::ThreadReleaseTask() : ACE_Method_Request()
{
}

int ThreadReleaseTask::call()
{
	return ReleaseThread;
}

}
