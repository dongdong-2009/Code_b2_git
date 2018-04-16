/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataCache.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object maintains all data cache maps for ATS
  */

#ifndef _DATA_CACHE_H_
#define _DATA_CACHE_H_

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/ats/ats_agent/CommonATS/src/IPowerStatusUpdate.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/types/public_types/Constants.h"

namespace TA_IRS_Bus
{
	class AtsPasData;
	class DataPointHelper;
	class TrainDataLocation;
	class TrainPisData;

	class DataCache : public TA_IRS_Bus::IPowerStatusUpdate
	{
	public:
		/**
          * getInstance
		  * 
		  * Static function to retrieve the single instance of this class
          */
		static DataCache* getInstance();

		/**
          * releaseInstance
		  * 
		  * releases or free the single instance object of this class
          */
		static void releaseInstance();

		/**
		 * updatePACache
		 *
		 * Method to use to update PA information 
		 * 
		 * @param paStationInfo : The PA Station information
		 */
		void updatePACache(const PAStationInfoVec& paStationInfo);

		/**
		 * updateTrainCache
		 *
		 * Method to use to update Train information
		 * 
		 * @param trainInfo : The Train information
		 * @param quality : Data Block Quality
		 */
		void updateTrainCache(TrainInfoVec& trainInfo);

		/**
		 * updateTrainInfoDatapointQuality
		 *
		 * Method to use to update Train Datapoint quality
		 * 
		 * @param quality : Data Block Quality
		 */
		void updateTrainInfoDatapointQuality(TA_Base_Core::EDataPointQuality quality);

		/**
		 * initialize
		 *
		 * This method must be called first for initialization.
		 * 
		 * @param itScada : Scada object
		 * @param entityData : ATS Entity Data
		 */
		void initialize(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr entityData);

		/**
		 * setPIDSData
		 *
		 * Method used to set PIDS Data
		 * 
		 * @param data : DataBlock object
		 */
		void setPIDSData(const TA_Base_Bus::DataBlock<WORD>& data);

		/**
		 * getPIDSData
		 *
		 * This method returns PIDS Data information
		 * 
		 * @return DataBlock<WORD> : Raw data information
		 */
		TA_Base_Bus::DataBlock<WORD>& getPIDSData();

		/**
		 * getPasData
		 *
		 * This method returns PIDS Data information
		 * 
		 * @param platformId : Platform ID to get PA Information
		 * @return AtsPasData : AtsPasData object
		 */
		AtsPasData& getPasData(const TA_IRS_Bus::PlatformIdType platformId);

		/**
		 * getPasDataMap
		 *
		 * This method returns Pas Data information map
		 * 
		 * @return PasDataMap : PasDataMap object
		 */
		PasDataMap& getPasDataMap();

		/**
		 * powerStatusUpdated
		 *
		 * This method will update the Power Status information
		 * 
		 * @param pkey      : Entity key of the Datapoint
		 * @param dpQuality : Datapoint quality
		 * @param dpValue   : Datapoint value
		 */
		void powerStatusUpdated(unsigned long pkey,bool dpQuality,bool dpValue);

		/**
		 * IsPowerMapConfiged
		 *
		 * This will return boolean if the power map has been configured or not
		 * 
		 * @return bool   : Boolean
		 */
		bool IsPowerMapConfiged();

		/**
		 * getPowerStatus
		 *
		 * Returns the power status information
		 * 
		 * @return unsigned char*   : Power status information
		 */
		unsigned char* getPowerStatus();

		/**
		 * updateTrainPISCache
		 *
		 * Method to use to update Train PIS information
		 * 
		 * @param trainInfo : The Train PIS information
		 */
		void updateTrainPISCache(const PISInfoVec& pisInfo);

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
		 * Disable default, copy and assignment constructor
		 */
		DataCache();
		DataCache(const DataCache&);
		DataCache& operator= (const DataCache&);
		virtual ~DataCache();

		// Instance of this class
		static DataCache* m_pInstance;

		// static lock for singleton object creation and retrieval
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
		TA_Base_Core::ReEntrantThreadLockable m_powerStatLock;

		/**
		 * initializePAData
		 *
		 * Method used to initialize  PA Information
		 * 
		 * @param entityKey   : ATS Entity key
		 * @param locationKey : ATS Location key
		 */
		void initializePAData(unsigned entityKey, unsigned long locationKey);

		/**
		 * initializePowerData
		 *
		 * Method used to initialize Power Information
		 * 
		 * @param entityKey   : ATS Entity key
		 * @param locationKey : ATS Location key
		 */
		void initializePowerData(TA_Base_Core::AtsAgentEntityDataPtr entityData);

		/**
		 * setBits
		 *
		 * Method used to set bits for Power Status
		 * 
		 * @param data  : Power Status
		 * @param set   : Power status value
		 * @param start : Start bit for Power status
		 * @param bits  : Bit position
		 */
		void setBits(unsigned char* data,unsigned char set,unsigned short start,unsigned short bits);
		bool m_bPowerMapConfiged;

	protected:

		PasDataMap    m_pasDataMap;
		PowerDataMap  m_powerDataMap;
		TA_Base_Bus::DataBlock<WORD> m_PIDSData;
		unsigned char m_powerStatus[POWER_STATUS_SIZE];
		PasDataMap    m_ActivePAData;
		std::map<unsigned long,unsigned short> m_pwPKeytoSectionMap;
		DataPointHelper* m_dataPointHelper;
		TrainDataLocation* m_trainLocationData;
		TrainPisData*      m_trainPisData;

	};
}
#endif