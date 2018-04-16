
#ifndef _ATS_GENA_MANAGER_H_
#define	_ATS_GENA_MANAGER_H_

#include <string>
#include <vector>
#include <map>

#include "app/ats/AtsAgent/GenaProtocolHandler/src/PowerStatusDataStore.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsManager.h"
#include "bus/protocol/Gena/src/GENALibrary.h"

namespace TA_Base_Bus
{
	class CtaScada;
}

namespace TA_IRS_Bus
{
	class IProtocolManager;
	class IAtsDataNotification;
	class IDataProcessor;
}

namespace TA_IRS_App
{
	class DataProcessorManager;
	class AtsDataNotificationManager;

	class AtsGenaManager : public TA_IRS_Bus::AtsManager
	{
	public:
		/**
		 * Constructor
		 */
		AtsGenaManager();
		/**
		 * Destructor
		 */
		virtual ~AtsGenaManager();

		/**
		 * createEntity
		 *
		 * Method to use to create a new Entity
		 * 
		 * @param entityData : The entity data
		 * @return IEntity*  : new IEntity* or NULL
		 */
		TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);
        
        /**
		 * addAllDataPoints
		 *
		 * Method to use to add all datapoints have been created.
		 * 
		 * @param entityData : The map of datapoints
		 */
        void addAllDataPoints(std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList);

		/**
		 * initializeData
		 *
		 * Method to use for object initialization
		 * 
		 */
		void initializeData();

		/**
		 * createProtocolManager
		 *
		 * A method use to create a new Protocol to manage the special entity type
		 *
		 * @return IProtocolManager* :  Protocol Specific object
		 */
		TA_IRS_Bus::IProtocolManager* createProtocolManager();

		/**
		 * createDataNotification
		 *
		 * A method use to create a new Data Notification to manage the Data Notification functionalities
		 *
		 * @return IAtsDataNotification* :  Data Notification object
		 */
		TA_IRS_Bus::IAtsDataNotification* createDataNotification();

		/**
		 * createDataProcessor
		 *
		 * A method use to create a new Data Processor to manage the Data processing functionalities
		 *
		 * @return IDataProcessor* :  Data Processor object
		 */
		TA_IRS_Bus::IDataProcessor*       createDataProcessor();

		/**
		 * setOperationMode
		 *
		 * A method use to set the agent operation mode
		 *
		 * @return eMode :  EOperation Mode
		 */
		virtual void setOperationMode(TA_Base_Core::EOperationMode eMode);

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		AtsGenaManager(const AtsGenaManager&);
		AtsGenaManager& operator= (const AtsGenaManager&);

		/**
		 * initialize
		 *
		 * A method use to initialize the local data
		 */
		void initialize();

		/**
		 * deInitialize
		 *
		 * A method use to deinitialize the local data
		 */
		void deInitialize();

		/**
		 * initializeScada
		 *
		 * A method use to initialize Scada 
		 */
		void initializeScada();


		//bool checkTrainAddressInfo(const std::string& sourceAddress);

		PowerStatusDataStore*						m_pPowerStatusDataStore;
		TA_Base_Bus::CtaScada*						m_scada;
		std::vector<TA_Base_Bus::DataPoint*>		m_dataPointCache;	

	};
}
#endif	//_ATS_GENA_MANAGER_H_