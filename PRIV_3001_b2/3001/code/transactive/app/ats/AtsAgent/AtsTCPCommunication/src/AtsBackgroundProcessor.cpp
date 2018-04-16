
#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsBackgroundProcessor.h"
#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsTcpServerWrapper.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

AtsBackgroundProcessor::AtsBackgroundProcessor(AtsTcpServerWrapper* ats)
:m_atsTcpServerWrapper(*ats),m_threadPool(TA_Base_Core::ThreadPoolSingletonManager::getInstance())
{
	FUNCTION_ENTRY("AtsBackgroundProcessor");

    //not enough threads? let it grow
    m_threadPool->setUpperLimit( 6000 );

    FUNCTION_EXIT;
}

AtsBackgroundProcessor::~AtsBackgroundProcessor()
{
	FUNCTION_ENTRY("~AtsBackgroundProcessor");

    TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPool);
    
    FUNCTION_EXIT;
}