/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"
#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"


using namespace TA_Base_Core;
using namespace TA_Base_Bus;

static const long TRY_CONNECT_RIGHT_AGENT_TIME = 5000;

namespace TA_Base_App
{
	AccessControlHelper* AccessControlHelper::m_singleton = NULL;

	AccessControlHelper::~AccessControlHelper()
	{
		TA_THREADGUARD(m_ActionsMapLock);
		m_ActionsMap.clear();
		m_EntityInfoVector.clear();
		if(m_pRightsMgr) delete m_pRightsMgr;
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
													std::string aSessionId, 													
													std::vector<AccessEntityInfo> entityInfo)
	{
		m_pRightsMgr		= rightsMgr;
		m_SessionId			= aSessionId.c_str();		
		TA_THREADGUARD(m_EntityInfoVectorMapLock);
		m_EntityInfoVector	= entityInfo;
		if (m_isConnectingAgent == false)
		{			
			start();
		}
	}

	void AccessControlHelper::InitEntitiesRights()
	{
		//TA_THREADGUARD(m_entityAccessControlMapLock);
		//m_entityAccessControlMap.clear();
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;

		std::vector<bool> actionPermissions;

		TA_THREADGUARD(m_EntityInfoVectorMapLock);
		std::vector<AccessEntityInfo>::iterator iter = m_EntityInfoVector.begin();
		if ( iter == m_EntityInfoVector.end() ) return;

		std::list<unsigned long> t_actionList ,t_getActionList;
		TA_THREADGUARD(m_ActionsMapLock);
		std::map<unsigned long, bool>::iterator actionIterator = m_ActionsMap.begin();
		for (; actionIterator != m_ActionsMap.end(); actionIterator++)
		{
			t_actionList.push_back(actionIterator->first);
			actionIterator->second = false;	
		}

		try
		{
			 m_pRightsMgr->areActionsPermittedOnSubsystemAndLocSub(
				m_SessionId.c_str(), 
				(*iter).entityKey, 
				(*iter).locationKey,
				(*iter).subSystemKey,
				t_actionList, 
				t_getActionList,
				reason, 
				decisionModule);	
			 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "size of getActionList:%d", t_getActionList.size());
		}
		catch (const TA_Base_Core::RightsException& ex)
		{							
			//LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::RightsException", ex.what() );	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TA_Base_Core::RightsException:%s", ex.what());
			t_getActionList.clear();
			throw ( TA_Base_Core::RightsException(ex.what()) );
		} 
		catch (...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Can not get rights.");
			t_getActionList.clear();
		}	
				
		std::list<unsigned long>::iterator listIt = t_getActionList.begin();
		for (; listIt != t_getActionList.end(); listIt++)
		{
			std::map<unsigned long, bool>::iterator actionMapIt = m_ActionsMap.find(*listIt);
			if (actionMapIt != m_ActionsMap.end())
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "actionkey:%d,rights:true",actionMapIt->first);
				actionMapIt->second = true;				
			}
		}

		CWinApp *app = AfxGetApp();
		if (app != NULL && app->m_pMainWnd != NULL)
		{
			app->m_pMainWnd->PostMessage(UPDATE_PERMISSION, 1, 0);
		}
	}

	// Returns a bool specifying if action should be permitted or not
	//
	bool AccessControlHelper::IsActionPermitted(unsigned long aEntityKey, AccessControlActions aAccessControlType)
	{
		TA_THREADGUARD(m_ActionsMapLock);
		bool returnValue = false;
		std::map<unsigned long, bool>::iterator actionMapIt = m_ActionsMap.find(aAccessControlType);
		if (actionMapIt != m_ActionsMap.end())
		{
			returnValue = actionMapIt->second ;
		}
		return returnValue;		
	}


	// Function maps an internal action to the actual action id stored in the database
	//
	void AccessControlHelper::InitActionsMap()
	{
		m_isConnectingAgent	= false;
		m_pRightsMgr		= NULL;
		m_SessionId			= "";
		m_EntityInfoVector.clear();
		TA_THREADGUARD(m_ActionsMapLock);
		m_ActionsMap.clear();

		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_WRITE,				false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_ALARM_INHIBIT,		false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_ALARM_UNINHIBIT,		false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_CONTROL_INHIBIT,		false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_CONTROL_UNINHIBIT,	false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_SCAN_UNINHIBIT,		false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_SCAN_INHIBIT,			false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_MMS_INHIBIT,			false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_MMS_UNINHIBIT,		false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_SET_FORCED_STATE,		false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_RESET_FORCED_STATE,	false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_POINT_METER_ADJUST,			false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_NODE_TAG,					false));
		m_ActionsMap.insert(std::map<int, int>::value_type(DATA_NODE_NOTES,					false));
        m_ActionsMap.insert(std::map<int, int>::value_type(DATA_NODE_TAG_PTW,				false));
        m_ActionsMap.insert(std::map<int, int>::value_type(IP_LIMITS_TAB_ACCESS,			false));
	}

	void AccessControlHelper::run()
	{
		m_isConnectingAgent 	= true;
		while(	m_isConnectingAgent)
		{
			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"try to get entities rights");
				InitEntitiesRights();
				m_isConnectingAgent = false;				
			}
			catch (const TA_Base_Core::RightsException& ex)
			{				
				sleep(TRY_CONNECT_RIGHT_AGENT_TIME);
			}
		}
		//terminateAndWait();
	}
	void AccessControlHelper::terminate()
	{
		m_isConnectingAgent = false;
	}
} // namespace
