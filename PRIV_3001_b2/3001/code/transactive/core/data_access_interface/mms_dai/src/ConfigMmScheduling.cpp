/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/mms_dai/src/ConfigMmScheduling.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * ConfigMmScheduling is an implementation of IConfigMmScheduling. It holds the data specific to 
 * alarm plan associations and allows reads and writes.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/mms_dai/src/ConfigMmScheduling.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingHelper.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

	ConfigMmScheduling::ConfigMmScheduling( unsigned long key, 
				unsigned long subsystemKey,
				time_t scheduledTime,
                time_t lastRunDate,
				const std::string& subName) 
				: m_helper(0)
	{
		FUNCTION_ENTRY("ConfigMmScheduling");

		m_helper = new MmSchedulingHelper( key, 
				subsystemKey,
				scheduledTime,
                lastRunDate,
				subName);

		FUNCTION_EXIT;
	}


	ConfigMmScheduling::~ConfigMmScheduling()
	{
		FUNCTION_ENTRY("~ConfigMmScheduling");

		if(0 != m_helper)
		{
			delete m_helper;
		}

		FUNCTION_EXIT;
	}


	unsigned long ConfigMmScheduling::getKey()
	{
		FUNCTION_ENTRY("getKey");

		return m_helper->getKey();

		FUNCTION_EXIT;
	}


	std::string ConfigMmScheduling::getName()
	{
		FUNCTION_ENTRY("getName");

		return m_helper->getName();

		FUNCTION_EXIT;
	}


	std::string ConfigMmScheduling::getSubsystemName()
	{
		FUNCTION_ENTRY("getSubsystemName");

		return m_helper->getSubsystemName();

		FUNCTION_EXIT;
	}


	unsigned long ConfigMmScheduling::getSubsystemKey()
	{
		FUNCTION_ENTRY("getSubsystemKey");

		return m_helper->getSubsystemKey();

		FUNCTION_EXIT;
	}


	time_t ConfigMmScheduling::getLastReported()
	{
		FUNCTION_ENTRY("getLastReported");

		return m_helper->getLastReported();

		FUNCTION_EXIT;
	}


	time_t ConfigMmScheduling::getScheduledTime()
	{
		FUNCTION_ENTRY("getScheduledTime");

		return m_helper->getScheduledTime();

		FUNCTION_EXIT;
	}


	void ConfigMmScheduling::invalidate()
	{
		FUNCTION_ENTRY("invalidate");

		m_helper->invalidate();

		FUNCTION_EXIT;
	}



	bool ConfigMmScheduling::hasChanged()
	{
		FUNCTION_ENTRY("hasChanged");
		FUNCTION_EXIT;
		return m_helper->hasChanged();
	}


	void ConfigMmScheduling::applyChanges()
	{
		FUNCTION_ENTRY("applyChanges");

		m_helper->applyChanges();

		FUNCTION_EXIT;
	}



	void ConfigMmScheduling::setScheduledTime(time_t newTime)
	{
		FUNCTION_ENTRY("setScheduledTime");

		m_helper->setScheduledTime(newTime);

		FUNCTION_EXIT;
	}


    void ConfigMmScheduling::setLastReportedTime(time_t newTime)
	{
		FUNCTION_ENTRY("setLastReportedTime");

		m_helper->setLastReportedTime(newTime);

		FUNCTION_EXIT;
	}



	void ConfigMmScheduling::updateChanges(const std::string& name, unsigned long oldKey, 
		unsigned long newKey)
	{
		FUNCTION_ENTRY("updateChanges");

		//todo

		FUNCTION_EXIT;
	}



} // closes TA_IRS_Core


