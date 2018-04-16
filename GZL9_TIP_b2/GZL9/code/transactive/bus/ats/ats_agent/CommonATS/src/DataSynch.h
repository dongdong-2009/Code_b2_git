/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling the activation and deactivation of Corba Servant.
  * It act as a base class for all ATS Data that needs synchronization.
  * E.g. ( PASycn, TrainSyc, PIDSync and etc. )
  */

#ifndef _DATA_SYNCH_H_
#define	_DATA_SYNCH_H_

#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/OccUpdateProcessor.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataSync.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	class OccUpdateProcessor;

	class DataSynch : public virtual TA_Base_Core::ServantBase,
					  public TA_IRS_Bus::IDataSync
	{
	public:
		/**
		 * Constructor
		 */
		DataSynch(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		/**
		 * Destructor
		 */
		virtual ~DataSynch();

		/**
		 * setDataUpdates
		 *
		 * This method sets the data update interface
		 *
		 * @param dataUpdate : Data Update object
		 */
		void setDataUpdates(IDataUpdate* dataUpdate);

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		DataSynch(const DataSynch&);
		DataSynch& operator= (const DataSynch&);

		/**
		 * initialize
		 *
		 * This method initializes the corba servant activation
		 */
		void initialize();

		/**
		 * deInitialize
		 *
		 * This method deinitializes the corba servant activation
		 */
		void deInitialize();

	protected:
		TA_Base_Core::AtsAgentEntityDataPtr m_entityData;
		OccUpdateProcessor* m_occUpdateProcessor;
		OccUpdateProcessor::AtsAgentNamedObject* m_occAtsAgent;
		IDataUpdate* m_dataUpdate;
	};
}
#endif