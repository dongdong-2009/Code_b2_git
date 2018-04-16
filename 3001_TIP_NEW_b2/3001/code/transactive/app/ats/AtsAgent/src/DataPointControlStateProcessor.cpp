/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/DataPointControlStateProcessor.cpp $
  * @author:   zhou yuan
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * this time is used for AtsTrainData to update the state of m_oa1Valid and m_atcTrainStatusValid 
  * of AtsTrainData
  *
  */

#include "core/synchronisation/src/ThreadGuard.h"

#include "app/ats/AtsAgent/src/DataPointControlStateProcessor.h"

namespace TA_IRS_App
{

	DataPointControlStateProcessor::DataPointControlStateProcessor()
	{
	}

	DataPointControlStateProcessor::~DataPointControlStateProcessor()
	{
	}

	void DataPointControlStateProcessor::InitialisePsdData(std::vector< AtsPsdData* > psdData)
	{
		m_psdData = psdData;
	}

	void DataPointControlStateProcessor::processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest )
	{
		FUNCTION_ENTRY("DataPointControlStateProcessor::run");

		bool processed = false;
		std::vector< AtsPsdData* >::iterator psdIt;
		for (psdIt = m_psdData.begin(); psdIt != m_psdData.end() && processed == false; psdIt++)
		{
			if ((*psdIt) != NULL)
			{
				TA_Base_Core::ThreadGuard guard( *(*psdIt) );
				
				try
				{
					processed = (*psdIt)->checkWriteQueue(newDataPointWriteRequest);
				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
						"run() - Exception caught while checkWriteQueue");
				}
			}
		}

		FUNCTION_EXIT;
	}

}