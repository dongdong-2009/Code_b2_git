/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/StationModeManager.h $
  * @author:	Liu Yong
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
  
#if !defined(APP_BAS_BASAgent_StationModeManager_H)
#define APP_BAS_BASAgent_StationModeManager_H

#include "app/bas/BASAgent/src/BasModeEquipment.h"
#include "app/bas/BASAgent/src/IStationModeExecutionProxy.h"
#include "app/bas/BASAgent/src/BasModeEquipmentFactory.h"
#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "core/data_access_interface/bas/src/BASTimeTableMappingAccessFactory.h"
#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{
	class IModbusHelper;
	class RightsLibrary;
}

namespace TA_IRS_App
{
	class FireTriggerBasEquipmentManager;
	typedef unsigned short WORD;

	class StationModeManager : public IStationModeExecutionProxy
	{
	public:
		StationModeManager();
		virtual ~StationModeManager();

		virtual void executeStationMode(CORBA::Long subsystemKey, CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId);
        virtual bool isAgentRunningControlMode();
        virtual void setControlMode(bool bControlMode);
		unsigned long getCurrentModeValue(unsigned long subsystemKey);

	private:
        StationModeManager(const StationModeManager&);
        StationModeManager& operator=(const StationModeManager&);
		void createModeEquipments();

		private:
        TA_IRS_App::AllBasModeEquipments                m_modeEquipments;
		TA_Base_Core::NonReEntrantThreadLockable		m_staionModeLock;
		FireTriggerBasEquipmentManager*					m_fireTriggerManager;
        bool                                            m_controlMode;
	};
}

#endif //APP_BAS_BASAgent_StationModeManager_H
