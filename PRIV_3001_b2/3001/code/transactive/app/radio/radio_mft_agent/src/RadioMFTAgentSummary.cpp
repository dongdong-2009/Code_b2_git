/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentSummary.cpp $
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

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/radio_mft_agent/src/RadioMFTAgentSummary.h"
#include "core/message/types/MessageTypes.h"


RadioMFTAgentSummary::RadioMFTAgentSummary( TA_Base_Core::IEntityDataPtr agentConfiguration)
:	TA_Base_Core::CtaAlarms( agentConfiguration ),
    TA_Base_Core::CtaRuntime(agentConfiguration),
    TA_Base_Bus::CtaRights(),
	TA_Base_Core::CtaAuditing(getMyEntityKey(), TA_Base_Core::MessageContext("channel","context",TA_Base_Core::AuditMessage) )
{
}

RadioMFTAgentSummary::~RadioMFTAgentSummary()
{
}
