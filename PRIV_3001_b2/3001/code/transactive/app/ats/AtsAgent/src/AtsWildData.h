/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsWildData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSWILDDATA_H
#define ATSWILDDATA_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/RawTable.h"

namespace TA_IRS_App
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
