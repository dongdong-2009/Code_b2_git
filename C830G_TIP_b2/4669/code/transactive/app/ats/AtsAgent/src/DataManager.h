/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/DataManager.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "core/corba/src/ServantBase.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AtsTwpEntityData.h"
#include "core/data_access_interface/entity_access/src/AtsPsdEntityData.h"
#include "core/data_access_interface/ats/src/IndexedAtsDssPlanMaps.h"

#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/WatchDogData.h"
#include "app/ats/AtsAgent/src/AtsCctvData.h"
#include "app/ats/AtsAgent/src/AtsEcsData.h"
#include "app/ats/AtsAgent/src/AtsTrainData.h"
#include "app/ats/AtsAgent/src/AtsPasData.h"
#include "app/ats/AtsAgent/src/AtsPsdData.h"
#include "app/ats/AtsAgent/src/AtsTisData.h"
#include "app/ats/AtsAgent/src/AtsDssData.h"
#include "app/ats/AtsAgent/src/AtsWildData.h"
#include "app/ats/AtsAgent/src/AtsTwpData.h"
#include "app/ats/AtsAgent/src/PowerData.h"
#include "app/ats/AtsAgent/src/SectorData.h"

#include "app/ats/AtsAgent/src/DataPointHelper.h"
#include "app/ats/AtsAgent/src/OccUpdateProcessor.h"
#include "app/ats/AtsAgent/src/OperationModeManager.h"
#include "app/ats/AtsAgent/src/DataPointControlStateProcessor.h"

#include "app/ats/AtsAgent/src/AtsTrainDataTimer.h"
#include "app/ats/AtsAgent/src/IAtsStatusObserver.h"

namespace TA_Base_Bus
{
    class IEntity;
}

namespace TA_IRS_App
{
    class AtsServerLinkStatusUpdateThread;
}

namespace TA_IRS_App
{
    class DataManager : public virtual TA_Base_Core::ServantBase,
						 public virtual POA_TA_IRS_Bus::IAtsCorbaDef,
						public virtual TA_Base_Core::Thread,
                        public virtual IAtsStatusObserver
    {
    public:

        /**
          * Constructor
          */
        DataManager(TA_Base_Core::AtsAgentEntityDataPtr entityData,
					 const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap,
					 OperationModeManager& operationModeManager,
                     AtsServerLinkStatusUpdateThread& linkStatusThread );

        /**
          * Destructor
          */
        virtual ~DataManager();

		//
		// IAtsCorbaDef Implementation
		//

		/**
          * getEcsInformationList
          *
          * This method is used to access the full list of ECS information
		  * and will typlically used to synchronise at startup of the client
		  * application
          *
		  * @return A sequence of AtsEcsMessageCorbaDef for all trains known
		  *			to the ATS agent.
          */
		virtual TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList* getEcsInformationList();

		/**
          * processOccEcsUpdate
          *
          * This method is used to allow the OCC ATS Agent to pass on ECS
		  * information
          *
		  * @param	ecsUpdate	A sequence of AtsEcsMessageCorbaDef for updates  
		  *						to trains for the sector
          */
		virtual void processOccEcsUpdate(const TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList& ecsUpdate);
		
		/**
          * getWildInformationList
          *
          * This method is used to access the full list of WILD information
		  * and will typlically used to synchronise at startup of the client
		  * application
          *
		  * @return A sequence of AtsWildMessageCorbaDef for all trains known
		  *			to the ATS agent for WILD.
          */
		virtual TA_IRS_Bus::IAtsWildCorbaDef::WildInfoList* getWildInformationList();

		/**
          * getTrainInformationList
          *
          * This method is used to access the full list of Train information
		  * and will typlically used to synchronise at startup of the client
		  * application
          *
		  * @return A sequence of AtsTrainMessageCorbaDef for all platforms known
		  *			to the ATS agent.
          */
		virtual TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList* getTrainInformationList();

		/**
          * processOccTrainUpdate
          *
          * This method is used to allow the OCC ATS Agent to pass on Train
		  * information
          *
		  * @param	trainUpdate	A sequence of AtsTrainMessageCorbaDef for updates  
		  *						to trains for the sector
          */
		virtual void processOccTrainUpdate(const TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList& trainUpdate);

		/**
          * sendOa1TableToAts
          *
          * This method is used to send OA1 tables to ATS
          *
		  * @param physicalTrainNumber	The identifier of the train that
		  *								originated the OA1 table
		  * @param oa1					A 24 byte OA1 table to send to ATS
          */
		virtual void sendOa1TableToAts(	TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, 
										const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& oa1);

		/**
          * sendAtcTrainStatusReportToAts
          *
          * This method is used to send Atc Alarm tables to ATS
          *
		  * @param physicalTrainNumber		The identifier of the train that
		  *									originated the OA1 table
		  *	@param atcTrainStatusCodeHeader	A code header for the Atc data
		  * @param atcTrainStatus			A 22 byte Atc data table to send to ATS
          */
		virtual void sendAtcTrainStatusReportToAts(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, 
												   const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& atcTrainStatus);

		/**
          * getTisInformationList
          *
          * This method is used to access the full list of TIS information
		  * and will typlically used to synchronise at startup of the client
		  * application
          *
		  * @return A sequence of AtsTisMessageCorbaDef for all platforms known
		  *			to the ATS agent. There will be two AtsTisMessageCorbaDef in
		  *			the sequence for each platform - one for platform information,
		  *			one for train information.
          */
		virtual TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList* getTisInformationList();

		/**
          * processOccTisUpdate
          *
          * This method is used to allow the OCC ATS Agent to pass on TIS
		  * information
          *
		  * @param	tisUpdate	A sequence of AtsTisMessageCorbaDef for updates  
		  *						to platforms for the station
          */
		virtual void processOccTisUpdate(const TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList& tisUpdate);

		/**
		  * getPasInformationList
		  *
		  * This method is used to access the full list of PAS information
		  * and will typlically used to synchronise at startup of the client
		  * application
		  *
		  * @return A sequence of AtsPasMessageCorbaDef for all trains known
		  *			to the ATS agent.
		  */
		virtual TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* getPasInformationList();

		/**
          * processOccPasUpdate
          *
          * This method is used to allow the OCC ATS Agent to pass on PAS
		  * updates
          *
		  * @param	pasUpdate	A sequence of AtsPasMessageCorbaDef for updates  
		  *						to platforms for the station
          */
		virtual void processOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate);

