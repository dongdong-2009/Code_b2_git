/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgent.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgent.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

RadioMFTAgent::RadioMFTAgent() : m_pCommandImpl(NULL)
{

}


RadioMFTAgent::~RadioMFTAgent()
{

}

void RadioMFTAgent::setServant(RadioMFTAgentCommandServant* pServant)
{
    m_pCommandImpl = pServant->getImplementation();

}

void RadioMFTAgent::initialisation()
{
    //change application state to
	//initialisation, and notify client(s)
	//obtain any required entity configuration
	//establish session to Mconsole, Mcdb
	//refresh ResourceConfig API information
	//operatorSessionInitialisation()
    FUNCTION_ENTRY("initialisation");

	m_ApStateMgr.setApState(RA_INITIALISATION_MODE);

	m_pCommandImpl->setApStateMgr(&m_ApStateMgr);
	m_pCommandImpl->callback_initialisation();

    FUNCTION_EXIT;
}


void RadioMFTAgent::termination()
{
    FUNCTION_ENTRY("termination");

    try
    {
    	m_pCommandImpl->callback_termination();
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Error while terminating RadioMFTAgent");
    }

    FUNCTION_EXIT;
}


void RadioMFTAgent::agentSetMonitor()
{
    FUNCTION_ENTRY("agentSetMonitor");

    FUNCTION_EXIT;    
}


void RadioMFTAgent::agentTerminate()
{
    FUNCTION_ENTRY("agentTerminate");
    FUNCTION_EXIT;    
}


void RadioMFTAgent::agentSetControl()
{
    FUNCTION_ENTRY("agentSetControl");

    //do nothing

    FUNCTION_EXIT;
}


void RadioMFTAgent::operatorSessionInitialisation()
{
    FUNCTION_ENTRY("operatorSessionInitialisation");
    FUNCTION_EXIT;    
}
