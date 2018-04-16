/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataNotification.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is a base class for all data notification.
  * Specific Data object must extend and use this class.
  * E.g. PADataNotification, TrainDataNotification & PIDSDataNotification 
  */

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolParser.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	DataNotification::DataNotification(void* data)
		: m_Data(data),
		  m_dataUpdater(NULL),
		  m_dataQuality(TA_Base_Core::ITA_QUALITY_BAD)
	{
		FUNCTION_ENTRY("DataNotification");

		FUNCTION_EXIT;
	}

	DataNotification::~DataNotification()
	{
		FUNCTION_ENTRY("DataNotification");
		FUNCTION_EXIT;
	}

	void* DataNotification::getData()
	{
		FUNCTION_ENTRY("getData");
		FUNCTION_EXIT;
		return m_Data;
	}

	void DataNotification::setDataUpdater(IDataUpdate* dataUpater)
	{
		FUNCTION_ENTRY("setDataUpdater");

		m_dataUpdater = dataUpater;

		FUNCTION_EXIT
	}

	void DataNotification::setDataQuality(const TA_Base_Core::EDataPointQuality& quality)
	{
		FUNCTION_ENTRY("setDataQuality");

		m_dataQuality = quality;

		FUNCTION_EXIT;
	}

	TA_Base_Core::EDataPointQuality DataNotification::getDataQuality()
	{
		FUNCTION_ENTRY("getDataQuality");
		FUNCTION_EXIT;
		return m_dataQuality;
	}

}