		/**
          * getCctvInformationList
          *
          * This method is used to access the full list of CCTV information
		  * and will typlically used to synchronise at startup of the client
		  * application
          *
		  * @return A sequence of AtsCctvMessageCorbaDef for all trains known
		  *			to the ATS agent.
          */
		virtual TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList* getCctvInformationList();

		/**
          * processOccCctvUpdate
          *
          * This method is used to allow the OCC ATS Agent to pass on CCTV
		  * information
          *
		  * @param	cctvUpdate	A sequence of AtsCctvMessageCorbaDef for updates  
		  *						to trains for the sector
          */
		virtual void processOccCctvUpdate(const TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList& cctvUpdate);

		/**
          * inOccDoubleAtsFailure
          *
          * This method is used to retrieve the double ATS failure state of the agent
          *
          */
		virtual CORBA::Boolean inOccDoubleAtsFailure();

		/**
          * radioInFallback
          *
          * This method is used to send Radio Fallback state to ATS
          *
		  * @param radioFallback			True if radio is in fallback, false otherwise
          */
		virtual void radioInFallback(CORBA::Boolean radioFallback);

		/**
          * resetAts
          *
          * This method is used to reset ATS after some form of degraded mode
          *
          */
		virtual void resetAts();


		//
		//	Thread Methods
		//

		// Define a run() method to do your work. This should block until
		// terminate() is called eg: CorbaUtil::GetInstance().Run() or
		// m_dialog->DoModal()
		//
		// NOTE: do not call this method directly. It needs to be public so that it
		//       can be called from the thread-spawning function
		virtual void run();

		// The terminate() method should cause run() to return. eg:
		// CorbaUtil::GetInstance().Shutdown() or EndDialog(m_dialog)
		//
		// NOTE: probably not a good idea to call this method directly - call
		//       terminateAndWait() instead
		virtual void terminate();

		void processUpdates();
		
		// Change between Monitor/Control modes
		void setToMonitorMode();
		void setToControlMode();
		void updateControlModeStatus(bool inControl); //wenching++ (TD14526)

		//
		// WatchDog
		//

		WatchDogData& getWatchDogData();

		//
		// ECS
		//
		AtsEcsData& getEcsData(const TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);
		
		//
		// Train
		//
		AtsTrainData& getTrainData(const TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);
		
		typedef std::map<TA_IRS_Bus::PhysicalTrainNumberType, AtsTrainData*> TrainDataMap;
		typedef TrainDataMap::iterator TrainDataMapIt;

		TrainDataMap&	getTrainDataMap();

		void			clearTrainImportFlags();
		void			checkIfTrainNotImported();

		//
		//	CCTV
		//
		typedef TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef CctvMessage;
		typedef std::vector<CctvMessage> CctvMessageVector;
		typedef std::map<unsigned long, CctvMessageVector> CctvMessageMap;

		AtsCctvData& getCctvData(const TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);

		AtsTisData&		getTisData(const TA_IRS_Bus::PlatformIdType platformId);
		AtsPasData&		getPasData(const TA_IRS_Bus::PlatformIdType platformId);
		AtsPsdData&		getPsdData(const TA_IRS_Bus::PlatformIdType platformId);

		typedef std::map<TA_IRS_Bus::PlatformIdType, AtsPsdData*> PsdDataMap;
		typedef PsdDataMap::iterator PsdDataMapIt;

		PsdDataMap&		getPsdDataMap();

