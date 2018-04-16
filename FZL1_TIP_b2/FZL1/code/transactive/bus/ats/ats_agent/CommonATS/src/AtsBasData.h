/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/ats/AtsAgent/src/AtsBasData.h $
  * @author:
  * @version:  $Revision: # $
  *
  * Last modification: $DateTime:  $
  * Last modified by:  $Author: $
  *
  * This represents the ATS BAS Data
  */
#ifndef ATSBASDATA_H
#define ATSBASDATA_H

#include <time.h>
#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Bus
{
    class AtsBasData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsBasData( TA_IRS_Bus::PhysicalTrainNumberType trainId,
                    unsigned long entityKey,
                    unsigned long locationKey );

		virtual ~AtsBasData();

		TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef buildAtsBasMessage();
		void importAtsBasMessage(const TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef& basUpdate);


		void updateCongestionMode(bool isInCongestion);
		void updateStationId(unsigned long currentStationId);

		bool isBasInfoUpdated();

		void processUpdates();



    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsBasData();
        AtsBasData( const AtsBasData& atsBasRecord);
        AtsBasData& operator=(const AtsBasData &);

		//void setValid();
	protected:

		TA_IRS_Bus::PhysicalTrainNumberType					m_trainId;

		TA_IRS_Bus::IAtsBasCorbaDef::CongestionInfo   	    m_CongestionInfo;


		bool										m_BasInfoChanged;

		TA_Base_Core::CommsMessageSender*			m_commsMessageSender;
		unsigned long								m_entityKey;
		unsigned long								m_locationKey;
	
    };
}

#endif // !defined(ATSBASDATA_H)
