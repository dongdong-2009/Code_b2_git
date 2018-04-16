/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/SimplePointDisplayClient.cpp $
  * @author  HoaVu
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2012/06/12 17:13:30 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"

#include "SimplePoint.h"
#include "OpcSimplePointAgentCorbaDef_Impl.h"
#include "SimplePointDisplayClient.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	SimplePointDisplayClient::SimplePointDisplayClient ( unsigned long clientID )
	:
	m_clientID ( clientID ),
	m_purgeExistingUpdatesTimer ( 0 )
	{
		FUNCTION_ENTRY ("SimplePointDisplayClient()");
		m_listOfUpdateDetails.clear();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
		FUNCTION_EXIT;
	}


	SimplePointDisplayClient::~SimplePointDisplayClient()
	{
		FUNCTION_ENTRY ("~SimplePointDisplayClient()");
		m_listOfUpdateDetails.clear();
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}
		FUNCTION_EXIT;
	}


	unsigned long SimplePointDisplayClient::getClientID() const
	{
		return m_clientID;
	}

	unsigned long SimplePointDisplayClient::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}

    void SimplePointDisplayClient::saveSimplePointDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> displayUpdate)
    {
        //put it into our local cache
		std::stringstream ss;
		ss << "saveSimplePointDisplayUpdateDetails: Client ID "  << m_clientID ;
		FUNCTION_ENTRY (ss.str().c_str() );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimplePointDisplayClient::saveSimplePointDisplayUpdateDetails() - m_clientID %u", m_clientID);

		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );
		// save the new SimplePointDisplayState to the list of current updates
        TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* dpupdate = 0;
        dpupdate = new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState();
        *dpupdate = *displayUpdate;
        m_listOfUpdateDetails.push_back ( dpupdate );        
		FUNCTION_EXIT;
    }

    TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* SimplePointDisplayClient::getCurrrentDisplayUpdates()
    {
		FUNCTION_ENTRY ("getCurrrentDisplayUpdates" );
		std::vector <TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* > localList;
		localList.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimplePointDisplayClient::getCurrrentDisplayUpdates() - m_clientID %u", m_clientID);

		{
			// stop multi threads accessing the update details list
			TA_THREADGUARD( m_updateDetailsLock );

			if ( 0 < m_listOfUpdateDetails.size() )
			{
				// clear the update list
                localList.swap( m_listOfUpdateDetails );
			}
			// else do nothing
		}

        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence();
		displaySequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator currentUpdateItr;
		for ( currentUpdateItr = localList.begin();
			  currentUpdateItr != localList.end();
			  currentUpdateItr++ )
		{
			// copy it to the specified output sequence
			(*displaySequence)[sequenceIndex++] = **currentUpdateItr;

			// delete the update details from the list
			delete (*currentUpdateItr);
			(*currentUpdateItr) = 0;
		}

		// restart the timer
		m_purgeExistingUpdatesTimer->startTiming();

		localList.clear();
		FUNCTION_EXIT;

		return displaySequence;    
    }

}
