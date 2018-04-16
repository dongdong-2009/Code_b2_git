/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsPasData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSPASDATA_H
#define ATSPASDATA_H

#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "bus/ats/ats_agent/CommonATS/src/AtsTisData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Bus
{
    class AtsPasData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsPasData(TA_IRS_Bus::PlatformIdType platformId, unsigned long stationId, unsigned long entityKey, 
						  unsigned long locationKey,unsigned char  tisPlatformId);

		virtual ~AtsPasData();

		TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::EEventType event);
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> buildAtsPasMessageVector();
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> buildAtsPasMessageState();
		
		/**
		  * Resets the ATSPasMessage flags based on message type.
		  * This function is only used in station SIG Agent,
		  * before updating with ATSPasMessage from OCC.
		  */
		void resetAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef pasUpdate);
		void importAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef pasUpdate);

		void updatePreArrival(bool preArrival);
		void updateArrival(bool arrival);
		void updatePreDeparture(bool preDeparture);
		void updateDeparture(bool departure);
		void updateSkip(bool skip);
		void updateHold(bool hold);
		void updateEvacuation(bool evacuation);
		void updateTrainOverrun(bool trainOverrun);

		void updateTrainInfoValidity(bool valid);
		void updateInDepot(bool inDepot); //New Event type

		void updateDestinationStationId(TA_IRS_Bus::StationIdType destinationStationId);
		void updateLastTrainOfOperatingDay(bool lastTrainOfOperatingDay);
		void updateTisPlatformId(TA_IRS_Bus::EDirection platformDirection);

		TA_IRS_Bus::PlatformIdType getPlatformId();
		
		void processUpdates();

		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>& getPasMessageVector();
		void clearPasMessageVector();

    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsPasData();
        AtsPasData( const AtsPasData& atsPlatformRecord);
        AtsPasData& operator=(const AtsPasData &);
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> m_pasMessageVector;

	protected:

		// AtsPasMessage
		TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef
											m_atsPasMessage;

		//
		// Platform's train events: used for TIS & PA
		// -	Pre-arrival: Yes | No,
		// -	Arrival: Yes | No,
		// -	Pre-departure : Yes | No,
		// -	Departure: Yes | No,
		// -	Skip: Yes | No,
		// -	Hold: Yes | No,
		// -	Evacuation: Yes | No.
		// -	Train overrun: Yes | No
		//
		bool								m_preArrival;			// <Pre-arrival> 
		bool								m_arrival;				// <Arrival>
		bool								m_preDeparture;			// <Pre-departure>
		bool								m_departure;			// <Departure>
		bool								m_skip;					// <Skip>
		bool								m_hold;					// <Hold>
		bool								m_evacuation;			// <Evacuation>
		bool								m_trainOverrun;			// <Train overrun>
		bool                                m_inDepot;              // <In Depot>
		
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::EEventType> 
											m_pasEvents;

		TA_Base_Core::CommsMessageSender*		m_commsMessageSender;
		unsigned long						m_entityKey;
		unsigned long						m_locationKey;
    };
}

#endif // !defined(ATSPASDATA_H)
