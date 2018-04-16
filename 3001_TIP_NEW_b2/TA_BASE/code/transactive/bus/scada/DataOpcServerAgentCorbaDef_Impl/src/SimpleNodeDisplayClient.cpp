/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/SimpleNodeDisplayClient.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "SimpleNodeDisplayClient.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	SimpleNodeDisplayClient::SimpleNodeDisplayClient ( unsigned long clientID )
	:
	m_purgeExistingUpdatesTimer ( 0 ),
	m_clientID ( clientID )
	{
		FUNCTION_ENTRY ("SimpleNodeDisplayClient()" );
		m_listOfUpdateDetails.clear();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
		FUNCTION_EXIT;
	}


	SimpleNodeDisplayClient::~SimpleNodeDisplayClient()
	{
		FUNCTION_ENTRY ("~SimpleNodeDisplayClient()" );
		m_listOfUpdateDetails.clear();
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}
		FUNCTION_EXIT;
	}


	unsigned long SimpleNodeDisplayClient::getClientID() const
	{
		return m_clientID;
	}

	unsigned long SimpleNodeDisplayClient::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}

    void SimpleNodeDisplayClient::saveDataNodeDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> displayUpdate)
    {
        //put it into our local cache
		std::stringstream ss;
		ss << "saveDataNodeUpdateDetails: Client ID "  << m_clientID ;
		FUNCTION_ENTRY (ss.str().c_str() );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimpleNodeDisplayClient::saveDataNodeDisplayUpdateDetails() - m_clientID %u", m_clientID);

		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );
		// save the new DataNodeDisplayState to the list of current updates
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState* dpupdate = 0;
        dpupdate = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState();
        *dpupdate = *displayUpdate;
        m_listOfUpdateDetails.push_back ( dpupdate );        
		FUNCTION_EXIT;
    }

    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* SimpleNodeDisplayClient::getCurrrentDisplayUpdates()
    {
		FUNCTION_ENTRY ("getCurrrentDisplayUpdates" );
		std::vector <TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState* > localList;
		localList.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimpleNodeDisplayClient::getCurrrentDisplayUpdates() - m_clientID %u", m_clientID);
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

        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence();
		displaySequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator currentUpdateItr;
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
