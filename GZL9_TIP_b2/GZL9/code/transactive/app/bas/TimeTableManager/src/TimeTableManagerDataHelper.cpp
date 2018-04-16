#include "StdAfx.h"
#include "TimeTableManagerDataHelper.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerUtility.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/bas/src/BASTimeTableMappingAccessFactory.h"
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"

namespace TA_IRS_App
{
	TimeTableManagerDataHelper::TimeTableManagerDataHelper(void)
	{
	}

	TimeTableManagerDataHelper::~TimeTableManagerDataHelper(void)
	{
	}

	// The location actions
	ILocation_SmartPtr TimeTableManagerDataHelper::getLocalLocation()
	{
		FUNCTION_ENTRY("getLocalLocation");

        if ( NULL == m_ptrLocalStation.get() )
        {
            std::string strLocationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);

            // get the local location key
            unsigned long locationKey = TimeTableManagerUtility::stringToUnsignedLong(strLocationKey);
            BAS_TRY
            {
                m_ptrLocalStation.reset(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( locationKey ));
            }
            BAS_CATCH;
        }

		FUNCTION_EXIT;
		return m_ptrLocalStation;
	}

	std::vector<ILocation_SmartPtr> TimeTableManagerDataHelper::getAllLocations()
	{
		FUNCTION_ENTRY("getAllLocations");

		TA_THREADGUARD(m_LocationLock);
		if (m_locations.empty())
        {
            // there have all locations, we need have a filter
            std::set<unsigned long> setLocKeys = TA_IRS_Core::BASTimeTableMappingAccessFactoryInstance::instance()->getAllValidLocationKeys();

            std::vector<TA_Base_Core::ILocation*> vctLocations;

			BAS_TRY
			{
                vctLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

                for (std::vector<TA_Base_Core::ILocation*>::iterator it = vctLocations.begin(); it != vctLocations.end(); it++)
                {
                    ILocation_SmartPtr temp(*it);

                    if ( setLocKeys.end() != setLocKeys.find( temp->getKey() ))
                    {
                        m_locations.push_back(temp);
                    }
                }
			}
			BAS_CATCH;

			if (BAS_WhetherHaveError)
			{
				vctLocations.clear();
			}
		}
		FUNCTION_EXIT;
		return m_locations;
	}

	ILocation_SmartPtr TimeTableManagerDataHelper::getLocationByKey(unsigned long locationKey)
	{
		FUNCTION_ENTRY("getLocationByKey");

		if (m_locations.empty())
		{
			getAllLocations();
		}

		{
			TA_THREADGUARD(m_LocationLock);
			std::vector<ILocation_SmartPtr>::iterator it;
			for (it = m_locations.begin(); it != m_locations.end(); it++)
			{
				if (locationKey == (*it)->getKey())
				{
					FUNCTION_EXIT;
					return *it;
				}
			}
		}

		ILocation_SmartPtr ptrNullLoc( static_cast<TA_Base_Core::ILocation*>(NULL) );
		FUNCTION_EXIT;
		return ptrNullLoc;
	}

	void TimeTableManagerDataHelper::getTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, 
												   const std::string& strType,	TimeTableScheduleInfoVct& timeTableInfo)
	{
		FUNCTION_ENTRY("getTimeTableScheduleInfoByLocationAndType");
		timeTableInfo.clear();

		TimeTableScheduleInfoVct::iterator it;
		for (it = m_timeTableSchedules.begin(); it != m_timeTableSchedules.end(); it++)
		{
			if ((*it)->m_locationKey == locationKey && (*it)->m_typeName == strType)
			{
				timeTableInfo.push_back((*it));
			}
		}
		
		// Load the uncached items
		if (timeTableInfo.empty())
		{
			BASTimeTableMappingAccessFactoryInstance::instance()->getAllTimeTableScheduleInfoByLocationAndType(locationKey, strType, timeTableInfo);
			m_timeTableSchedules.insert(m_timeTableSchedules.begin(), timeTableInfo.begin(), timeTableInfo.end());
		} 
		FUNCTION_EXIT;
	}

	void TimeTableManagerDataHelper::deleteTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& strType)
	{
		FUNCTION_ENTRY("deleteTimeTableScheduleInfoByLocationAndType");
		std::vector<unsigned long> mappingPkey;
		// get the items should be deleted
		TimeTableScheduleInfoVct::iterator it = m_timeTableSchedules.begin();
		while(it != m_timeTableSchedules.end())
		{
			if ((*it)->m_locationKey == locationKey && (*it)->m_typeName == strType)
			{
				mappingPkey.push_back((*it)->m_pkey);
			}

			it++;
		}
		// delete from DB
		BASTimeTableMappingAccessFactoryInstance::instance()->deleteTimeTableMapping(mappingPkey);

		// delete from memory
		it = m_timeTableSchedules.begin();
		while (it != m_timeTableSchedules.end())
		{
			if ((*it)->m_locationKey == locationKey && (*it)->m_typeName == strType)
			{
				it = m_timeTableSchedules.erase(it);
			}
			else
			{
				++it;
			}
		}
		FUNCTION_EXIT;
	}

	void TimeTableManagerDataHelper::addTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& strType, const ScheduleInfoVct& scheduleInfo)
	{
		FUNCTION_ENTRY("addTimeTableScheduleInfoByLocationAndType");
		if (scheduleInfo.size() > 0)
		{
			unsigned long timetableKey = BASTimeTableTimeTableAccessFactoryInstance::instance()->getTimeTableKeyByLocationkeyAndType(locationKey, strType);

			ScheduleInfoVct::const_iterator it = scheduleInfo.begin();
			while(it != scheduleInfo.end())
			{
				ModeTime modeTime((*it).modeTime);
				unsigned long modeKey = (*it).modeKey;
				unsigned long modetimeKey = BASTimeTableModeTimeAccessFactoryInstance::instance()->addModeTime(modeKey, modeTime);
				BASTimeTableMappingAccessFactoryInstance::instance()->addTimeTableMapping(timetableKey,modetimeKey);
				it++;
			}		
		}
		

		FUNCTION_EXIT;
	}


	bool less(Mode_SmartPtr leftMode, Mode_SmartPtr rightMode)
	{
		unsigned long left_subsystemOrderID = TimeTableManagerDataHelperInstance::instance()->getSubsystemOrderIDByKey(leftMode);
		unsigned long right_subsystemOrderID = TimeTableManagerDataHelperInstance::instance()->getSubsystemOrderIDByKey(rightMode);

		return left_subsystemOrderID < right_subsystemOrderID;
	}

	void TimeTableManagerDataHelper::getModesByLocation(unsigned long locationKey, locationModes& modes)
	{
		FUNCTION_ENTRY("getModesByLocation");
		ModesInfoIter it = m_modes.find(locationKey);
		if (it == m_modes.end())
		{
			if(!TA_IRS_Core::ModeAccessFactory::getInstance().getModes(locationKey, modes))
			{
				modes.clear();
			}
			else
			{
				std::sort(modes.begin(), modes.end(), less); //the CEditableListCtrl have a default sort, unused it.
				m_modes.insert(std::make_pair(locationKey, modes));
			}
		}
		else
		{
			modes = m_modes[locationKey];
		}
		FUNCTION_EXIT;
	}

	unsigned long TimeTableManagerDataHelper::getModeKeyByName(unsigned long locationKey, const std::string& strModeName)
	{
		FUNCTION_ENTRY("getModeKeyByName");
		loadModesByLocation(locationKey);

		unsigned long modeKey = TA_Base_Core::DatabaseKey::getInvalidKey();
		locationModes modes = m_modes[locationKey];
		for (locationModesIter it = modes.begin(); it != modes.end(); it++)
		{
			if ((*it)->getName() == strModeName)
			{
				modeKey = (*it)->getKey();
				break;
			}
		}
		FUNCTION_EXIT;
		return modeKey;
	}

	Mode_SmartPtr TimeTableManagerDataHelper::getModeByModeValue(unsigned long locationKey, unsigned long modeValue)
	{
		FUNCTION_ENTRY("getModeByModeValue");
		loadModesByLocation(locationKey);

		Mode_SmartPtr modePtr;
		locationModes modes = m_modes[locationKey];
		for (locationModesIter it = modes.begin(); it != modes.end(); it++)
		{
			if ((*it)->getValue() == modeValue)
			{
				modePtr = (*it);
				break;
			}
		}
		FUNCTION_EXIT;
		return modePtr;
	}

	void TimeTableManagerDataHelper::getTimeTableNames(TimeTableNameMap& timeTableNames)
	{
		FUNCTION_ENTRY("getTimeTableNames");
		if (m_timeTableNameMaps.empty())
		{
			m_timeTableNameMaps[TTT_Workdays] = TimeTableName("WORKDAYS","工作日时间表");
			m_timeTableNameMaps[TTT_Friday] = TimeTableName("FRIDAY","星期五时间表");
			m_timeTableNameMaps[TTT_Saturday] = TimeTableName("SATURDAY","星期六时间表");
			m_timeTableNameMaps[TTT_Sunday] = TimeTableName("SUNDAY","星期日时间表");
			m_timeTableNameMaps[TTT_Holiday] = TimeTableName("HOLIDAY","节假日时间表");
			m_timeTableNameMaps[TTT_Special1] = TimeTableName("SPECIAL1","特殊事件一时间表");
			m_timeTableNameMaps[TTT_Special2] = TimeTableName("SPECIAL2","特殊事件二时间表");
			m_timeTableNameMaps[TTT_Special3] = TimeTableName("SPECIAL3","特殊事件三时间表");
			m_timeTableNameMaps[TTT_Special4] = TimeTableName("SPECIAL4","特殊事件四时间表");
			m_timeTableNameMaps[TTT_Special5] = TimeTableName("SPECIAL5","特殊事件五时间表");		
		}

		timeTableNames = m_timeTableNameMaps;
		FUNCTION_EXIT;
	}

	void TimeTableManagerDataHelper::loadModesByLocation(unsigned long locationKey)
	{
		FUNCTION_ENTRY("loadModesByLocation");
		locationModes modes;
		BAS_TRY
		{
			getModesByLocation(locationKey, modes);
		}
		BAS_CATCH;			

		FUNCTION_EXIT;
	}

	unsigned long TimeTableManagerDataHelper::getSubsystemOrderIDByKey(Mode_SmartPtr modePtr)
	{
		FUNCTION_ENTRY("getSubsystemOrderIDByKey");
		unsigned long subsystemOrderValue = 0;
		if (modePtr.get() != NULL)
		{
			if (m_allSubSystems.empty())
			{
				TA_IRS_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(m_allSubSystems);
			}

			TA_IRS_Core::AllLocationSubsystems::iterator it = m_allSubSystems.find(modePtr->getLocationKey());
			if (m_allSubSystems.end() == it)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not found the Bas subsystem info in location %d:%s",modePtr->getLocationKey(), modePtr->getLocationName());
			}
			else
			{
				TA_IRS_Core::LocationSubsystem::iterator subsystemIter;
				for (subsystemIter = it->second.begin(); subsystemIter != it->second.end(); subsystemIter++)
				{
					if ((*subsystemIter)->getKey() == modePtr->getSubsystemKey())
					{
						subsystemOrderValue = (*subsystemIter)->getOrderID();
						break;
					}
				}
			}
		}
		
		FUNCTION_EXIT;
		return subsystemOrderValue;
	}
}