		AtsTwpData&		getTwpData();

		AtsWildData&	getWildData(unsigned int index);

		void addNewDssEvent(std::vector< AtsDssData >& newData);

		typedef std::map<unsigned short, PowerData*> PowerDataMap;
		typedef PowerDataMap::iterator PowerDataMapIt;

		PowerData&		getPowerData(unsigned short subsectionId);
		PowerDataMap&	getPowerDataMap();
		void updatePSDWhenDoubleATSFail();		//TD17760, hongzhi
		//
		// Operation Mode Manager
		//

		OperationModeManager& getOperationModeManager();


        /**
         * Called when the ATS server health status changes
         * from good to bad or bad to good.
         *
         * @param atsOk true if at least one ATS server is healthy
         */
        virtual void atsServerStatusChanged(bool atsOk);

		//TD15213
		void setMAtsTwpEntityData(TA_Base_Core::AtsTwpEntityData* patstwpentitydata);
		void setMAtsPsdEntityData(TA_Base_Core::AtsPsdEntityData* patspsdentitydata);

		//TD 16335
		//zhou yuan++
		unsigned long getStationIdByPlatformId(unsigned long platformId);
		//++zhou yuan

	protected:

		void createOccPlatformData(unsigned long entityKey);
		void createStationPlatformData(unsigned long entityKey);
		bool ensureNormalMode();

		void localProcessOccEcsUpdate(const TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList& ecsUpdate);
		void localProcessOccTrainUpdate(const TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList& trainUpdate);
		void localProcessOccTisUpdate(const TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList& tisUpdate);
		void localProcessOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate);
		void localProcessOccCctvUpdate(const TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList& cctvUpdate);

		void triggerDssPlan(const unsigned long event, const std::string& input);

    private:
        // Disable default constructor, copy constructor and assignment operator
		DataManager();
        DataManager( const DataManager& atsAgent);
        DataManager& operator=(const DataManager &);

	protected:

		TA_Base_Core::AtsAgentEntityDataPtr		m_entityData;

		unsigned long						m_locationKey;
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType 
											m_locationType;

		// The DataPointHelper for the Agent
		DataPointHelper*					m_dataPointHelper;

		TA_IRS_Core::IAtsFixedBlockMap*			m_stationZones;
		TA_IRS_Core::IAtsFixedBlockMap*			m_cctvZones;
		TA_IRS_Core::IAtsFixedBlockMap*			m_ecsZones;

		typedef std::map<TA_IRS_Bus::PhysicalTrainNumberType, AtsCctvData*> CctvDataMap;
		typedef CctvDataMap::iterator CctvDataMapIt;

		typedef std::map<TA_IRS_Bus::PhysicalTrainNumberType, AtsEcsData*> EcsDataMap;
		typedef EcsDataMap::iterator EcsDataMapIt;

		typedef std::map<TA_IRS_Bus::PlatformIdType, AtsPasData*> PasDataMap;
		typedef PasDataMap::iterator PasDataMapIt;

		typedef std::map<TA_IRS_Bus::PlatformIdType, AtsTisData*> TisDataMap;
		typedef TisDataMap::iterator TisDataMapIt;

		typedef std::vector< AtsDssData >	DssDataArray;
		typedef AtsWildData* WildDataArray[5];

		WatchDogData*						m_watchDogData;

		CctvDataMap							m_cctvDataMap;
		EcsDataMap							m_ecsDataMap;
		TrainDataMap						m_trainDataMap;

		PasDataMap							m_pasDataMap;
		PsdDataMap							m_psdDataMap;
		TisDataMap							m_tisDataMap;

		DssDataArray						m_dssDataArray;
		WildDataArray						m_wildDataArray;

		PowerDataMap						m_powerDataMap;

		AtsTwpData*							m_twpData;

		volatile bool						m_keepRunning;
		TA_Base_Core::Semaphore					m_semaphore;

		TA_IRS_Core::IndexedAtsDssPlanMaps*		m_dssPlanMap;
		TA_Base_Bus::PlanAgentLibrary			m_planAgentLibrary;

		OccUpdateProcessor*					m_occUpdateProcessor;
		time_t								m_occUpdateTime;
		OccUpdateProcessor::AtsAgentNamedObject*
											m_occAtsAgent;
		OperationModeManager&				m_operationModeManager;
		SectorData							m_sectorData;

		TA_Base_Core::ReEntrantThreadLockable m_dssGuard;

		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table
		AtsTrainDataTimer* m_atsTrainDataTimer;
		//++zhou yuan

		//TD 16335
		//zhou yuan++
		std::map< unsigned long, unsigned long > m_platformIdToStationMap;
		//++zhou yuan

		DataPointControlStateProcessor* m_dpControlStateProcessor;

		//Hodle Ats previous Link Status
		bool                                m_bAtsRecoverLink;

		AtsServerLinkStatusUpdateThread&    m_linkStatusThread;
		
    };
}

#endif // !defined(ATSAGENT_H)

