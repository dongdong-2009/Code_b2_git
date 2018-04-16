/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsTisData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS TIS Data
  */
#ifndef ATSTISDATA_H
#define ATSTISDATA_H

#include <time.h>
#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

namespace TA_IRS_App
{
    class AtsTisData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsTisData( TA_IRS_Bus::PlatformIdType platformId,
                    unsigned long stationId,
                    TA_IRS_Bus::TisPlatformIdType tisPlatformId,
                    unsigned long entityKey,
                    unsigned long locationKey );

		virtual ~AtsTisData();

		TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef buildAtsTisMessage(bool platformInfo);
		void importAtsTisMessage(const TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef& tisUpdate);

		void updatePreArrival(bool preArrival);
		void updateArrival(bool arrival);
		void updatePreDeparture(bool preDeparture);
		void updateDeparture(bool departure);
		void updateSkip(bool skip);
		void updateHold(bool hold);
		void updateEvacuation(bool evacuation);
		void updateTrainOverrun(bool trainOverrun);

		void updateTrainInfoValidity(unsigned int trainIndex, bool valid);
		void updatePlatformType(unsigned int trainIndex, bool arrival);
		void updatePhysicalTrainNumber(unsigned int trainIndex, TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);
		void updateServiceNumber(unsigned int trainIndex, TA_IRS_Bus::ServiceNumberType serviceNumber);
		void updateDestinationStationId(unsigned int trainIndex, TA_IRS_Bus::StationIdType destinationStationId);
		void updateLastTrainOfOperatingDay(unsigned int trainIndex, bool lastTrainOfOperatingDay);
		void updateScheduledArrival(unsigned int trainIndex, TA_IRS_Bus::TimeStampType scheduledArrival);
	
		bool isPlatformUpdated();
		bool isTrainUpdated();

		void processUpdates();

		//TD 16335
		//zhou yuan++
		void invalidPlatformData();
		//++zhou yuan

	protected:
		bool getStationType();

    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsTisData();
        AtsTisData( const AtsTisData& atsPlatformRecord);
        AtsTisData& operator=(const AtsTisData &);

		void setValid();
	protected:

		//
		// Platform Area ID
		//
		TA_IRS_Bus::PlatformIdType					m_platformId;			// <Platform area ID>
		TA_IRS_Bus::StationIdType					m_stationId;
        TA_IRS_Bus::TisPlatformIdType				m_tisPlatformId;

		TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo	m_platformInfo;
		bool										m_platformInfoChanged;

		TA_IRS_Bus::IAtsTisCorbaDef::TrainInfo		m_trainInfo[2];
		bool										m_trainInfoChanged;

		TA_Base_Core::CommsMessageSender*			m_commsMessageSender;
		unsigned long								m_entityKey;
		unsigned long								m_locationKey;
		time_t										m_lastTimePlatformInfoSend;		//CL20557, ATS Agent needs to be modify for platform TIS message. hongzhi
    };
}

#endif // !defined(ATSTISDATA_H)
