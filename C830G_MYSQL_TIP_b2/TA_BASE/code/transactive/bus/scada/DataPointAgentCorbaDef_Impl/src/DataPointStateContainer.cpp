/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  *	Description
  *
  */

#include <sstream>


#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/StopwatchUtil.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointStateContainer.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	DataPointStateContainer::DataPointStateContainer ( TA_Base_Bus::IDataPointAgent * specificDataPointAgent)
	:
	m_purgeExistingUpdatesTimer ( NULL ),
	m_specificDataPointAgent ( NULL )
	{
		m_listOfObservedDataPoints.clear();
        m_listOfDataPointDisplayState.clear();

		// save the pointer to the data point agent for later usage
		m_specificDataPointAgent = specificDataPointAgent;

        loadAllDataPoints();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
	}


	DataPointStateContainer::~DataPointStateContainer()
	{
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

		// reset the list of observed datapoints, without deleting
		// the actual datapoints since they are not created and owned
		// by this class
        {
            TA_THREADGUARD( m_observedDataPointListLock );
		    std::map < unsigned long, DataPoint * >::iterator dpItr;
		    for ( dpItr = m_listOfObservedDataPoints.begin();
			      dpItr != m_listOfObservedDataPoints.end();
			      dpItr++ )
		    {
			    try
			    {
				    dpItr->second->deregisterForUpdateDetails ( this );
			    }
			    catch ( ... )
			    {
				    // do nothing as the data point might have been already removed
			    }
		    }
		    m_listOfObservedDataPoints.clear();
        }


        {
            TA_THREADGUARD( m_clientListLock );
            //clear the clinet list first
            m_listOfDataPointClients.clear();
        }

        {
            TA_THREADGUARD( m_listOfDataPointDisplayStateLock );
            //remove the datapoint observer and display list
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator itrds;
            for (itrds = m_listOfDataPointDisplayState.begin(); itrds != m_listOfDataPointDisplayState.end(); itrds++)
            {
                removeDataPoint(itrds->first);
			    delete (itrds->second);
			    itrds->second = NULL;
            }
        }

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificDataPointAgent = NULL;
	}

    DataPoint * DataPointStateContainer::findExistingDataPoint ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are searching
		TA_THREADGUARD( m_observedDataPointListLock );

		DataPoint * dp = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataPoint * >::iterator itr = m_listOfObservedDataPoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataPoints.end() )
		{
			dp = itr->second;
		}
		// else do nothing

		return dp;
	}

	void DataPointStateContainer::removeDataPoint ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataPointListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataPoint * >::iterator itr = m_listOfObservedDataPoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataPoints.end() )
		{
			itr->second->deregisterForUpdateDetails ( this );
			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedDataPoints.erase ( itr );
		}
		// else do nothing
	}

    void DataPointStateContainer::registerForUpdates ( TA_Base_Bus::DataPoint * datapoint )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataPointListLock );

        unsigned long dpkey = datapoint->getEntityKey();
		// save the reference of data points to internal list
		m_listOfObservedDataPoints [ dpkey ] = datapoint;

		// register to the DataPoint for callback of latest update details
		datapoint->registerForUpdateDetails ( this );
	}


	void DataPointStateContainer::processDataPointUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
	{
		//
		// callback from one of the DataPoint supposedly being assigned to this observer
		//

		std::stringstream ss;
		ss << "processDataPointUpdate: --> received an update from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );
		
		switch(updateDetails->updateType)
		{
			case	CompleteDynamicSyncStateUpdate:
			case	ValueUpdate:
			case	AlarmStateUpdate:
        	case	AlarmAckStateUpdate:
				break;
			case	AlarmPropertiesUpdate:
			case	ControlUpdate:
			case	InhibitUpdate:
			case	LastOperatorUpdate:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "no need updates type received %d",updateDetails->updateType);
				return;
			default:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown updates type received %d",updateDetails->updateType);
				return;
		}
		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		DataPoint * existingDataPoint = 0;
		existingDataPoint = findExistingDataPoint ( pkey );

		// if found one
		if ( 0 != existingDataPoint )
		{
            TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState update;
            try
            {
				boost::shared_ptr< TA_Base_Bus::DataPointDisplayState > dpDisplay = existingDataPoint->getDisplayState();
				
				if ( 0 != dpDisplay.get() )
				{
					convertDpDisplayUpdate(*dpDisplay, update);
				}
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataPoint State for DataPoint key %d",
							 pkey);
			}

            saveDataPointUpdateDetails ( pkey, update );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


    void DataPointStateContainer::saveDataPointUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState & updateDetails )
    {
		std::stringstream ss;
		ss << "saveDataPointUpdateDetails: --> saving updates from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );
        {
		    TA_THREADGUARD( m_listOfDataPointDisplayStateLock );
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator itr;
			itr = m_listOfDataPointDisplayState.find(pkey);
			if(itr != m_listOfDataPointDisplayState.end())
			{
				*(itr->second) = updateDetails;
			}
        }


		// create instance of DataPointUpdate
        boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> dpUpdate (new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState);

		// populate the new DataPointUpdate with the info from the DataPoint
		*dpUpdate = updateDetails;

        // notify all DataPointDisplayClient this update
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        
        std::map < unsigned long, DataPointDisplayClient * >::iterator itr;
		for ( itr = m_listOfDataPointClients.begin();
			  itr != m_listOfDataPointClients.end();
			  itr++ )
		{
            // notify each client
            (*itr).second->saveDataPointDisplayUpdateDetails(pkey,dpUpdate);
		}

		FUNCTION_EXIT;
    }

    void DataPointStateContainer::addDisplayClient(unsigned long clientID, DataPointDisplayClient * displayClient)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );

        m_listOfDataPointClients[clientID] = displayClient;
    }

    void DataPointStateContainer::removeDisplayClient(unsigned long clientID)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        std::map < unsigned long, DataPointDisplayClient * >::iterator itr;
        itr = m_listOfDataPointClients.find(clientID);
        if (itr != m_listOfDataPointClients.end())
        {
            m_listOfDataPointClients.erase(itr);
        }
    }

    void DataPointStateContainer::loadAllDataPoints()
    {
        FUNCTION_ENTRY("loadAllDataPoints");
        {
		    TA_THREADGUARD( m_observedDataPointListLock );
            m_specificDataPointAgent->getAllDataPoints(m_listOfObservedDataPoints);
        }
        
        {
		    TA_THREADGUARD( m_listOfDataPointDisplayStateLock );
            if (m_listOfObservedDataPoints.size() > 0)
            {
                std::map< unsigned long, DataPoint * >::iterator itr;
                for (itr = m_listOfObservedDataPoints.begin(); itr != m_listOfObservedDataPoints.end(); ++itr)
                {
                    TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * dpUpdate = 0;
                    dpUpdate = new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState();

					// peter low: register first then only get updates
					registerForUpdates ( itr->second );
                    
                    try
                    {
						boost::shared_ptr< TA_Base_Bus::DataPointDisplayState > dpDisplay = (*itr).second->getDisplayState();
						
						if ( 0 != dpDisplay.get() )
						{
							convertDpDisplayUpdate(*dpDisplay, *dpUpdate);
						}

                        m_listOfDataPointDisplayState.insert(std::pair<unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *>(itr->first, dpUpdate));
                    }
			        catch ( ... )
			        {
				        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught unknown exception while trying to retrieve DataPoint State for DataPoint key %d",
							         itr->first);
			        }
                }
           }
        }

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* DataPointStateContainer::getAllDataPointDisplayState()
    {
        FUNCTION_ENTRY("getAllDataPointDisplayState");
        //todo: need to delete the new sequence or not
		TA_THREADGUARD( m_listOfDataPointDisplayStateLock );
        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence();
		displaySequence->length ( m_listOfDataPointDisplayState.size() );

        if (m_listOfDataPointDisplayState.size() > 0)
        {
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *>::iterator itr;
		    unsigned int sequenceIndex = 0;
            for (itr = m_listOfDataPointDisplayState.begin(); itr != m_listOfDataPointDisplayState.end(); itr++)
            {
			    // copy it to the specified output sequence
               (*displaySequence)[sequenceIndex++] = (*(itr->second));
            }
        }
        FUNCTION_EXIT;
        return displaySequence;
    }    
}
