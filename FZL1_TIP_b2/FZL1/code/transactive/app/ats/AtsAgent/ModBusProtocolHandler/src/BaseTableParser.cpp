/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\BaseTableParser.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * The base class for all Table parser that must be extended for specific table parsing purposes.
  * E.g PATableParser, TrainTableParser, PIDSTableParser
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/BaseTableParser.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"

namespace TA_IRS_App
{
	BaseTableParser::BaseTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: m_AtsEntityData(EntityData),
		  m_atsNotification(NULL)
	{
		FUNCTION_ENTRY("BaseTableParser");
		FUNCTION_EXIT;
	}

	BaseTableParser::~BaseTableParser()
	{
		FUNCTION_ENTRY("~BaseTableParser");
		FUNCTION_EXIT;
	}

	void BaseTableParser::setAtsDataNotification(TA_IRS_Bus::IAtsDataNotification* atsDataNofic)
	{
		FUNCTION_ENTRY("setAtsDataNotification");

		m_atsNotification = atsDataNofic;

		FUNCTION_EXIT;
	}
}