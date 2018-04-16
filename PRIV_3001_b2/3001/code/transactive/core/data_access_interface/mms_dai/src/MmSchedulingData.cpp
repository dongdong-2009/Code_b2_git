/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/mms_dai/src/MmSchedulingData.cpp $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * MmSchedulingData is an implementation of IMmSchedulingData. It holds the data specific to 
 * alarm plan associations.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/mms_dai/src/MmSchedulingData.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingHelper.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <ctime>

namespace TA_IRS_Core
{


	MmSchedulingData::MmSchedulingData( unsigned long key, 
				unsigned long subsystemKey,
				time_t scheduledTime,
                time_t lastRunDate,
				const std::string& subName) 
				: m_helper(0)
	{
		FUNCTION_ENTRY("MmSchedulingData");

		m_helper = new MmSchedulingHelper( key, 
				subsystemKey,
				scheduledTime,
                lastRunDate,
				subName);

		FUNCTION_EXIT;
	}


	MmSchedulingData::~MmSchedulingData()
	{
		FUNCTION_ENTRY("~MmSchedulingData");

		if(0 != m_helper)
		{
			delete m_helper;
		}

		FUNCTION_EXIT;
	}
	

	unsigned long MmSchedulingData::getKey()
	{
		FUNCTION_ENTRY("getKey");

		return m_helper->getKey();

		FUNCTION_EXIT;
	}


	std::string MmSchedulingData::getName()
	{
		FUNCTION_ENTRY("getName");

		return m_helper->getName();

		FUNCTION_EXIT;
	}


	std::string MmSchedulingData::getSubsystemName()
	{
		FUNCTION_ENTRY("getSubsystemName");

		return m_helper->getSubsystemName();

		FUNCTION_EXIT;
	}


	unsigned long MmSchedulingData::getSubsystemKey()
	{
		FUNCTION_ENTRY("getSubsystemKey");

		return m_helper->getSubsystemKey();

		FUNCTION_EXIT;
	}


	time_t MmSchedulingData::getLastReported()
	{
		FUNCTION_ENTRY("getLastReported");

		return m_helper->getLastReported();

		FUNCTION_EXIT;
	}


	time_t MmSchedulingData::getScheduledTime()
	{
		FUNCTION_ENTRY("getScheduledTime");

		return m_helper->getScheduledTime();

		FUNCTION_EXIT;
	}


	void MmSchedulingData::invalidate()
	{
		FUNCTION_ENTRY("invalidate");

		m_helper->invalidate();

		FUNCTION_EXIT;
	}


} // closes TA_IRS_Core


