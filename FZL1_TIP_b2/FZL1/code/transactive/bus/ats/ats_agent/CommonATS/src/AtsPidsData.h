/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/ats/AtsAgent/src/AtsPidsData.h $
  * @author:
  * @version:  $Revision: # $
  *
  * Last modification: $DateTime:  $
  * Last modified by:  $Author: $
  *
  * This represents the ATS PIDS Data
  */
#ifndef ATSPIDSDATA_H
#define ATSPIDSDATA_H

#include <time.h>
#include "core/message/src/CommsMessageSender.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Bus
{
    class AtsPidsData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsPidsData( TA_IRS_Bus::PlatformIdType platformId,unsigned long stationId,
                    unsigned long entityKey,
                    unsigned long locationKey );

		virtual ~AtsPidsData();

		TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef buildAtsPidsMessage();
		void importAtsPidsMessage(const TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef& pidsUpdate);


		void updateTimeNxtTrain(TA_IRS_Bus::TimeTTimeStampType nTimeNxtTrain);
		void updateDeptTime(TA_IRS_Bus::TimeTTimeStampType nDeptTime);
		void updateDwellTime(TA_IRS_Bus::TimeTTimeStampType nDwelltime);
		void updateDestStationId(unsigned long nDestId);
		void updateSkip(bool bSkip );

		void updateTime2NxtTrain(TA_IRS_Bus::TimeTTimeStampType time2NxtTrain);
		void update2DeptTime(TA_IRS_Bus::TimeTTimeStampType nDeptTime2);
		void update2DwellTime(TA_IRS_Bus::TimeTTimeStampType nDwelltime2);
		void update2DestStationId(unsigned long nDestId2);
		void update2Skip(bool bSkip2 );


		bool isPidsInfoUpdated();

		void processUpdates();

    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsPidsData();
        AtsPidsData( const AtsPidsData& atsPidsRecord);
        AtsPidsData& operator=(const AtsPidsData &);

		//void setValid();
	protected:

		TA_IRS_Bus::PlatformIdType						m_platformId;
		unsigned long										m_stationId;


		TA_IRS_Bus::IAtsPidsCorbaDef::PlatformInfo   	    m_platformInfo;


		bool										m_pidsInfoChanged;

		TA_Base_Core::CommsMessageSender*			m_commsMessageSender;
		unsigned long								m_entityKey;
		unsigned long								m_locationKey;		
    };
}

#endif // !defined(ATSPIDSDATA_H)
