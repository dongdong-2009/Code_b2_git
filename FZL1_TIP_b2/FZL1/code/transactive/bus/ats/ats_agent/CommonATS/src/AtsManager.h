/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\AtsManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object handles and manages the initialization of protocol handlers.
  * It is abstract class that must be implemented by the protocol specific. 
  * Example: ModBusProtocol, SignallingProtocol, SOAPProtocol
  */

#ifndef _ATS_MANAGER_H_
#define _ATS_MANAGER_H_

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{
	class IEntity;
}

namespace TA_IRS_Bus
{
	class IProtocolManager;
	class IAtsDataNotification;
	class IDataProcessor;
	class AtsManager
	{
	public:
		/**
		 * Constructor
		 */
		AtsManager();

		/**
		 * Destructor
		 */
		virtual ~AtsManager();

		typedef std::map< unsigned long, TA_Base_Bus::DataPoint*> DataPointMap;
		/**
		 * createEntity
		 *
		 * Method to use to create a new Entity
		 * 
		 * @param entityData : The entity data
		 * @return IEntity*  : new IEntity* or NULL
		 */
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData) = 0;
		
		/**
		 * createProtocolManager
		 *
		 * A method use to create a new Protocol to manage the special entity type
		 *
		 * @return IProtocolManager* :  Protocol Specific object
		 */
		virtual IProtocolManager*     createProtocolManager() = 0;

		/**
		 * createDataNotification
		 *
		 * A method use to create a new Data Notification to manage the Data Notification functionalities
		 *
		 * @return IAtsDataNotification* :  Data Notification object
		 */
		virtual IAtsDataNotification* createDataNotification() = 0;

		/**
		 * createDataProcessor
		 *
		 * A method use to create a new Data Processor to manage the Data processing functionalities
		 *
		 * @return IDataProcessor* :  Data Processor object
		 */
		virtual IDataProcessor*       createDataProcessor() = 0;
		virtual void initializeData() = 0;

		void initializeAtsEntity(TA_Base_Core::IEntityDataPtr entityData);
	
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		AtsManager(const AtsManager&);
		AtsManager& operator= (const AtsManager&);

	protected:

		IProtocolManager*                   m_protocolManager;
		IAtsDataNotification*               m_dataNotification;
		IDataProcessor*                     m_DataProcessorManager;
		TA_Base_Core::AtsAgentEntityDataPtr m_atsEntityData;
	};
}
#endif