/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasCachedConfig.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#if !defined(APP_BAS_BASAgent_BasCachedConfig_H)
#define APP_BAS_BASAgent_BasCachedConfig_H

#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/data_access_interface/bas/src/BasParameterValueType.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"

namespace TA_IRS_App
{
	class BasCachedConfig
	{
	public:
		virtual ~BasCachedConfig();
		static BasCachedConfig& getInstance();
		virtual void initCache(const TA_Base_Core::StationBASAgentEntityDataPtr& entityData, 
							   const std::map<unsigned long, TA_Base_Bus::DataPoint*>& datapoints);
		const std::string& getStationName() const;
		const std::string& getModeControlToken() const;
		
		TA_Base_Bus::DataPoint* getModeCtrlDPFromNode(const std::string& pnName);
		TA_Base_Bus::DataPoint* getModeAutoManualDPFromNode(const std::string& pnName);
		TA_Base_Bus::DataPoint* getModeInProgressDPFromNode(const std::string& pnName);
		TA_Base_Bus::DataPoint* getModeValueDPFromNode(const std::string& pnName);
        TA_Base_Bus::DataPoint* getModeRightControlDPFromNode(const std::string& pnName );
		TA_Base_Bus::DataPoint* getCurrentDayTTPt();
		TA_Base_Bus::DataPoint* getNextDayTTPt();
		
		unsigned long 	getLocationKey();
		unsigned long 	getEntityKey();
        std::string 	getLocDisplayName();
		unsigned long 	getStationFireMonitorTime(); //by seconds
        std::string 	getTTScheduleTime();

		bool		  	isStationFireMonitorValid();

        TA_Base_Bus::DataPoint& 	getDatapointFromKey(const unsigned long dpKey );
        void 						getParameterValueType(TA_IRS_Core::ParameterValueTypeInfoVct& valueTypes);
        TA_IRS_Core::Mode_SmartPtr 	getBasModeFromValue(const unsigned long modeValue);
        std::string 				getTimeTableTypeNameByKey(const unsigned long pkey);
        TA_Base_Bus::DataPoint* 	getFireModeRecoverDPFromNode(const std::string& pnName );

        std::string 				getTimetableDisplayNameByKey( const unsigned long pkey );

	private:
		BasCachedConfig();
		BasCachedConfig(const BasCachedConfig& theConfig);
		BasCachedConfig& operator=(const BasCachedConfig& theConfig);

		TA_Base_Bus::DataPoint* 	getDatapointFromName(const std::string& ptName);
		
		static BasCachedConfig* m_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		
		std::map<unsigned long,TA_Base_Bus::DataPoint*> m_dataPoints;
		
        std::string 	m_stationName;
		std::string 	m_modeControlToken;
		std::string 	m_modeAutoManualToken;
		std::string 	m_modeInProgressToken;
		std::string 	m_modeValueToken;
        std::string 	m_modeRightControlSqlToken;
        std::string 	m_fireModeRecoverDpTag;
        std::string 	m_locationDisplayName;
        std::string 	m_TTScheduleTime;
		
		unsigned long 	m_stationFireMonitorTime;
		unsigned long 	m_entityKey;
        unsigned long 	m_ulLocationKey;

		TA_Base_Bus::DataPoint* m_currentDayTTPt;
		TA_Base_Bus::DataPoint* m_nextDayTTPt;
		
		// The parameter value types
        TA_IRS_Core::ParameterValueTypeInfoVct              m_parameterValueTypeVct;
		
        std::map<unsigned long, TA_IRS_Core::Mode_SmartPtr> m_basModeMap;
        TA_IRS_Core::LocationAndTimeTableTypeMap            m_timeTableTypeMap;
	};
}
#endif //APP_BAS_BASAgent_BasCachedConfig_H
