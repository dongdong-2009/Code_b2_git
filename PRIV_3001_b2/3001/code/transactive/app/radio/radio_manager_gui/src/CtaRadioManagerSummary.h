/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/CtaRadioManagerSummary.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_CTARADIOMANAGERSUMMARY_H__061AB25A_47CF_49DC_8D7E_93EABC87352F__INCLUDED_)
#define AFX_CTARADIOMANAGERSUMMARY_H__061AB25A_47CF_49DC_8D7E_93EABC87352F__INCLUDED_


#include "app/radio/radio_manager_gui/src/ItaRadioManagerSummary.h"
#include "app/radio/radio_manager_gui/src/RadioMFTAgentCallbackSubscriber.h"

#include "bus/security/rights_library/src/CtaRights.h"
#include "core/message/src/CtaAuditing.h"
#include "core/process_management/src/CtaRuntime.h"
#include "core/alarm/src/CtaAlarms.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4250)
#pragma warning(disable:4259)
#endif


class CtaRadioManagerSummary : public virtual TA_Base_Bus::CtaRights,
                               public virtual TA_Base_Core::CtaRuntime,
							   public virtual TA_Base_Core::CtaAlarms,
							   public virtual TA_Base_Core::CtaAuditing,
							   public virtual ItaRadioManagerSummary						 
{
	public:

		CtaRadioManagerSummary(TA_Base_Core::IEntityDataPtr agentConfiguration);
		
		virtual ~CtaRadioManagerSummary();


		
		void Subscribe();

	// Attributes
	private:
		
		RadioMFTAgentCallbackSubscriber* m_RadioMFTAgentCallbackSubscriber;
	
};


#endif
