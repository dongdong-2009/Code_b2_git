/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  HoaVu
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataPointDisplayClient::DataPointDisplayClient ( unsigned long clientID )
	:
	m_purgeExistingUpdatesTimer ( 0 ),
	m_clientID ( clientID )
	{
		m_mapOfUpdateDetails.clear();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
	}


	DataPointDisplayClient::~DataPointDisplayClient()
	{
		m_mapOfUpdateDetails.clear();
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}
	}


	unsigned long DataPointDisplayClient::getClientID() const
	{
		return m_clientID;
	}

	unsigned long DataPointDisplayClient::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}

    void DataPointDisplayClient::saveDataPointDisplayUpdateDetails(unsigned long key,boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> displayUpdate)
    {
        //put it into our local cache
		std::stringstream ss;
		ss << "saveDataPointDisplayUpdateDetails: Client ID "  << m_clientID ;
		FUNCTION_ENTRY (ss.str().c_str() );

		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );
		// save the new DataPointDisplayState to the list of current updates
        m_mapOfUpdateDetails[key]=displayUpdate;        
    }

    TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* DataPointDisplayClient::getCurrrentDisplayUpdates()
    {

		FUNCTION_ENTRY ("getCurrrentDisplayUpdates" );
		DataPointDisplayStateMap localMap;
		localMap.clear();

		{
			// stop multi threads accessing the update details list
			TA_THREADGUARD( m_updateDetailsLock );

			if ( 0 < m_mapOfUpdateDetails.size() )
			{
				// clear the update list
                localMap.swap( m_mapOfUpdateDetails );
			}
			// else do nothing
		}

        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence();
		displaySequence->length ( localMap.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		DataPointDisplayStateMap::iterator currentUpdateItr;
		for ( currentUpdateItr = localMap.begin();
			  currentUpdateItr != localMap.end();
			  currentUpdateItr++ )
		{
			// copy it to the specified output sequence
			(*displaySequence)[sequenceIndex++] = *(currentUpdateItr->second);

		}

		// restart the timer
		m_purgeExistingUpdatesTimer->startTiming();

		localMap.clear();

		return displaySequence;    
    }

}
