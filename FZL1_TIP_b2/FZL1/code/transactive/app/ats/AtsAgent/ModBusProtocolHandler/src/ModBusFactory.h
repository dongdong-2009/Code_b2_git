/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\ModBusFactory.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is a factory object which is responsible for creating the Parser, DataDispatcher and Data Synch object
  */

#ifndef _MODBUS_FACTORY_H_
#define _MODBUS_FACTORY_H_

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_Bus
{
	class IProtocolParser;
	class IDataDispatcher;
	class IDataSync;
	class DataProcessorThread;
}

namespace TA_IRS_App
{
	class ModBusFactory
	{
	public:
		/**
          * getInstance
		  * 
		  * Static function to retrieve the single instance of this class
          */
		static ModBusFactory* getInstance();

		/**
		 * createParser
		 *
		 * Method to use to create Parser object
		 * 
		 * @param type       : Ats Message type
		 * @param EntityData : Ats Entity Data
		 */
		TA_IRS_Bus::IProtocolParser* createParser(TA_IRS_Bus::AtsMessageType type,TA_Base_Core::AtsAgentEntityDataPtr EntityData);

		/**
		 * createDispatcher
		 *
		 * Method to use to create Dispatcher object
		 * 
		 * @param type       : Ats Message type
		 */
		TA_IRS_Bus::IDataDispatcher* createDispatcher(TA_IRS_Bus::AtsMessageType type);

		/**
		 * createDataSynch
		 *
		 * Method to use to create Data Synch object
		 * 
		 * @param type       : Ats Message type
		 * @param EntityData : Ats Entity Data
		 */
		TA_IRS_Bus::IDataSync* createDataSynch(TA_IRS_Bus::AtsMessageType type,TA_Base_Core::AtsAgentEntityDataPtr EntityData);

		TA_IRS_Bus::DataProcessorThread* createAtsDataProcessor(TA_IRS_Bus::AtsMessageType type, TA_Base_Core::AtsAgentEntityDataPtr EntityData);

	private:
		/**
		 * Disable default, copy, and assignment construct
		 */
		ModBusFactory();
		ModBusFactory(const ModBusFactory&);
		ModBusFactory& operator= (const ModBusFactory&);
		virtual ~ModBusFactory();

		static ModBusFactory* m_instance;
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

	};
}
#endif