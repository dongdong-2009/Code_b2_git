/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/scada/inspector_panel/src/AccessControlHelper.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/06/14 21:09:24 $
  * Last modified by:  $Author: yong.liu $
  * 
  * This singleton creates a map of permissions for InspectorPanel.
  */

#pragma warning ( disable:4786 )

#if !defined(AFX_ACCESSCONTROLHELPER_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_)
#define AFX_ACCESSCONTROLHELPER_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/threads/src/Thread.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_Base_App
{
    struct AccessEntityInfo
    {
        unsigned long entityKey;
        unsigned long locationKey;
        unsigned long subSystemKey;
    };
    
	class AccessControlHelper  : public virtual TA_Base_Core::Thread
	{
	public:		

		enum AccessControlActions	{	
					DATA_POINT_WRITE					= TA_Base_Bus::aca_DATA_POINT_WRITE,
					DATA_POINT_ALARM_INHIBIT			= TA_Base_Bus::aca_DATA_POINT_ALARM_INHIBIT,
					DATA_POINT_ALARM_UNINHIBIT			= TA_Base_Bus::aca_DATA_POINT_ALARM_UNINHIBIT,
					DATA_POINT_CONTROL_INHIBIT			= TA_Base_Bus::aca_DATA_POINT_CONTROL_INHIBIT,
					DATA_POINT_CONTROL_UNINHIBIT		= TA_Base_Bus::aca_DATA_POINT_CONTROL_UNINHIBIT,
					DATA_POINT_SCAN_UNINHIBIT			= TA_Base_Bus::aca_DATA_POINT_SCAN_UNINHIBIT,
					DATA_POINT_SCAN_INHIBIT				= TA_Base_Bus::aca_DATA_POINT_SCAN_INHIBIT,
					DATA_POINT_MMS_INHIBIT				= TA_Base_Bus::aca_DATA_POINT_MMS_INHIBIT,
					DATA_POINT_MMS_UNINHIBIT			= TA_Base_Bus::aca_DATA_POINT_MMS_UNINHIBIT,
					DATA_POINT_SET_FORCED_STATE			= TA_Base_Bus::aca_DATA_POINT_SET_FORCED_STATE,
					DATA_POINT_RESET_FORCED_STATE		= TA_Base_Bus::aca_DATA_POINT_RESET_FORCED_STATE,
					DATA_POINT_METER_ADJUST				= TA_Base_Bus::aca_DATA_POINT_METER_ADJUST,
					DATA_NODE_TAG						= TA_Base_Bus::aca_DATA_NODE_TAG,
					DATA_NODE_NOTES						= TA_Base_Bus::aca_DATA_NODE_NOTES,
                    DATA_NODE_TAG_PTW					= TA_Base_Bus::aca_DATA_NODE_TAG_PTW,
					IP_LIMITS_TAB_ACCESS				= TA_Base_Bus::aca_IP_LIMITS_TAB_ACCESS,
					ACCESS_CONTROL_ACTION_COUNT = 16 };

		static AccessControlHelper* getInstance();
		bool IsActionPermitted(unsigned long aEntityKey, AccessControlActions aAccessControlType);

		void InitAccessControlMap(TA_Base_Bus::RightsLibrary* rightsMgr,
								std::string aSessionId, 													
								std::vector<AccessEntityInfo> entityInfo);

		virtual void run();
		virtual void terminate();
				
		virtual ~AccessControlHelper();

	protected:

		void InitEntitiesRights();

	private:
		void InitActionsMap();
		AccessControlHelper(const AccessControlHelper& obj) {};			
		AccessControlHelper&operator=(const AccessControlHelper& rhs) {};		
		AccessControlHelper();

		static AccessControlHelper*	m_singleton;
				
		// a map of entity keys with a vector of bool access flags for each of the access
		// controlled actions
		TA_Base_Core::ReEntrantThreadLockable		m_ActionsMapLock;
		std::map<unsigned long, bool>				m_ActionsMap;
		bool										m_isConnectingAgent;
		TA_Base_Bus::RightsLibrary*					m_pRightsMgr;
		std::string									m_SessionId;
		TA_Base_Core::ReEntrantThreadLockable		m_EntityInfoVectorMapLock;
		std::vector<AccessEntityInfo>				m_EntityInfoVector;	

	};

} // namespace

#endif // !defined(AFX_ACCESSCONTROLHELPER_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_)
