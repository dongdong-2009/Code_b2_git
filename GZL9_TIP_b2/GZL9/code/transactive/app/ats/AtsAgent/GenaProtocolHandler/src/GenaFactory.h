
#ifndef _GENA_FACTORY_H_
#define _GENA_FACTORY_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Bus
{
	class IProtocolParser;
	class IDataDispatcher;
	class IDataSync;
	class DataProcessorThread;
}

namespace TA_IRS_App
{
	class GenaFactory
	{
	public:
		/**
          * getInstance
		  * 
		  * Static function to retrieve the single instance of this class
          */
		static GenaFactory* getInstance();

		/**
		 * createParser
		 *
		 * Method to use to create Parser object
		 * 
		 * @param type       : Ats Message type
		 * @param EntityData : Ats Entity Data
		 */
		TA_IRS_App::GenaBaseTableParser* createParser(TA_IRS_Bus::AtsMessageType type);

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
		GenaFactory();
		GenaFactory(const GenaFactory&);
		GenaFactory& operator= (const GenaFactory&);
		virtual ~GenaFactory();

		static GenaFactory* m_instance;
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

	};
}
#endif //_GENA_FACTORY_H_