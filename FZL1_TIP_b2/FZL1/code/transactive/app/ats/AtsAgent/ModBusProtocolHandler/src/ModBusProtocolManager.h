/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\AtsDataNotificationManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is a thread object which is responsible for handling RTU Library to receive the data for data processing.
  */

#ifndef _MODBUS_PROTOCOL_MANAGER_H_
#define _MODBUS_PROTOCOL_MANAGER_H_

#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/scada/rtu_library/src/DataBlockInterface.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/rtu_library/src/DataBlockInterface.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include <vector>
#include "core/types/public_types/Constants.h"

namespace TA_IRS_Bus
{
	class IAtsDataNotification;
}
namespace TA_IRS_App
{
	class ModBusProtocolManager	 : public TA_IRS_Bus::IProtocolManager, 
								   public TA_Base_Bus::DataBlockObserver,
								   public TA_Base_Core::Thread
	{
	public:
		ModBusProtocolManager(TA_Base_Core::AtsAgentEntityDataPtr atsEntity, TA_Base_Bus::IProcessDataBlock& processDataBlock);
		virtual ~ModBusProtocolManager();

		/**
		 * registerProtocolParser
		 *
		 * Method to use to register Protocol parser
		 * 
		 * @param type            : Ats message type
		 * @param protocolParser  : Protocol parser object
		 */
		void registerProtocolParser(TA_IRS_Bus::AtsMessageType type,TA_IRS_Bus::IProtocolParser* protocolParser);
		
		/**
		 * deRegisterProtocolParser
		 *
		 * Method to use to register Protocol parser
		 * 
		 * @param type            : Ats message type
		 */
		void deRegisterProtocolParser(TA_IRS_Bus::AtsMessageType type);
	
		/**
		 * getDataBlock
		 *
		 * Method to use to get Data Block from the raw data
		 * 
		 * @return DataBlock<WORD>&   : Raw Data
		 */
		TA_Base_Bus::DataBlock<WORD>& getDataBlock();

		/**
		 * updataDataBlock
		 *
		 * Method to use to update the data block
		 * 
		 * @param dataBlock  : DataBlock<WORD>
		 */
		void updataDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock);

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

		/**
		 * run
		 *
		 * Method to use to start the thread
		 */
		virtual void run();

		/**
		 * terminate
		 *
		 * Method to use to terminate the thread
		 */
		virtual void terminate();

		/**
		 * updateQualityStatus
		 *
		 * Method to use to set the Quality of the DataBlock;
		 */
		void updateQualityStatus(const TA_Base_Core::EDataPointQuality& quality);

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		ModBusProtocolManager(const ModBusProtocolManager&);
		ModBusProtocolManager& operator= (const ModBusProtocolManager&);

		/**
		 * startProcolProcessing
		 *
		 * Method to use to start the Protocol processing
		 */
		void startProcolProcessing();

		/**
		 * startProcolProcessing
		 *
		 * Method to use to stop the Protocol processing
		 */
		void stopProtocolProcessing();

		/**
		 * updatePowerStatus
		 *
		 * Method to use to update power status
		 */
		void updatePowerStatus();
		TA_IRS_Bus::RawTable m_rawTable;
		TA_Base_Bus::IProcessDataBlock& m_processDataBlock;

		TA_IRS_Bus::ParserProcessorMap m_parserProcessors;
		TA_IRS_Bus::IAtsDataNotification* m_dataNotif;
		bool m_bPowerRequired;
		bool m_keepRunning;
		TA_Base_Core::Semaphore m_semaphore;
		TA_Base_Core::ReEntrantThreadLockable m_lockCache;


	};
}
#endif