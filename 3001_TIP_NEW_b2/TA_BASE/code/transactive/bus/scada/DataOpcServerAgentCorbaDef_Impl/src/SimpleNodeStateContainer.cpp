/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/SimpleNodeStateContainer.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/03 14:03:36 $
  * Last modified by:  $Author: jinmin.huang $
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "SimpleNode.h"
#include "ISimpleNodeAgent.h"
#include "OpcSimpleNodeAgentCorbaDef_Impl.h"
#include "SimpleNodeStateContainer.h"
#include "SimpleNodeDisplayClient.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	SimpleNodeStateContainer::SimpleNodeStateContainer ( TA_Base_Bus::ISimpleNodeAgent * specificDataNodeAgent)
	:m_specificDataNodeAgent ( 0 )
	{
		FUNCTION_ENTRY ("SimpleNodeStateContainer()" );
		m_listOfObservedDataNodes.clear();
        m_listOfSimpleNodeDisplayState.clear();

		// save the pointer to the data point agent for later usage
		m_specificDataNodeAgent = specificDataNodeAgent;

        loadAllSimpleNodes();

        FUNCTION_EXIT;
	}

	SimpleNodeStateContainer::~SimpleNodeStateContainer()
	{
		FUNCTION_ENTRY ("~SimpleNodeStateContainer()" );
        {
            TA_THREADGUARD( m_observedDataNodeListLock );
		    std::map < unsigned long, SimpleNode * >::iterator dnItr;
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
            //remove the SimpleNode observer and display list
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator itrds;
            for (itrds = m_listOfSimpleNodeDisplayState.begin(); itrds != m_listOfSimpleNodeDisplayState.end(); itrds++)
            {
                removeDataNode(itrds->first);
			    delete (itrds->second);
			    itrds->second = 0;
            }
        }

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificDataNodeAgent = 0;
        FUNCTION_EXIT;
	}

    SimpleNode * SimpleNodeStateContainer::findExistingDataNode ( unsigned long entityKey )
	{
		FUNCTION_ENTRY ("findExistingDataNode()" );
		// stop multi threads adding/removing data points while we are searching
		TA_THREADGUARD( m_observedDataNodeListLock );

		SimpleNode * dp = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, SimpleNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			dp = itr->second;
		}
		// else do nothing

		return dp;
        FUNCTION_EXIT;
	}

	void SimpleNodeStateContainer::removeDataNode ( unsigned long entityKey )
	{
		FUNCTION_ENTRY ("removeDataNode()" );
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataNodeListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, SimpleNode * >::iterator itr = m_listOfObservedDataNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedDataNodes.end() )
		{
			itr->second->deregisterForUpdateDetails ( this );
			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedDataNodes.erase ( itr );
		}
		// else do nothing
        FUNCTION_EXIT;
	}

    void SimpleNodeStateContainer::registerForUpdates( SimpleNode * dataNode )
	{
		FUNCTION_ENTRY ("registerForUpdates()" );
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedDataNodeListLock );

        unsigned long dpkey = dataNode->getEntityKey();
		// save the reference of data points to internal list
		m_listOfObservedDataNodes [ dpkey ] = dataNode;

		// register to the SimpleNode for callback of latest update details
		dataNode->registerForUpdateDetails ( this );
        FUNCTION_EXIT;
	}


	void SimpleNodeStateContainer::processSimpleNodeUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> updateDetails )
	{
		//
		// callback from one of the SimpleNode supposedly being assigned to this observer
		//
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimpleNodeStateContainer::processSimpleNodeUpdate() - EK %u", pkey);

		std::stringstream ss;
		ss << "processDataNodeUpdate: --> received an update from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		SimpleNode * existingDataNode = 0;
		existingDataNode = findExistingDataNode ( pkey );

		// if found one
		if ( 0 != existingDataNode )
		{
            TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState update;
            try
            {
				TA_Base_Bus::DataNodeDisplayState dpDisplay = existingDataNode->getDisplayState();
				
				if ( 0 != updateDetails )
				{
					convertDnDisplayUpdate(dpDisplay, update);
				}
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve SimpleNode State for DataNode key %d",
							 pkey);
			}

            saveSimpleNodeUpdateDetails ( pkey, update );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


    void SimpleNodeStateContainer::saveSimpleNodeUpdateDetails( unsigned long pkey, 
		const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState & updateDetails )
    {
		std::stringstream ss;
		ss << "saveSimpleNodeUpdateDetails: --> saving updates from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimpleNodeStateContainer::saveSimpleNodeUpdateDetails() - EK %u", pkey);
        {
		    TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * >::iterator itr;
			itr = m_listOfSimpleNodeDisplayState.find(pkey);
			if(itr != m_listOfSimpleNodeDisplayState.end())
			{
				*(itr->second) = updateDetails;
			}
        }


		// create instance of DataNodeUpdate
        boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> dpUpdate (new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState);

		// populate the new DataNodeUpdate with the info from the SimpleNode
		*dpUpdate = updateDetails;

        // notify all SimpleNodeDisplayClient this update
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        
        std::map < unsigned long, SimpleNodeDisplayClient * >::iterator itr = m_listOfDataNodeClients.end();
		for ( itr = m_listOfDataNodeClients.begin();
			  itr != m_listOfDataNodeClients.end();
			  itr++ )
		{
            // notify each client
            (*itr).second->saveDataNodeDisplayUpdateDetails(dpUpdate);
		}

		FUNCTION_EXIT;
    }

    void SimpleNodeStateContainer::addDisplayClient(unsigned long clientID, SimpleNodeDisplayClient * displayClient)
    {
		FUNCTION_ENTRY ("addDisplayClient()" );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In SimpleNodeStateContainer::addDisplayClient() - clientID %u", clientID);

		TA_Base_Core::ThreadGuard guard( m_clientListLock );

        m_listOfDataNodeClients[clientID] = displayClient;
        FUNCTION_EXIT;
    }

    void SimpleNodeStateContainer::removeDisplayClient(unsigned long clientID)
    {
		FUNCTION_ENTRY ("removeDisplayClient()" );
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        std::map < unsigned long, SimpleNodeDisplayClient * >::iterator itr;
        itr = m_listOfDataNodeClients.find(clientID);
        if (itr != m_listOfDataNodeClients.end())
        {
			//no need delete this pointer, as the DataNodeAgentCorbaDef_Impl will do it.
            m_listOfDataNodeClients.erase(itr);
        }
        FUNCTION_EXIT;
    }

    void SimpleNodeStateContainer::loadAllSimpleNodes()
    {
        FUNCTION_ENTRY("loadAllSimpleNodes");
        {
		    TA_THREADGUARD( m_observedDataNodeListLock );
            m_specificDataNodeAgent->getAllSimpleNodes(m_listOfObservedDataNodes);
        }
        
        {
		    TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
            if (m_listOfObservedDataNodes.size() > 0)
            {
                std::map< unsigned long, SimpleNode * >::iterator itr;
                for (itr = m_listOfObservedDataNodes.begin(); itr != m_listOfObservedDataNodes.end(); itr++)
                {
                    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * dnUpdate = 0;
                    dnUpdate = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState();

					// peter low: register first then only get updates
					registerForUpdates ( itr->second );

                    try
                    {
						TA_Base_Bus::DataNodeDisplayState dnDisplay = (*itr).second->getDisplayState();
						
						//if ( 0 != dnDisplay.get() )
						{
							convertDnDisplayUpdate(dnDisplay, *dnUpdate);
						}
						
                        m_listOfSimpleNodeDisplayState.insert(std::pair<unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *>(itr->first, dnUpdate));
                    }
			        catch ( ... )
			        {
				        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve SimpleNode State for SimpleNode key %d",
							         itr->first);
			        }
                }
           }
        }

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* SimpleNodeStateContainer::getAllSimpleNodeDisplayState()
    {
        FUNCTION_ENTRY("getAllSimpleNodeDisplayState");
        //todo: need to delete the new sequence or not
		TA_THREADGUARD( m_listOfDataNodeDisplayStateLock );
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence * displaySequence = 0;

		displaySequence = new TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence();
		displaySequence->length (m_listOfSimpleNodeDisplayState.size());

        if (m_listOfSimpleNodeDisplayState.size() > 0)
        {
            std::map< unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *>::iterator itr;
		    unsigned int sequenceIndex = 0;
            for (itr = m_listOfSimpleNodeDisplayState.begin(); itr != m_listOfSimpleNodeDisplayState.end(); itr++)
            {
			    // copy it to the specified output sequence
               (*displaySequence)[sequenceIndex++] = (*(itr->second));
            }
        }
        FUNCTION_EXIT;
        return displaySequence;
    }    
}
