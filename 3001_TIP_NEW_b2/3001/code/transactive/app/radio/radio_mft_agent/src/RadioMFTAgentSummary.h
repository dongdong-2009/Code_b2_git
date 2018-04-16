/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentSummary.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Facade to transActive framework functionality.
 */

#ifndef RADIO_MFT_AGENT_SUMMARY_H_INCLUDED
#define RADIO_MFT_AGENT_SUMMARY_H_INCLUDED

#include "bus/security/rights_library/src/CtaRights.h"
#include "core/alarm/src/CtaAlarms.h"
#include "core/message/src/CtaAuditing.h"
#include "core/process_management/src/CtaRuntime.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"


class RadioMFTAgentSummary:	public TA_Base_Bus::CtaRights,
                            public TA_Base_Core::CtaRuntime,
                            public TA_Base_Core::CtaAuditing,
							public TA_Base_Core::CtaAlarms
{

public:
    RadioMFTAgentSummary(TA_Base_Core::IEntityDataPtr agentConfiguration);
	virtual ~RadioMFTAgentSummary();

private:
    //Disabled Methods

	RadioMFTAgentSummary();
    RadioMFTAgentSummary(const RadioMFTAgentSummary&);
    RadioMFTAgentSummary& operator=(const RadioMFTAgentSummary&);
    
};
#endif
