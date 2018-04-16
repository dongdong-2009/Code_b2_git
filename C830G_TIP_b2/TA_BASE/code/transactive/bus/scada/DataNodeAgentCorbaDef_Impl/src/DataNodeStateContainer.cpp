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
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeStateContainer.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeDisplayClient.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	DataNodeStateContainer::DataNodeStateContainer ( TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent)
	:
	m_purgeExistingUpdatesTimer ( 0 ),
	m_specificDataNodeAgent ( 0 )
	{
		m_listOfObservedDataNodes.clear();
        m_listOfDataNodeDisplayState.clear();

		// save the pointer to the data point agent for later usage
		m_specificDataNodeAgent = specificDataNodeAgent;

        loadAllDataNodes();

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();
	}


	DataNodeStateContainer::~DataNodeStateContainer()
	{
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

		// reset the list of observed DataNodes, without deleting
		// the actual DataNodes since they are not created and owned
		// by this class
        {
            TA_THREADGUARD( m_observedDataNodeListLock );
		    std::map < unsigned long, DataNode * >::iterator dnItr;
		    for ( dnItr = m_listOfObservedDataNodes.begin();
			      dnItr != m_listOfObservedDataNodes.end();
			      dnItr++ )
		    {
			    try
			    {
				    dnItr->second->deregisterForUpdateDetails ( this );
			    }
			    catch ( ... )
			    {
				    // do nothing as the data point might have been already removed
			    }
		    }
		    m_listOfObservedDataNodes.clear();
        }


        {
            TA_THREADGUARD( m_clientListLock );
            //clear the clinet list first
            m_listOfDataNodeClients.clear();
        }

        {
            TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
            //remove the DataNode observer and display list
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator itrds;
            for (itrds = m_listOfDataNodeDisplayState.begin(); itrds != m_listOfDataNodeDisplayState.end(); itrds++)
            {
                removeDataNode(itrds->first);
			    delete (itrds->second);
			    itrds->second = 0;
            }
        }

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificDataNodeAgent = 0;
	}

    DataNode * DataNodeStateContainer::findExistingDataNode ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are searching
		TA_THREADGUARD( m_observedDataNodeListLock );

		DataNode * dp = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			dp = itr->second;
		}
		// else do nothing

		return dp;
	}

	void DataNodeStateContainer::removeDataNode ( unsigned long entityKey )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataNodeListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, DataNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			itr->second->deregisterForUpdateDetails ( this );
			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedDataNodes.erase ( itr );
		}
		// else do nothing
	}

    void DataNodeStateContainer::registerForUpdates( TA_Base_Bus::DataNode * dataNode )
	{
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataNodeListLock );

        unsigned long dpkey = dataNode->getEntityKey();
		// save the reference of data points to internal list
		m_listOfObservedDataNodes [ dpkey ] = dataNode;

		// register to the DataNode for callback of latest update details
		dataNode->registerForUpdateDetails ( this );
	}


	void DataNodeStateContainer::processDataNodeUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails )
	{
		//
		// callback from one of the DataNode supposedly being assigned to this observer
		//

		std::stringstream ss;
		ss << "processDataNodeUpdate: --> received an update from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		switch(updateDetails->updateType)
		{
			case 	DnCompleteSyncState:
			case	DnTagStateUpdate:
			case	DnAlarmSummaryUpdate:
			case	DnAlarmAckSummaryUpdate:
			case	DnScanInhibitSummaryUpdate:
			case	DnAlarmInhibitSummaryUpdate:
			case	DnMmsAlarmInhibitSummaryUpdate:
			case	DnControlInhibitSummaryUpdate:
			case	DnOverrideSummaryUpdate:
			case	DnQualitySummaryUpdate:
			case	DnControlPendingSummaryUpdate:
			case	DnNotesUpdate:
			case	DnInhibitMMSUpdate:
				break;
			case	DnLastOperatorUpdate:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "no need updates type received %d",updateDetails->updateType);
				return;
			default:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown updates type received %d",updateDetails->updateType);
				return;
		}
		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		DataNode * existingDataNode = 0;
		existingDataNode = findExistingDataNode ( pkey );

		// if found one
		if ( 0 != existingDataNode )
		{
            TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState update;
            try
            {
				boost::shared_ptr< TA_Base_Bus::DataNodeDisplayState > dpDisplay = existingDataNode->getDisplayState();
				
				if ( 0 != dpDisplay.get() )
				{
					convertDnDisplayUpdate(*dpDisplay, update);
				}
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataNode State for DataNode key %d",
							 pkey);
			}

            saveDataNodeUpdateDetails ( pkey, update );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


    void DataNodeStateContainer::saveDataNodeUpdateDetails( unsigned long pkey, 
		const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState & updateDetails )
    {
		std::stringstream ss;
		ss << "saveDataNodeUpdateDetails: --> saving updates from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );
        {
		    TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator itr;
			itr = m_listOfDataNodeDisplayState.find(pkey);
			if(itr != m_listOfDataNodeDisplayState.end())
			{
				*(itr->second) = updateDetails;
			}
        }


		// create instance of DataNodeUpdate
        boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> dpUpdate (new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState);

		// populate the new DataNodeUpdate with the info from the DataNode
		*dpUpdate = updateDetails;

        // notify all DataNodeDisplayClient this update
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        
        std::map < unsigned long, DataNodeDisplayClient * >::iterator itr;
		for ( itr = m_listOfDataNodeClients.begin();
			  itr != m_listOfDataNodeClients.end();
			  itr++ )
		{
            // notify each client
            (*itr).second->saveDataNodeDisplayUpdateDetails(pkey,dpUpdate);
		}

		FUNCTION_EXIT;
    }

    void DataNodeStateContainer::addDisplayClient(unsigned long clientID, DataNodeDisplayClient * displayClient)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );

        m_listOfDataNodeClients[clientID] = displayClient;
    }

    void DataNodeStateContainer::removeDisplayClient(unsigned long clientID)
    {
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        std::map < unsigned long, DataNodeDisplayClient * >::iterator itr;
        itr = m_listOfDataNodeClients.find(clientID);
        if (itr != m_listOfDataNodeClients.end())
        {
			//no need delete this pointer, as the DataNodeAgentCorbaDef_Impl will do it.
            m_listOfDataNodeClients.erase(itr);
        }
    }

    void DataNodeStateContainer::loadAllDataNodes()
    {
        FUNCTION_ENTRY("loadAllDataNodes");
        {
		    TA_THREADGUARD( m_observedDataNodeListLock );
            m_specificDataNodeAgent->getAllDataNodes(m_listOfObservedDataNodes);
        }
        
        {
		    TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
            if (m_listOfObservedDataNodes.size() > 0)
            {
                std::map< unsigned long, DataNode * >::iterator itr;
                for (itr = m_listOfObservedDataNodes.begin(); itr != m_listOfObservedDataNodes.end(); itr++)
                {
                    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * dnUpdate = 0;
                    dnUpdate = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState();

					// peter low: register first then only get updates
					registerForUpdates ( itr->second );

                    try
                    {
						boost::shared_ptr< TA_Base_Bus::DataNodeDisplayState > dnDisplay = (*itr).second->getDisplayState();
						
						if ( 0 != dnDisplay.get() )
						{
							convertDnDisplayUpdate(*dnDisplay, *dnUpdate);
						}
						
                        m_listOfDataNodeDisplayState.insert(std::pair<unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *>(itr->first, dnUpdate));
                    }
			        catch ( ... )
			        {
				        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve DataNode State for DataNode key %d",
							         itr->first);
			        }
                }
           }
        }

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* DataNodeStateContainer::getAllDataNodeDisplayState()
    {
        FUNCTION_ENTRY("getAllDataNodeDisplayState");
        //todo: need to delete the new sequence or not
		TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence * displaySequence = 0;

		displaySequence = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence();
		displaySequence->length (m_listOfDataNodeDisplayState.size());

        if (m_listOfDataNodeDisplayState.size() > 0)
        {
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *>::iterator itr;
		    unsigned int sequenceIndex = 0;
            for (itr = m_listOfDataNodeDisplayState.begin(); itr != m_listOfDataNodeDisplayState.end(); itr++)
            {
			    // copy it to the specified output sequence
               (*displaySequence)[sequenceIndex++] = (*(itr->second));
            }
        }
        FUNCTION_EXIT;
        return displaySequence;
    }    
}
