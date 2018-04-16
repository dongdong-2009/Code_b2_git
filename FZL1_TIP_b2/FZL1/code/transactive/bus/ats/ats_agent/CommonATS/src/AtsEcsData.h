/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsEcsData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSECSDATA_H
#define ATSECSDATA_H

#include <time.h>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/data_access_interface/ats/src/IAtsFixedBlockMap.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

namespace TA_IRS_Bus
{
    class AtsEcsData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsEcsData(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, unsigned long entityKey,
					   unsigned long locationKey, TA_IRS_Core::IAtsFixedBlockMap& stationZones, 
					   TA_IRS_Core::IAtsFixedBlockMap& ecsZones,
					   std::vector<unsigned long>& stationsInThisSector);
		virtual ~AtsEcsData();

		TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef buildAtsEcsMessage();
		void importAtsEcsMessage(const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& ecsMessage);

		void updateAtcTrainNumber(TA_IRS_Bus::AtcTrainNumberType atcTrainNumber);

		// Train Validity
		void clearImportFlag();
		void setTrainValidity();
		void checkIfTrainNotImported();

		// Localisation Data
		void updateLocalisationValidity(bool valid);
		void updateServiceNumber(TA_IRS_Bus::ServiceNumberType serviceNumber);
		void updateScheduleNumber(TA_IRS_Bus::ScheduleNumberType scheduleNumber);
		void updateZoneId(unsigned long fixedBlocksBranchId,
						  unsigned long fbAbscissaInTheBranch,
						  unsigned long	headCarAbscissaOnFb);

		// OA1 Data
		void updateOa1Validity(bool valid);
		void updateEmergency(bool emergency);
		void updateExternalTemperatureInCelsius(unsigned long externalTemperature);
		void updateStalled(bool stalled);

		bool isUpdated();
		bool isValid();
		void sendUpdates();

	protected:
	
		void updateZoneId(unsigned long stationZoneId, unsigned long ecsZoneId);

		// The stallTimeInSeconds attribute will be set based on the 
		// <Stalled in inter-station> field.  The ATS Agent will be responsible
		// for keeping track of stalled trains and how long they are stalled
		// for.
		void updateStallTime();
		void updateStallTime(unsigned long stallTime);
		void updateTrainValidity(bool valid);

		bool inThisSector(unsigned long stationId);
	
    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsEcsData();
        AtsEcsData( const AtsEcsData& trainRecord);
        AtsEcsData& operator=(const AtsEcsData &);

	protected:

		TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef	m_atsEcsMessage;

		bool								m_recordValid;
		
		//
		// Stalled: Used for ECS
		// - Stalled status: Yes | No
		//
		bool								m_recordImported;
		bool								m_stalled;					// <Stalled in inter-station>
		time_t								m_stallTime;				// Maintained by ATS Agent
		
		bool								m_ecsInfoChanged;

		TA_Base_Core::CommsMessageSender*	m_commsMessageSender;
		unsigned long						m_entityKey;
		unsigned long						m_locationKey;

		TA_IRS_Core::IAtsFixedBlockMap&		m_stationZones;
		TA_IRS_Core::IAtsFixedBlockMap&		m_ecsZones;

		std::vector<unsigned long>			m_stationsInThisSector;
    };
}

#endif // !defined(ATSECSDATA_H)
