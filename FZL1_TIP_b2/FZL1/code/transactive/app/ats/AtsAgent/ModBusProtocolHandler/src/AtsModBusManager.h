/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\AtsModBusManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling and managing Protocol Handling, Data Notification and Data processing.
  */

#ifndef _ATS_MODBUS_MANAGER_H_
#define	_ATS_MODBUS_MANAGER_H_

#include "bus/ats/ats_agent/CommonATS/src/AtsManager.h"
#include <string>
#include <vector>

namespace TA_Base_Bus
{
	class BaseRTUManager;
	class CtaScada;
	class Datapoint;
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

	class AtsModBusManager : public TA_IRS_Bus::AtsManager
	{
	public:
		/**
		 * Constructor
		 */
		AtsModBusManager();
		/**
		 * Destructor
		 */
		virtual ~AtsModBusManager();

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
		void setOperationMode(TA_Base_Core::EOperationMode eMode);

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		AtsModBusManager(const AtsModBusManager&);
		AtsModBusManager& operator= (const AtsModBusManager&);

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
		 * initializeRTUManager
		 *
		 * A method use to initialize RTU Manager
		 */
		void initializeRTUManager();

		/**
		 * initializeScada
		 *
		 * A method use to initialize Scada 
		 */
		void initializeScada();


		bool checkTrainAddressInfo(const std::string& sourceAddress);

		TA_Base_Bus::BaseRTUManager*    		m_rtuManager;
		TA_Base_Bus::CtaScada*      			m_scada;
		std::vector<TA_Base_Bus::DataPoint*>    m_dataPointCache;

	};
}
#endif