/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/inspector_panel/src/AccessControlHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/07/11 18:24:37 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_Base_App
{
	AccessControlHelper* AccessControlHelper::m_singleton = NULL;

	AccessControlHelper::~AccessControlHelper()
	{
		m_entityAccessControlMap.clear();
		m_ActionsMap.clear();
	}

	AccessControlHelper::AccessControlHelper()
	{ 
		// should only be done once
		InitActionsMap(); 
	}

	AccessControlHelper* AccessControlHelper::getInstance()
	{
		if (m_singleton == NULL)
		{
			m_singleton = new AccessControlHelper;
		}

		return m_singleton;
	};

	// Initialises the access control map 
	void AccessControlHelper::InitAccessControlMap(RightsLibrary* rightsMgr,
													std::string& aSessionId, 													
													std::vector<AccessEntityInfo> entityInfo)
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason="";
		std::map<unsigned long, std::vector<bool> >  tmpEntityAccessControlMap;
		std::vector<bool> actionPermissions;
		std::vector<AccessEntityInfo>::iterator iter = entityInfo.end();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,"Begin InitAccessControlMap");

		//call rightslib may cause corba call which may lead to long time block.,so do not use lock in the loop.
		for (iter = entityInfo.begin(); iter != entityInfo.end();iter ++)
		{			
			// find out if each of actions is allowed on this entity
			std::map<int, int>::iterator actionIterator = m_ActionsMap.end();
			actionPermissions.clear();

			for (unsigned short actionIndex = 0; actionIndex < ACCESS_CONTROL_ACTION_COUNT; actionIndex++)
			{
				bool actionAllowed = false;
				int actionId = 0;
	
				actionIterator = m_ActionsMap.find(actionIndex);
				if (actionIterator != m_ActionsMap.end())
				{
					actionId = actionIterator->second;
				}
				
				try
				{
					actionAllowed = rightsMgr->isActionPermittedOnResourceAndLocSub(
										aSessionId, 
										(*iter).entityKey, 
                                        (*iter).locationKey,
                                        (*iter).subSystemKey,
										actionId, 
										reason, 
										decisionModule);										
				}
				catch (...)
				{
					actionAllowed = false;
				}   
				
				actionPermissions.push_back(actionAllowed);
			};

			tmpEntityAccessControlMap.insert(
				std::map<unsigned long, std::vector<bool> >::value_type((*iter).entityKey, actionPermissions));
		}

		{
			ThreadGuard guard( m_lock );
			m_entityAccessControlMap.swap(tmpEntityAccessControlMap);
			m_currentSessionId = aSessionId;
			m_currentEntityInfo.swap(entityInfo);
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,"End InitAccessControlMap");
	}

	void AccessControlHelper::updateAccessControlMap(RightsLibrary* rightsMgr)
	{
		
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason = "";
		std::map<unsigned long, std::vector<bool> >  tmpEntityAccessControlMap;
		std::vector<bool> actionPermissions;
		std::vector<AccessEntityInfo>	tmpEntityInfo;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,"Begin updateAccessControlMap");
		
		{
			ThreadGuard guard( m_lock );
			tmpEntityInfo = m_currentEntityInfo;
		}

		for (std::vector<AccessEntityInfo>::iterator iter = tmpEntityInfo.begin();iter != tmpEntityInfo.end();iter++)
		{			
			// find out if each of actions is allowed on this entity
			std::map<int, int>::iterator actionIterator = m_ActionsMap.end();
			actionPermissions.clear();

			for (unsigned short actionIndex = 0; actionIndex < ACCESS_CONTROL_ACTION_COUNT; actionIndex++)
			{
				bool actionAllowed = false;
				int actionId = 0;
	
				actionIterator = m_ActionsMap.find(actionIndex);
				if (actionIterator != m_ActionsMap.end())
				{
					actionId = actionIterator->second;
				}
				
				try
				{
					actionAllowed = rightsMgr->isActionPermittedOnResourceAndLocSub(
										m_currentSessionId, 
										(*iter).entityKey, 
                                        (*iter).locationKey,
                                        (*iter).subSystemKey,
										actionId, 
										reason, 
										decisionModule);										
				}
				catch (...)
				{
					actionAllowed = false;
				}   
				
				actionPermissions.push_back(actionAllowed);
			};

			tmpEntityAccessControlMap.insert(
				std::map<unsigned long, std::vector<bool> >::value_type((*iter).entityKey, actionPermissions));
		}

		{
			ThreadGuard guard( m_lock );
			m_entityAccessControlMap.swap(tmpEntityAccessControlMap);			
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,"End updateAccessControlMap");
	}
	// Returns a bool specifying if action should be permitted or not
	//
	bool AccessControlHelper::IsActionPermitted(unsigned long aEntityKey, AccessControlActions aAccessControlType)
	{
		std::map<unsigned long, std::vector<bool> >::iterator iter;

		ThreadGuard guard( m_lock );
		iter = m_entityAccessControlMap.find(aEntityKey);
		if (iter != m_entityAccessControlMap.end())
		{			
			return iter->second[aAccessControlType];

		} else
			return false;
	}


	// Function maps an internal action to the actual action id stored in the database
	//
	void AccessControlHelper::InitActionsMap()
	{
		m_ActionsMap.clear();

		// DATA_POINT_WRITE = 0,
		m_ActionsMap.insert(std::map<int, int>::value_type(0, TA_Base_Bus::aca_DATA_POINT_WRITE));

		// DATA_POINT_ALARM_INHIBIT = 1,
		m_ActionsMap.insert(std::map<int, int>::value_type(1, TA_Base_Bus::aca_DATA_POINT_ALARM_INHIBIT));
					
		// DATA_POINT_ALARM_UNINHIBIT = 2,
		m_ActionsMap.insert(std::map<int, int>::value_type(2, TA_Base_Bus::aca_DATA_POINT_ALARM_UNINHIBIT));
		
		// DATA_POINT_CONTROL_INHIBIT = 3,
		m_ActionsMap.insert(std::map<int, int>::value_type(3, TA_Base_Bus::aca_DATA_POINT_CONTROL_INHIBIT));
		
		// DATA_POINT_CONTROL_UNINHIBIT = 4,
		m_ActionsMap.insert(std::map<int, int>::value_type(4, TA_Base_Bus::aca_DATA_POINT_CONTROL_UNINHIBIT));
		
		// DATA_POINT_SCAN_UNINHIBIT = 5,
		m_ActionsMap.insert(std::map<int, int>::value_type(5, TA_Base_Bus::aca_DATA_POINT_SCAN_UNINHIBIT));
			
		// DATA_POINT_SCAN_INHIBIT = 6,
		m_ActionsMap.insert(std::map<int, int>::value_type(6, TA_Base_Bus::aca_DATA_POINT_SCAN_INHIBIT));
			
		// DATA_POINT_MMS_INHIBIT = 7,
		m_ActionsMap.insert(std::map<int, int>::value_type(7, TA_Base_Bus::aca_DATA_POINT_MMS_INHIBIT));

		// DATA_POINT_MMS_UNINHIBIT = 8,
		m_ActionsMap.insert(std::map<int, int>::value_type(8, TA_Base_Bus::aca_DATA_POINT_MMS_UNINHIBIT));
					
		// DATA_POINT_SET_FORCED_STATE = 9,
		m_ActionsMap.insert(std::map<int, int>::value_type(9, TA_Base_Bus::aca_DATA_POINT_SET_FORCED_STATE));

		// DATA_POINT_RESET_FORCED_STATE = 10,
		m_ActionsMap.insert(std::map<int, int>::value_type(10, TA_Base_Bus::aca_DATA_POINT_RESET_FORCED_STATE));

		// DATA_POINT_METER_ADJUST == 11
		m_ActionsMap.insert(std::map<int, int>::value_type(11, TA_Base_Bus::aca_DATA_POINT_METER_ADJUST));

		// DATA_NODE_TAG = 12,
		m_ActionsMap.insert(std::map<int, int>::value_type(12, TA_Base_Bus::aca_DATA_NODE_TAG));
		
		// DATA_NODE_NOTES = 13;
		m_ActionsMap.insert(std::map<int, int>::value_type(13, TA_Base_Bus::aca_DATA_NODE_NOTES));

        // DATA_NODE_TAG_PTW = 14;
        m_ActionsMap.insert(std::map<int, int>::value_type(DATA_NODE_TAG_PTW, TA_Base_Bus::aca_DATA_NODE_TAG_PTW));

        // IP_LIMITS_TAB_ACCESS = 15;
        m_ActionsMap.insert(std::map<int, int>::value_type(IP_LIMITS_TAB_ACCESS, TA_Base_Bus::aca_IP_LIMITS_TAB_ACCESS));
	}
} // namespace