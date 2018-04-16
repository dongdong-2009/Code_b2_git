/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsAgentSummary.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/09/27 14:47:02 $
  * Last modified by:  $Author: huangjian $
  *
  */
///////////////////////////////////////////////////////////
//  CtaEcsAgentSummary.cpp
//  Implementation of the Class CtaEcsAgentSummary
//  Created on:      13-Apr-2005 05:36:17 PM
//  Original author: Sean Liew
///////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER


#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/ecs/ECSAgent/src/CtaEcsAgentSummary.h"

using TA_Base_Core::DebugUtil;

CtaEcsAgentSummary::CtaEcsAgentSummary()
: m_ctaEcsComms(NULL),
  m_ctaEcsAgentCompleteState(NULL),
  m_ctaEcsCachedConfig(NULL),
  m_ctaEcsSynchronisation(NULL),
  m_ctaEcsAuditing(NULL),
  m_ctaEcsAlarms(NULL),
  m_ctaEcsCachedMaps(NULL),
  m_ctaEcsDatapoints(NULL)
{
	FUNCTION_ENTRY("CtaEcsAgentSummary");

}

void CtaEcsAgentSummary::setup()
{
    FUNCTION_ENTRY("setup");
	try
	{
		m_ctaEcsCachedMaps = new CtaEcsCachedMaps();
		m_ctaEcsCachedConfig = new CtaEcsCachedConfig(*m_ctaEcsCachedMaps);
		m_ctaEcsAuditing = new CtaEcsAuditing(*m_ctaEcsCachedConfig);
		m_ctaEcsComms = new CtaEcsComms(*m_ctaEcsCachedConfig);
		m_ctaEcsSynchronisation = new CtaEcsSynchronisation(*m_ctaEcsCachedConfig);
		m_ctaEcsAlarms = new CtaEcsAlarms(*m_ctaEcsCachedConfig, *m_ctaEcsCachedMaps);
		m_ctaEcsAgentCompleteState = new CtaEcsAgentCompleteState(*m_ctaEcsComms, *m_ctaEcsSynchronisation, *m_ctaEcsCachedConfig);
		m_ctaEcsDatapoints = new CtaEcsDatapoints(*m_ctaEcsCachedMaps, *m_ctaEcsCachedConfig);
	}
	catch ( ... )
	{
		LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Unknown Exception");
	}
    FUNCTION_EXIT;
}

void CtaEcsAgentSummary::cleanup()
{
	FUNCTION_ENTRY("cleanup");

	try
    {
        if (m_ctaEcsDatapoints)
        {
            delete m_ctaEcsDatapoints;
            m_ctaEcsDatapoints = NULL;
        }        
		if (m_ctaEcsAgentCompleteState)
        {
            delete m_ctaEcsAgentCompleteState;
            m_ctaEcsAgentCompleteState = NULL;
        }
        if (m_ctaEcsAlarms)
        {
            delete m_ctaEcsAlarms;
            m_ctaEcsAlarms = NULL;
        }
        if (m_ctaEcsSynchronisation)
        {
            delete m_ctaEcsSynchronisation;
            m_ctaEcsSynchronisation = NULL;
        }
        if (m_ctaEcsComms)
        {
            delete m_ctaEcsComms;
            m_ctaEcsComms = NULL;
        }
        if (m_ctaEcsAuditing)
        {
            delete m_ctaEcsAuditing;
            m_ctaEcsAuditing = NULL;
        }
        if (m_ctaEcsCachedConfig)
        {
            delete m_ctaEcsCachedConfig;
            m_ctaEcsCachedConfig = NULL;
        }
        if (m_ctaEcsCachedMaps)
        {
            delete m_ctaEcsCachedMaps;
            m_ctaEcsCachedMaps = NULL;
        }
    }
    catch(...)
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Unknown Exception");
    }
	FUNCTION_EXIT;
}
void CtaEcsAgentSummary::setControl(bool isOnStartup)
{
	FUNCTION_ENTRY("setControl");

    m_ctaEcsDatapoints->setControl(isOnStartup);
    m_ctaEcsAgentCompleteState->setControl(isOnStartup);
    AbstractFailoverable::setControl(isOnStartup);
	FUNCTION_EXIT;
}
void CtaEcsAgentSummary::setMonitor(bool isOnStartup)
{
	FUNCTION_ENTRY("setMonitor");

    AbstractFailoverable::setMonitor(isOnStartup);
    m_ctaEcsAgentCompleteState->setMonitor(isOnStartup);
    m_ctaEcsDatapoints->setMonitor(isOnStartup);
	FUNCTION_EXIT;
}



CtaEcsAgentSummary::~CtaEcsAgentSummary()
{
	FUNCTION_ENTRY("~CtaEcsAgentSummary");

	// if not already done.
	cleanup();
	FUNCTION_EXIT;
}


