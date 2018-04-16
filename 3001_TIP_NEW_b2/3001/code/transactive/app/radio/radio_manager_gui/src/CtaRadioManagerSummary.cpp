/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/CtaRadioManagerSummary.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/CtaRadioManagerSummary.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"

#include "core/message/types/RadioAudit_MessageTypes.h"

#include "core/utilities/src/DebugUtil.h"


CtaRadioManagerSummary::CtaRadioManagerSummary(TA_Base_Core::IEntityDataPtr agentConfiguration) :
      CtaRuntime(agentConfiguration),
	  CtaAlarms(agentConfiguration),
	  CtaAuditing(agentConfiguration->getKey(), TA_Base_Core::RadioAudit::Context)
{
	m_RadioMFTAgentCallbackSubscriber   = new RadioMFTAgentCallbackSubscriber();
}

CtaRadioManagerSummary::~CtaRadioManagerSummary()
{
	delete m_RadioMFTAgentCallbackSubscriber;
	m_RadioMFTAgentCallbackSubscriber = NULL;
}


void CtaRadioManagerSummary::Subscribe()
{
    FUNCTION_ENTRY("getRadioMFTAgentCallbackSubscriber");

	The_RadioSystemStatus::instance()->registerToMFT(m_RadioMFTAgentCallbackSubscriber);

    FUNCTION_EXIT;
}


