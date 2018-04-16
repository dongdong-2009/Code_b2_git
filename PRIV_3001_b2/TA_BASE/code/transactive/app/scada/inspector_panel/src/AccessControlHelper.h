/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/inspector_panel/src/AccessControlHelper.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
    
	class AccessControlHelper  
	{
	public:		

		enum AccessControlActions	{	
					DATA_POINT_WRITE = 0,
					DATA_POINT_ALARM_INHIBIT = 1,
					DATA_POINT_ALARM_UNINHIBIT = 2,
					DATA_POINT_CONTROL_INHIBIT = 3,
					DATA_POINT_CONTROL_UNINHIBIT = 4,
					DATA_POINT_SCAN_UNINHIBIT = 5,
					DATA_POINT_SCAN_INHIBIT = 6,
					DATA_POINT_MMS_INHIBIT = 7,
					DATA_POINT_MMS_UNINHIBIT = 8,
					DATA_POINT_SET_FORCED_STATE = 9,
					DATA_POINT_RESET_FORCED_STATE = 10,
					DATA_POINT_METER_ADJUST = 11,
					DATA_NODE_TAG = 12,
					DATA_NODE_NOTES = 13,
                    DATA_NODE_TAG_PTW = 14,
					IP_LIMITS_TAB_ACCESS = 15,
					ACCESS_CONTROL_ACTION_COUNT = 16 };

		static AccessControlHelper* getInstance();
		bool IsActionPermitted(unsigned long aEntityKey, AccessControlActions aAccessControlType);

		void InitAccessControlMap(TA_Base_Bus::RightsLibrary* rightsMgr,
								std::string& aSessionId, 													
								std::vector<AccessEntityInfo> entityInfo);

		void updateAccessControlMap(TA_Base_Bus::RightsLibrary* rightsMgr);
				
		virtual ~AccessControlHelper();

	private:
		void InitActionsMap();
		AccessControlHelper(const AccessControlHelper& obj) {};			
		AccessControlHelper&operator=(const AccessControlHelper& rhs) {};		
		AccessControlHelper();

		static AccessControlHelper*	m_singleton;
				
		// a map of entity keys with a vector of bool access flags for each of the access
		// controlled actions
		std::map<unsigned long, std::vector<bool> > m_entityAccessControlMap;
		std::map<int, int> 							m_ActionsMap;

		std::string									m_currentSessionId;
		std::vector<AccessEntityInfo>				m_currentEntityInfo;
		TA_Base_Core::ReEntrantThreadLockable 		m_lock;
	};

} // namespace

#endif // !defined(AFX_ACCESSCONTROLHELPER_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_)
