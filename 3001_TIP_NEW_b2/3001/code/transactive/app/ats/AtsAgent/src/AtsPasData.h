/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/AtsPasData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/08/15 17:15:12 $
  * Last mofified by:  $Author: peter.wong $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSPASDATA_H
#define ATSPASDATA_H

#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/AtsTisData.h"

namespace TA_IRS_App
{
    class AtsPasData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsPasData(TA_IRS_Bus::PlatformIdType platformId, unsigned long stationId, unsigned long entityKey, 
						  unsigned long locationKey);

		virtual ~AtsPasData();

		TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::EEventType event);
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> buildAtsPasMessageVector();
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> buildAtsPasMessageState();
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

		void updateDestinationStationId(TA_IRS_Bus::StationIdType destinationStationId);
		void updateLastTrainOfOperatingDay(bool lastTrainOfOperatingDay);
		void updateTrainNotBoard(bool trainNotBoard);

		TA_IRS_Bus::PlatformIdType getPlatformId();
		
		void processUpdates();

		std::string eEventTypeToString(TA_IRS_Bus::IAtsPasCorbaDef::EEventType eventType);
    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsPasData();
        AtsPasData( const AtsPasData& atsPlatformRecord);
        AtsPasData& operator=(const AtsPasData &);

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
		bool                                m_trainNotBoard;        // <TrainNotBoard> //Lucky : Newly added event type
		
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::EEventType> 
											m_pasEvents;

		TA_Base_Core::CommsMessageSender*		m_commsMessageSender;
		unsigned long						m_entityKey;
		unsigned long						m_locationKey;
    };
}

#endif // !defined(ATSPASDATA_H)
