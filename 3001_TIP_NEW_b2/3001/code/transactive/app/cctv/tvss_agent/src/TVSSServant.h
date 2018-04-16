/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP/3001/transactive/app/cctv/tvs_agent/src/TVSSServant.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/04 10:43:52 $
  * Last modified by:  $Author:  $
  *
  */
#ifndef TVSS_SERVANT_H
#define TVSS_SERVANT_H
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/src/CommsMessageSender.h"

#include "bus/cctv/tvss_agent/TVSSManager/IDL/src/TVSSManagerCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"
#include "app/cctv/tvss_agent/src/TVSSServerLinkStatusUpdateThread.h"
#include "app/cctv/tvss_agent/src/TVSS.h"

#include "app/cctv/tvss_agent/TCPCommunication/src/ITCPMessageProcessor.h"
#include "core/utilities/src/RunParams.h"

#include <string>
#include <map>
#include <memory>

#define TRAINBORNE_EVENT_NUMBER				75
#define TYPE_TRAINBORNE_CAMERA_ALARM		"TrainBorneCameraAlarm"

namespace TA_IRS_App
{
	class TVSS;

	//Method
	class TVSSMethod;
	class TVSSClearImageMethod;
	class TVSSQuadScreenMethod;
	class TVSSSingleScreenMethod;
	class TVSSSequencingScreenMethod;
	class TVSSEquipmentQueryMethod;
	class TVSSAlarmProcessor;
    class TrainBorneHeartBeatProcessor;

	class TrainBorneMonitor
	{
	public:
		TrainBorneMonitor(
			unsigned long entityKey,
			const std::string& name,
			const std::string& description,
			const std::string& addr,
			unsigned int displayOrder,
			unsigned long consoleKey,
			bool isOverallMonitor
		):m_entityKey(entityKey),m_name(name),m_description(description),m_addr(addr),m_displayOrder(displayOrder),m_consoleKey(consoleKey),m_isOverallMonitor(isOverallMonitor)
		{};
		unsigned long m_entityKey;
		std::string m_name;
		std::string m_description;
		std::string m_addr;
		unsigned int m_displayOrder;
		unsigned long m_consoleKey;
		bool m_isOverallMonitor;
	};

	class TrainBorneCameraAlarm
	{
	public:
		unsigned long   pkey;
		unsigned long	locationKey;
		unsigned long	typeKey;
		unsigned long	subSystemKey;
		std::string     name;
		std::string     address;
		std::string		description;
		std::string     cameraNumbers;
		std::string		assetName;
	};
	
	class TVSSServant: public virtual POA_TA_Base_Bus::TVSSManagerCorbaDef,
					   public virtual TA_Base_Core::ServantBase,
					   public virtual TA_Base_Bus::IEntity,
					   public ITCPMessageProcessor,
             public TA_Base_Core::RunParamUser
	{
	public:

		TVSSServant(std::string strTCPAddress1,std::string strTCPAddress2,std::string strTrainBorneAddress1,std::string strTrainborneAddress2,unsigned long timeout);
		virtual ~TVSSServant();
		
		void setEntity(TA_Base_Core::IEntityData &EntityData);
		void setAlarmProcessor(TVSSAlarmProcessor* processor);
        void setTrainborneHeartbeatProcessor(TrainBorneHeartBeatProcessor* trainborneHeartbeatProcessor);
		void startServerLinkStatusThread() { m_serverLinkStatusThread->start(); };//limin, called by RadioTetraAgent after running control
		int GetDPCount();
		/**
		 * Implement TVSSManagerCorbaDef Methods
		 */
		virtual void tvssClearImage(CORBA::Long trainId,CORBA::Char monitorId );
		//virtual void tvssQuadScreen(CORBA::Long trainId,CORBA::Char monitorId, CORBA::Char cameraW, CORBA::Char cameraX, CORBA::Char cameraY, CORBA::Char cameraZ );
		virtual void tvssQuadScreen(CORBA::Long trainId,CORBA::Char monitorId, CORBA::Char quadId );
		virtual void tvssSingleScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX );
		virtual void tvssPecSingleScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX );
		virtual void tvssSequencingScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char sequenceId ); 
		virtual void tvssAcknowledgeEvent(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX,CORBA::Char cameraY );
		virtual void tvssTrainBorneEvent(CORBA::Long trainId,const char* eventStr);
		virtual void tvssEquipmentAlarmQuery();
        virtual void tvssTrainborneHeartbeat();
		virtual void tvssAcknowledgeAlarm(::CORBA::ULong entityKey, const char* sessionID);
		
		// IEntity Interface Functions
		virtual bool isValid(void);
		virtual void start(void);
		virtual void stop(void);
		virtual void update(const class TA_Base_Core::ConfigUpdateDetails &details);
		virtual void remove(void);
		virtual void setToControlMode(void);
		virtual void setToMonitorMode(void);
		virtual void onTerminate(bool peerAvailable);
		void setEntityAddressMap(std::map<std::string, TrainBorneCameraAlarm*> entityAddressMap);
		void setEntityKeyMap(std::map<unsigned long, TrainBorneCameraAlarm*> entityKeyMap);
		void setMonitorConsoleMap(std::map<unsigned long,TrainBorneMonitor*> monitorConsoleMap);
		virtual void HandleTVSSTrainBorneEvent(long trainID,const char*eventStr);
		virtual void onRunParamChange(const std::string& name, const std::string& value);
	private:
		bool ChangeAlaram(TrainBorneCameraAlarm* entityData,long trainID,bool bAlarmActived);
		bool RaiseAlaram(TrainBorneCameraAlarm* entityData,long trainID);
		bool CloseAlaram(TrainBorneCameraAlarm* entityData);
		

	private:

		std::string         m_objectName;
		unsigned long       m_EntityKey;
		unsigned long   	m_entityLocation;
		unsigned long		m_entitySubsystem;
		unsigned long		m_entityTypeKey;
    std::string     m_locationName;
		std::string			m_assetName;
		std::string			m_primaryAddress,m_secondaryAddress;
		std::string			m_primaryTrainborneAddress,m_secondaryTrainborneAddress;
		TA_IRS_App::TVSS*   m_tvss;
		TVSSAlarmProcessor* m_alarmProcessor;
        TrainBorneHeartBeatProcessor* m_trainborneHeartbeatProcessor;
		bool                m_active;

		TVSSServerLinkStatusUpdateThread*    m_serverLinkStatusThread;
		std::map<std::string,TrainBorneCameraAlarm*>		 m_alarmAddressMap;
		std::map<unsigned long ,TrainBorneCameraAlarm*>		 m_alarmKeyMap;
		std::map<unsigned long,TrainBorneMonitor*>     m_monitorConsoleMap;
		std::auto_ptr< TA_Base_Core::CommsMessageSender > m_msgSender;
		std::map<std::string,bool>			 m_eventAlarmCache;	
		unsigned long						 m_timeout;
    // Cache the monitor assigned info
    std::map<CORBA::Char, CORBA::Long> m_monitorIdToAssignedTrainMap;
    std::map<unsigned char, unsigned long> m_monitorIdToKeyMap;
	};
};
#endif