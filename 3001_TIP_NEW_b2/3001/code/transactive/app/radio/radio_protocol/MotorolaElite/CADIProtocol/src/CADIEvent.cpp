
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventHelper.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/ostreamHelper.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App {

	
	/************************************************************************/
	/* CADI Event	                                                        */
	/************************************************************************/

	CADIEvent::CADIEvent()
	{
		FUNCTION_ENTRY("CADIEvent");
		FUNCTION_EXIT;
		
	}

	CADIEvent::~CADIEvent()
	{
		FUNCTION_ENTRY("~CADIEvent");
		FUNCTION_EXIT;
	}

	int CADIEvent::call()
	{
		FUNCTION_ENTRY("call");
		FUNCTION_EXIT;
		return 0;
	}


	/************************************************************************/
	/* Affiliation                                                          */
	/************************************************************************/

	AffiliationEvent::~AffiliationEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int AffiliationEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_AffiliationEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}

	
	/************************************************************************/
	/* Command Status                                                       */
	/************************************************************************/
	
	CmdStatusEvent::~CmdStatusEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int CmdStatusEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_CommandStatusEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}


	/************************************************************************/
	/* Deaffiliation														*/
	/************************************************************************/

	DeaffiliationEvent::~DeaffiliationEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int DeaffiliationEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_DeaffiliationEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}


	/************************************************************************/
	/* Emergency Alarm														*/
	/************************************************************************/

	EmergencyAlarmEvent::~EmergencyAlarmEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int EmergencyAlarmEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_EmergencyAlarmEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}

	/************************************************************************/
	/* Group Call															*/
	/************************************************************************/


	GroupCallEvent::~GroupCallEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int GroupCallEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_GroupCallEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}

	/************************************************************************/
	/* Private Call															*/
	/************************************************************************/

	PrivateCallEvent::~PrivateCallEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int PrivateCallEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_PrivateCallEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}


	/************************************************************************/
	/* Radio Ack															*/
	/************************************************************************/

	RadioAckEvent::~RadioAckEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int RadioAckEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_RadioAckEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}


	/************************************************************************/
	/* Session Event														*/
	/************************************************************************/

	SessionEvent::~SessionEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int SessionEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_SessionEvent: data " << m_event << ")[" << CADIEventHelper::getSessionStatus(m_event) << "]";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());
		
		FUNCTION_EXIT;
		return 0;
	}


	/************************************************************************/
	/* Status Event															*/
	/************************************************************************/

	StatusEvent::~StatusEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int StatusEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_StatusEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());

		FUNCTION_EXIT;
		return 0;
	}

	/************************************************************************/
	/* Zone Controller Status												*/
	/************************************************************************/

	ZCStatusEvent::~ZCStatusEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int ZCStatusEvent::call()
	{
		FUNCTION_ENTRY("call");

		std::ostringstream logmsg;
		logmsg << "CADI_ZCStatusEvent: data " << m_event;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logmsg.str().c_str());
		
		FUNCTION_EXIT;
		return 0;   
	}

} 
