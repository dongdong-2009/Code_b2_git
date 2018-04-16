/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsWildData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSWILDDATA_H
#define ATSWILDDATA_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"


namespace TA_IRS_Bus
{
    class AtsWildData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsWildData(unsigned long entityKey, unsigned long locationKey, unsigned int recordNumber);

		virtual ~AtsWildData();

		TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef buildAtsWildMessage();

		void updateTimestamp(TA_IRS_Bus::TimeStampType timeStamp);
		void updatePhysicalTrainNumber(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);
		void updateActiveCabNumber(TA_IRS_Bus::IAtsWildCorbaDef::ActiveCabNumberType activeCabNumber,TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);
		void updateHeadCabDirection(unsigned char headCabDirectionByte);

		void sendUpdates();

	protected:
		
		TA_IRS_Bus::TimeTTimeStampType getTimeTTimestamp();
	
    private:
        // Disable default constructor, copy constructor and assignment operator
        AtsWildData();
		AtsWildData( const AtsWildData& AtsWildData);
        AtsWildData& operator=(const AtsWildData &);

	protected:

		// AtsWildMessage
		TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef
											m_atsWildMessage;

		unsigned int						m_recordNumber;

		bool								m_wildInfoChanged;

		TA_Base_Core::CommsMessageSender*		m_commsMessageSender;
		unsigned long						m_entityKey;
		unsigned long						m_locationKey;
    };
}

#endif // !defined(ATSWILDDATA_H)
