/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\OccUpdateProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling the data synchronization from OCC to Stations
  */

#ifndef _OCC_UPDATE_PROCESSOR_H_
#define _OCC_UPDATE_PROCESSOR_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/naming/src/NamedObject.h"

namespace TA_IRS_Bus
{
	class StationOccUpdateProcessor;

	class OccUpdateProcessor
	{
	public:
		typedef std::map<unsigned long, StationOccUpdateProcessor*> StationProcessorMap;
		typedef TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef PasMessage;
		typedef std::vector<PasMessage> PasMessageVector;

		typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IAtsPasCorbaDef,
			TA_IRS_Bus::IAtsPasCorbaDef_ptr,
			TA_IRS_Bus::IAtsPasCorbaDef_var> AtsAgentNamedObject;

		struct OccPasUpdates
		{
			PasMessageVector			messages;
			TA_Base_Core::ReEntrantThreadLockable lock;
		};

		/**
		 * Constructor
		 */
		OccUpdateProcessor(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType);
		/**
		 * Destructor
		 */
		virtual ~OccUpdateProcessor();

		/**
		 * addPasUpdate
		 *
		 * This method adds the PA data Information for PA Synchronization
		 * 
		 * @param pasMessage : PasMessage 
		 */
		void addPasUpdate(const PasMessage& pasMessage);

		/**
		 * sendOccUpdates
		 *
		 * This method sends Occ Updates to stations
		 * 
		 * @param pasMessage : PasMessage 
		 */
		void sendOccUpdates();

	private:
		// Disable default constructor, copy constructor and assignment operator
		OccUpdateProcessor();
		OccUpdateProcessor( const OccUpdateProcessor& occUpdateProcessor);
		OccUpdateProcessor& operator=(const OccUpdateProcessor &);

		unsigned long m_locationKey;
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType m_locationType;

		/**
		 * initialize
		 *
		 * This method initializes all SIG Station object.
		 * 
		 */
		void initialize();

		/**
		 * initialize
		 *
		 * This method deinitialize SIG Station object
		 */
		void deInitialize();

	protected:
		
		StationProcessorMap m_stationProcessorMap;
	};
}
#endif