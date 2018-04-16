#if !defined(GF_3002_app_bas_BASAgent_FireTriggerBasEquipmentManager_H)
#define GF_3002_app_bas_BASAgent_FireTriggerBasEquipmentManager_H

#include <string>
#include <vector>
#include <map>
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/bas/src/BasFireAccessFactory.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "app/bas/BASAgent/src/FireTriggerBasEquipment.h"

namespace TA_IRS_App
{
	typedef struct  
	{
		unsigned long modeValue;
		time_t fireTime;
		bool whetherProcessed;
		TA_IRS_Core::BasStationFireStatus status;
	}FireInfo;
	typedef std::vector<FireInfo> LocationFires;
	typedef std::map<unsigned long/*locationKey*/,LocationFires> AllFireModes;

	typedef struct 
	{
		unsigned long locationKey;
		unsigned long modeValue;
	}StationFireNotifyACKEvent;

	typedef struct 
	{
		unsigned long locationKey;
		unsigned long applicationType;
		bool bSuccess;
	}LaunchApplicationACKEvent;

	class StationModeManager;
	class StationFireMonitorThread;
	class FireTriggerBasEquipmentManager : public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
	{
	public:
		friend class FireTriggerBasEquipment;
		friend class StationFireMonitorThread;
		FireTriggerBasEquipmentManager(StationModeManager* basAgent);
		virtual ~FireTriggerBasEquipmentManager(void);

	public:
		void initializeManager();
		void uninitializeManager();
		void getExpiredStationFires(AllFireModes& fireModeInfo);

	protected:
		void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);
		void receiveLaunchApplicationAckMessage(TA_Base_Core::IControlStationCorbaDef::LaunchApplicationACKEvent event);
		void receiveStationFireACKMessage(TA_IRS_Bus::IStationBasAgentCorbaDef::StationFireNotifyACKEvent event);
		void sendLaunchApplicaitonMessage(unsigned long locationKey, unsigned long appType, std::string runParameter);
		void sendStationFireMessage(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status);
		bool isReceiveLaunchAppAckMessage(unsigned long locationKey, unsigned long appType);
		bool isReceiveStationFireAckMessage(unsigned long locationKey, unsigned long modeValue);
		
		void processStationFireMode();
		bool launchApplication(unsigned long locationKey, unsigned long modeValue, unsigned long ApplicationType);
		bool sendStationFireNotifyCation(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status);
		void processStationFireMode(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status);
		void setStationFireProcessed(unsigned long locationKey, unsigned long modeValue);
		bool needProcessUnprocessedFireMode();
	protected:
		void createAllFireTriggerEquipment();
		inline time_t getCurrentTime();
		inline bool isTimeExpired(time_t fireTime);
		unsigned long getSubsystemKeyByModeValue(unsigned long modeValue);
		bool isSubsystemModeMatched(unsigned long subsystemKey, unsigned long modeValue);
		void processFireTriggerBas(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus fireOrNormal);		
	private:
		StationFireMonitorThread*					m_fireMonitorThread;
		StationModeManager*							m_basAgentPtr;
		AllFireModes								m_allfireModes;
		TA_Base_Core::NonReEntrantThreadLockable	m_fireModeLock;
		std::vector<FireTriggerBasEquipment*>		m_allFireTriggerEquipments;

		std::vector<LaunchApplicationACKEvent>		m_receivedLaunchAppEvent;
		std::vector<StationFireNotifyACKEvent>		m_receivedStationFireEvent;
		TA_Base_Core::NonReEntrantThreadLockable	m_launchAppLock;
		TA_Base_Core::NonReEntrantThreadLockable	m_StationFireLock;

		std::vector<TA_IRS_Core::Mode_SmartPtr>		m_allModes;
	};

}

#endif //GF_3002_app_bas_BASAgent_FireTriggerBasEquipmentManager_H