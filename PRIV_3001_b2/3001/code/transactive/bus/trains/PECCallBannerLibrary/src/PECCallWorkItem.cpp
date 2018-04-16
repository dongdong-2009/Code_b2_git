/**
  * PECCallWorkItem.cpp
  */

#include "PECCallWorkItem.h"

namespace TA_IRS_Bus
{

	PECCallWorkItem::PECCallWorkItem( PECCallEntry* CallEntry, PECCallActionType::EAction action)
		: m_pCallEntry(CallEntry),
		m_action(action)
	{
		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"Call Work Item : Constructor()");
	}

	void PECCallWorkItem::executeWorkItem()
	{
		FUNCTION_ENTRY("executeWorkItem");
	
		switch ( m_action )
		{
		case PECCallActionType::CCTV:
			{
				LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Call Work Item : PEC CCTV ()");
				m_pCallEntry->dialogCctvButtonPressed();
			}
			break;

		case PECCallActionType::RESET:
			{
				LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Call Work Item : PEC Reset ()");

				m_pCallEntry->dialogResetButtonPressed();
			}
			break;

		case PECCallActionType::RESETALL:
			{
				LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Call Work Item : PEC Reset all ()");

				m_pCallEntry->dialogResetAllButtonPressed();
			}
			break;

		case PECCallActionType::ONHOLD:
			{
				LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Call Work Item : PEC On Hold ()");
				m_pCallEntry->dialogHoldButtonPressed();
			}
			break;

		default:
			// DO NOTHING
			break;
		}

		FUNCTION_EXIT;
	};
}