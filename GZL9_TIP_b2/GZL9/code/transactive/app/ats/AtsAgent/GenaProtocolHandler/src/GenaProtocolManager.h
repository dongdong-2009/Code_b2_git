
#ifndef _GENA_PROTOCOL_MANAGER_H_
#define _GENA_PROTOCOL_MANAGER_H_

#include <vector>

#include "app/ats/AtsAgent/GenaProtocolHandler/src/AtsGenaManager.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"

#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"
#include "bus/protocol/Gena/src/IServerStatusObserver.h"
#include "bus/protocol/Gena/src/IGENATopicDataStore.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/types/public_types/Constants.h"
#include "bus/scada/ItaScada/src/CtaScada.h"

namespace TA_IRS_Bus
{
	class IAtsDataNotification;
}

namespace TA_IRS_App
{
	typedef std::map<TA_IRS_Bus::AtsMessageType, GenaBaseTableParser*> GenaParserMap;
	typedef GenaParserMap::iterator	GenaParserMapIter;

	class AtsHeartBeatManager;
	class GenaProtocolManager	 : public TA_IRS_Bus::IProtocolManager, 
								   public TA_Base_Bus::IGENATopicDataStore,
								   public TA_Base_Bus::IConnStateListener
	{
	public:
		GenaProtocolManager(TA_Base_Bus::ItaScada* itScada);
		~GenaProtocolManager();

		/**
		 * registerProtocolParser
		 *
		 * Method to use to register Protocol parser
		 * 
		 * @param type            : Ats message type
		 * @param protocolParser  : Protocol parser object
		 */
		//void registerProtocolParser(TA_IRS_Bus::AtsMessageType type, GenaBaseTableParser* protocolParser);
		virtual void registerProtocolParser(TA_IRS_Bus::AtsMessageType type,TA_IRS_Bus::IProtocolParser* protocolParser);
		
		/**
		 * deRegisterProtocolParser
		 *
		 * Method to use to register Protocol parser
		 * 
		 * @param type            : Ats message type
		 */
		void deRegisterProtocolParser(TA_IRS_Bus::AtsMessageType type);

		/**
		 * setAtsDataNotification
		 *
		 * Method to use to set Data Notification object
		 * 
		 * @param atsDataNofic  : Data Notification object
		 */
		void setAtsDataNotification(TA_IRS_Bus::IAtsDataNotification* atsDataNofic);

		/**
		 * setOperationMode
		 *
		 * Method to use to set the Agent operation mode
		 * 
		 * @param eMode  :  EOperationMode object
		 */
		void setOperationMode(TA_Base_Core::EOperationMode eMode);


		virtual TA_Base_Bus::T_GENATopicDataList getAllData(const std::string& topicName )
		{ 
			/*Do nothing here*/
			TA_Base_Bus::T_GENATopicDataList tmp;
			tmp.clear();
			return tmp;
		};

		virtual void synchronizeBegin(const std::string& topicName);
		virtual void synchronizeEnd(const std::string& topicName);

	protected:
		/**
		 * updateDataImpl
		 *
		 * Method to observe register topic data update
		 */

		virtual void updateDataImpl(const std::string& topicName, TA_Base_Bus::GENATopicDataPtr pData);

		/**
		 * onStateChanged
		 *
		 * Method to observe server status change
		 */

		virtual void onStateChanged(const std::string& strDpAddress, const GENAConnectionStatus& status );

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		GenaProtocolManager();
		GenaProtocolManager(const GenaProtocolManager&);
		GenaProtocolManager& operator= (const GenaProtocolManager&);


		void checkDataPointQuality();

		void setAllDataPointQuality();

		void updataConnectStatDp(const std::string& dpAddress, unsigned long dpValue);


		bool	m_bPowerRequired;
		bool	m_isControlMode;

		TA_Base_Core::Semaphore					m_semaphore;
		TA_Base_Core::ReEntrantThreadLockable	m_lockCache;

		TA_Base_Bus::GENALibrary*				m_pGenaLibrary;
		GenaParserMap							m_parserProcessors;

		GenaRawTable							m_genaTable;
		TA_IRS_Bus::IAtsDataNotification*		m_dataNotif;

		TA_Base_Bus::ItaScada*					m_pScada;

	};
}
#endif //_GENA_PROTOCOL_MANAGER_H_