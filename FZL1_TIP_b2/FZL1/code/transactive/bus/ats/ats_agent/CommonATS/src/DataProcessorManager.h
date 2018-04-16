/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataProcessorManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is a base class for all data notification.
  * Specific Data object must extend and use this class for data event processing
  * E.g. PADataNotification, TrainDataNotification & PIDSDataNotification 
  */

#ifndef _DATA_PROCESSOR_MANAGER_H_
#define _DATA_PROCESSOR_MANAGER_H_

#include "bus/ats/ats_agent/CommonATS/src/IDataProcessor.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include <map>

namespace TA_IRS_Bus
{
	class IDataSync;
	class IDataDispatcher;
	class DataProcessorThread;

	class DataProcessorManager : public TA_IRS_Bus::IDataProcessor
	{
	public:
		/**
		 * Constructor
		 */
		DataProcessorManager(TA_Base_Core::AtsAgentEntityDataPtr EntityData);

		/**
		 * Destructor
		 */
		virtual ~DataProcessorManager();

		/**
		 * processDataDispatcher
		 *
		 * This method will call data dispatcher to start processing
		 * 
		 * @param type : Ats Message Type
		 */
		void processDataDispatcher(TA_IRS_Bus::AtsMessageType type);

		/**
		 * registerDataDispatch
		 *
		 * This method is used to register Data dispatcher based on ats message type.
		 * 
		 * @param type : Ats Message Type
		 * @param dataDispatch : Data Dispatcher object
		 */
		void registerDataDispatch(TA_IRS_Bus::AtsMessageType type, TA_IRS_Bus::IDataDispatcher* dataDispatch);

		/**
		 * deRegisterDataDispatch
		 *
		 * This method is used to deregister Data dispatcher based on ats message type.
		 * 
		 * @param type : Ats Message Type
		 * @param dataDispatch : Data Dispatcher object
		 */
		void deRegisterDataDispatch(TA_IRS_Bus::AtsMessageType type);
			
		/**
		 * processDataSync
		 *
		 * This method will call data sychronization to start Sync the data
		 * 
		 * @param type : Ats Message Type
		 */
		void processDataSync(TA_IRS_Bus::AtsMessageType type);

		/**
		 * registerDataSynch
		 *
		 * This method is used to register Data Synch based on ats message type.
		 * 
		 * @param type : Ats Message Type
		 * @param dataSynch : Data Synch object
		 */
		void registerDataSynch(TA_IRS_Bus::AtsMessageType type, TA_IRS_Bus::IDataSync* dataSynch);
		void deRegisterDataSynch(TA_IRS_Bus::AtsMessageType type);


		void processDataNotification(DataNotificationPtr& newEvent);

		void registerAtsDataProcessor(AtsMessageType type, DataProcessorThread* dataProcess);
		void deRegisterAtsDataProcessor(AtsMessageType type);

	private:
		/**
		 * Disable copy, and assignment operator
		 */
		DataProcessorManager(const DataProcessorManager&);
		DataProcessorManager& operator= (const DataProcessorManager&);

		DataProcessorThread* getAtsDataProcessor(AtsMessageType type);


		TA_Base_Core::AtsAgentEntityDataPtr m_atsEntityData;
		DataSyncProcessorMap m_dataSyncMap;
		DispatcherProcessorMap m_dataDispatchMap;
		AtsDataProcessorMap m_atsDataProcessorMap;
	};
}
#endif