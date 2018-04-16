/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/SimpleNodeObserver.cpp $
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

#include "SimpleNodeObserver.h"
#include "ISimpleNodeAgent.h"
#include "SimpleNode.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"


using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	SimpleNodeObserver::SimpleNodeObserver ( TA_Base_Bus::ISimpleNodeAgent * specificSimpleNodeAgent, unsigned long observerID )
	:
	TA_Base_Core::QueueProcessor< DnObserverUpdate > (),
	m_observerID ( observerID ),
	m_purgeExistingUpdatesTimer ( NULL ),
	m_specificSimpleNodeAgent( NULL )
	{
		FUNCTION_ENTRY ("SimpleNodeObserver()" );
		m_listOfObservedSimpleNodes.clear();
		m_listOfUpdateDetails.clear();

		// save the pointer to the data node agent for later usage
		m_specificSimpleNodeAgent = specificSimpleNodeAgent;

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();

		//TD13275
		start();
		FUNCTION_EXIT;
	}


	SimpleNodeObserver::~SimpleNodeObserver()
	{
		FUNCTION_ENTRY ("~SimpleNodeObserver()" );
		//TD13275
		terminateAndWait();
		
		if ( 0 != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = 0;
		}

		// reset the list of observed SimpleNodes, without deleting
		// the actual SimpleNodes since they are not created and owned
		// by this class

		std::map < unsigned long, SimpleNode * >::iterator dpItr;
		for ( dpItr = m_listOfObservedSimpleNodes.begin();
			  dpItr != m_listOfObservedSimpleNodes.end();
			  dpItr++ )
		{
			try
			{
				dpItr->second->deregisterForUpdateDetails ( this );
			}
			catch ( ... )
			{
				// do nothing as the data node might have been already removed
			}
		}
		m_listOfObservedSimpleNodes.clear();

		// remove existing TA_Base_Bus::SimpleNodeCorbaTypes::SimpleNodeUpdate
		std::vector < TA_Base_Bus::DataNodeDisplayState * >::iterator itr;
		for ( itr = m_listOfUpdateDetails.begin();
			  itr != m_listOfUpdateDetails.end();
			  itr++ )
		{
			delete (*itr);
			(*itr) = 0;
		}
		m_listOfUpdateDetails.clear();

		// clear the reference to the data node agent, without actually deleting
		// the object
		m_specificSimpleNodeAgent = 0;
		FUNCTION_EXIT;
	}


	unsigned long SimpleNodeObserver::getObserverID() const
	{
		return m_observerID;
	}


	SimpleNode * SimpleNodeObserver::findExistingSimpleNode ( unsigned long entityKey )
	{
		FUNCTION_ENTRY ("findExistingSimpleNode()" );
		// stop multi threads adding/removing data nodes while we are searching
		TA_THREADGUARD( m_observedSimpleNodeListLock );

		SimpleNode * dn = 0;

		// find if the specified entity key matches with the pkey of one of the existing
		// data nodes in the list
		std::map < unsigned long, SimpleNode * >::iterator itr = m_listOfObservedSimpleNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedSimpleNodes.end() )
		{
			dn = itr->second;
		}
		// else do nothing
		FUNCTION_EXIT;
		return dn;
	}


	void SimpleNodeObserver::removeSimpleNode ( unsigned long entityKey )
	{
		FUNCTION_ENTRY ("removeSimpleNode()" );
		// stop multi threads adding/removing data nodes while we are modifying
		TA_THREADGUARD( m_observedSimpleNodeListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data nodes in the list
		std::map < unsigned long, SimpleNode * >::iterator itr = m_listOfObservedSimpleNodes.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedSimpleNodes.end() )
		{
			// tell the SimpleNode we are not interest in receiving callback of
			// new update details
			itr->second->deregisterForUpdateDetails ( this );

			// remove the iterator pointing to the data node, without deleting
			// the data node itself since it is not created and managed by this class
			m_listOfObservedSimpleNodes.erase ( itr );
		}
		FUNCTION_EXIT;
		// else do nothing
	}


	void SimpleNodeObserver::addSimpleNodesToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys )
	{
		FUNCTION_ENTRY ("addSimpleNodesToObserver()" );
		if (entityKeys.length() > 0)
		{
			boost::shared_ptr<DnObserverUpdate> dnKeys (new DnObserverUpdate);

			// for each of the entity key specified in the key sequence
			for (unsigned int i = 0; i < entityKeys.length(); i++)
			{
				dnKeys->keys.push_back(entityKeys[i]);
			}
			// else do nothing if already in the list
		
			insert(dnKeys);
		}
		FUNCTION_EXIT;
	}


	void SimpleNodeObserver::removeSimpleNodesFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence & entityKeys )
	{
		FUNCTION_ENTRY ("removeSimpleNodesFromObserver()" );
		// for each of the entity key specified in the key sequence
		for (unsigned int i = 0; i < entityKeys.length(); i++)
		{
			// remove the data node whose pkey matches with pkey specified in the sequence
			removeSimpleNode ( entityKeys[i] );
		}
		FUNCTION_EXIT;
	}

	TA_Base_Bus::DataNodeDisplayState*  SimpleNodeObserver::getUpdates()
	{
//		activateDataNodes();
		FUNCTION_ENTRY ("getUpdates()" );

		std::vector < TA_Base_Bus::DataNodeDisplayState * > localList;
		localList.clear();
		{
			// stop multi threads accessing the update details list
			TA_THREADGUARD( m_updateDetailsLock );

			if ( 0 < m_listOfUpdateDetails.size() )
			{
				// clear the update list
                localList.swap( m_listOfUpdateDetails );
			}
		}

		TA_Base_Bus::DataNodeDisplayState* updateSequence = NULL;
		//updateSequence->length ( localList.size() );

		// for each of the update details in the local list,
		unsigned int sequenceIndex = 0;
		std::vector < TA_Base_Bus::DataNodeDisplayState * >::iterator currentUpdateItr;
		for ( currentUpdateItr = localList.begin();
			  currentUpdateItr != localList.end();
			  currentUpdateItr++ )
		{
			// copy it to the specified output sequence
			//(*updateSequence)[sequenceIndex++] = **currentUpdateItr;

			// delete the update details from the list
			delete (*currentUpdateItr);
			(*currentUpdateItr) = 0;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Observer ID %lu : has %d updates in the returned sequence",
            m_observerID, sequenceIndex );

		// restart the timer
		//m_purgeExistingUpdatesTimer->startTiming();

		localList.clear();
		FUNCTION_EXIT;
		return updateSequence;
	}

	void SimpleNodeObserver::processEvent( boost::shared_ptr<DnObserverUpdate> localList )
	{
		FUNCTION_ENTRY ("processEvent()" );
        std::stringstream debugMessage;
        debugMessage << "processEvent() : Processing " << localList->keys.size() << " SimpleNode updates";
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, debugMessage.str().c_str() );

		// for each of the data nodes in the local list
		unsigned int counter = 0;
		std::vector < unsigned long > ::iterator keyItr;
		for ( keyItr = localList->keys.begin();
			  keyItr != localList->keys.end();
			  keyItr++ )
		{
			unsigned long entityKey = (*keyItr);

			// First find out if this dn is already observed
			TA_Base_Bus::SimpleNode * existingSimpleNode = 0;
			existingSimpleNode = findExistingSimpleNode ( entityKey );

			// if found none
			if ( 0 == existingSimpleNode )
			{
				// get the data node from the specific agent
				TA_Base_Bus::SimpleNode * pSimpleNode = 0;
				pSimpleNode = m_specificSimpleNodeAgent->getSimpleNode ( entityKey );

				// if found a data node with matched pkey
				if ( 0 != pSimpleNode )
				{
					// peter low: register first then only get updates
					registerForUpdates ( pSimpleNode );
					
					// get its complete internal state			
					try
					{
						TA_Base_Bus::DataNodeDisplayState dnDynamicState = pSimpleNode->getDisplayState();
						
						// save the data point complete state to updateDetails
						TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState updateDetail;
						//convertDnCompleteUpdate(*dnCompleteDynamicSyncState, updateDetail);
						////TO DO - --- Initialize with data
						//TA_Base_Bus::DataNodeDisplayState updateState;
						saveSimpleNodeUpdateDetails ( pSimpleNode->getEntityKey(), dnDynamicState );
						
						counter++;
					}
					catch ( ... )
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve SimpleNode State for SimpleNode key %d",
							entityKey);
					}
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "processEvent() : No SimpleNode for entity: %d",
								 entityKey);
				}
				// else do nothing

				// take a little break after processing 20 data nodes
				if ( 20 <= counter )
				{
					TA_Base_Core::Thread::sleep  ( 1 );

					counter = 0;
				}
			}
		}

		localList->keys.clear();
		FUNCTION_EXIT;
	}


	void SimpleNodeObserver::registerForUpdates( TA_Base_Bus::SimpleNode * pSimpleNode )
	{
		FUNCTION_ENTRY ("registerForUpdates()" );
		// stop multi threads adding/removing data nodes while we are modifying
		unsigned long dnkey = pSimpleNode->getEntityKey();
		TA_THREADGUARD( m_observedSimpleNodeListLock );

		// save the reference of data node to internal list
		m_listOfObservedSimpleNodes [ dnkey ] = pSimpleNode;

		// register to the data node for callback of latest update details
		pSimpleNode->registerForUpdateDetails ( this );
		FUNCTION_EXIT;
	}


	void SimpleNodeObserver::processSimpleNodeUpdate(unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> updateDetails)
	{
		//
		// callback from one of the SimpleNode supposedly being assigned to this observer
		//

		std::stringstream ss;
		ss << "processNodeUpdate: Observer ID "  << m_observerID << "--> received an update from data node " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// only proceed if the specified pkey matches with the pkey of the
		// data node assigned to this observer
		SimpleNode * existingSimpleNode = 0;
		existingSimpleNode = findExistingSimpleNode ( pkey );

		// if found one
		if ( 0 != existingSimpleNode )
		{
			//convet it from cpp to corba first.
			TA_Base_Bus::DataNodeCorbaTypes::USyncUpdate dnUpdate;
			
			//convertDnDynamicSyncUpdate(*updateDetails, dnUpdate);
			// TO DO ---convert properly!!
			TA_Base_Bus::DataNodeDisplayState update;
			saveSimpleNodeUpdateDetails ( pkey, update);
		}
		// else do nothing

		FUNCTION_EXIT;
	}


	void SimpleNodeObserver::saveSimpleNodeUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataNodeDisplayState & updateDetails )
	{
		std::stringstream ss;
		ss << "saveSimpleNodeUpdateDetails: Observer ID "  << m_observerID << "--> saving updates from data node " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );

		// create instance of SimpleNodeUpdate
		//TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate * dnUpdate = 0;
		//dnUpdate = new TA_Base_Bus::DataNodeCorbaTypes::SSyncUpdate();

		//// populate the new DataNodeUpdate with the info from the SimpleNode
		//dnUpdate->dnKey = pkey;
		//dnUpdate->syncUpdate = updateDetails;
		// create instance of SimpleNodeUpdate
		TA_Base_Bus::DataNodeDisplayState* dnUpdate = 0;
		dnUpdate = new TA_Base_Bus::DataNodeDisplayState(updateDetails);
		// save the new SimpleNodeUpdate to the list of current updates
		m_listOfUpdateDetails.push_back ( dnUpdate );

		FUNCTION_EXIT;
	}


	unsigned long SimpleNodeObserver::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}
}
