/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgent.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#include "app/radio/RadioTetraAgent/src/RadioServerAgent.h"
//#include "app/radio/RadioTetraAgent/src/RadioServerAgentSummary.h"

#include "bus/generic_agent/src/GenericAgent.h"


#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;
RadioServerAgent::RadioServerAgent() :
	m_inControlMode(false), m_commandServant(NULL)
{
	FUNCTION_ENTRY("Constructor");


	FUNCTION_EXIT;
}


RadioServerAgent::~RadioServerAgent()
{
	FUNCTION_ENTRY("Destructor");

	FUNCTION_EXIT;
}

//void RadioServerAgent::setSummary(RadioServerAgentSummary *summary)
//{
//    m_pServerAgentSummary = summary;
//}

void RadioServerAgent::setCommandServant(RadioServerAgentCommandServant* servant)
{
    m_commandServant = servant;
}
    

void RadioServerAgent::onMonitorMode()
{
	FUNCTION_ENTRY("onMonitorMode");

    if (m_commandServant != NULL)
    {
        if ( m_inControlMode )
	    {
		    m_inControlMode = false;
            m_commandServant->setToMonitorMode();
	    }
    }

	FUNCTION_EXIT;
}


void RadioServerAgent::onTerminate()
{
	FUNCTION_ENTRY("onTerminate");

    if ( m_inControlMode )
    {
        m_inControlMode = false;
	}

    if (m_commandServant != NULL)
    {
        m_commandServant->stop();
    }

	FUNCTION_EXIT;
}


void RadioServerAgent::onControlMode()
{
	FUNCTION_ENTRY("onControlMode");

    if (m_commandServant != NULL)
    {
        if (!m_inControlMode)
        {
            m_inControlMode = true;
            m_commandServant->setToControlMode();
        }
    }

	FUNCTION_EXIT;
}


