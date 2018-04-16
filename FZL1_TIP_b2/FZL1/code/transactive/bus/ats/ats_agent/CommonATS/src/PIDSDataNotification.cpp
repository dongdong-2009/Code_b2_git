/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PIDSDataNotification.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling PIDS Information which will be used for data processing.
  */

#include "bus/ats/ats_agent/CommonATS/src/PIDSDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	PIDSDataNotification::PIDSDataNotification(const RawTable& rawTable)
		: DataNotification(NULL),
		  m_rawTable(rawTable)
	{
		FUNCTION_ENTRY("PIDSDataNotification");
		FUNCTION_EXIT;
	}

	PIDSDataNotification::~PIDSDataNotification()
	{
		FUNCTION_ENTRY("~PIDSDataNotification");
		FUNCTION_EXIT;
	}

	RawTable& PIDSDataNotification::getRawTable()
	{
		FUNCTION_ENTRY("getRawTable");
		FUNCTION_EXIT;
		return m_rawTable;
	}

	void PIDSDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PIDS Data Cache
		TA_IRS_Bus::DataCache::getInstance()->setPIDSData(getRawTable().getDataBlock());

		FUNCTION_EXIT;
	}

	void PIDSDataNotification::processAtsData()
	{
		FUNCTION_ENTRY("processAtsData");

		//Process Data Dispatch
		m_dataUpdater->processDataDispatcher(ePIDS);

		FUNCTION_EXIT;
	}
}