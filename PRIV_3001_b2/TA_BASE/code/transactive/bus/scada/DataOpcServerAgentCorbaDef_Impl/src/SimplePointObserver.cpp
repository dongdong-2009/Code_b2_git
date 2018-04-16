/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/12 17:13:30 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/timers/src/StopwatchUtil.h"

#include "SimplePoint.h"
#include "SimplePointObserver.h"
#include "ISimplePointAgent.h"
#include "OpcSimplePointAgentCorbaDef_Impl.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	SimplePointObserver::SimplePointObserver ( ISimplePointAgent * specificSimplePointAgent, unsigned long observerID )
	:
	m_observerID ( observerID ),
	m_purgeExistingUpdatesTimer ( NULL ),
	m_specificSimplePointAgent ( NULL ),
	TA_Base_Core::QueueProcessor< TA_Base_Bus::DpObserverUpdate > ()
	{
		FUNCTION_ENTRY("SimplePointObserver()");
		m_listOfObservedSimplePoints.clear();
		m_listOfUpdateDetails.clear();

		// save the pointer to the data point agent for later usage
		m_specificSimplePointAgent = specificSimplePointAgent;

		// create and start the timer
		m_purgeExistingUpdatesTimer = new TA_Base_Core::StopwatchUtil();
		m_purgeExistingUpdatesTimer->startTiming();

		//TD13275
		start();
		FUNCTION_EXIT;
	}


	SimplePointObserver::~SimplePointObserver()
	{
		FUNCTION_ENTRY("~SimplePointObserver()");
		//TD13275
		terminateAndWait();

		if ( NULL != m_purgeExistingUpdatesTimer )
		{
			m_purgeExistingUpdatesTimer->stopTiming();
			delete m_purgeExistingUpdatesTimer;
			m_purgeExistingUpdatesTimer = NULL;
		}

		// reset the list of observed SimplePoints, without deleting
		// the actual SimplePoints since they are not created and owned
		// by this class

		std::map < unsigned long, SimplePoint * >::iterator dpItr;
		for ( dpItr = m_listOfObservedSimplePoints.begin();
			  dpItr != m_listOfObservedSimplePoints.end();
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
		m_listOfObservedSimplePoints.clear();


		// remove existing TA_Base_Bus::DataPointAgentCorbaDef::DataPointUpdate
		std::vector < TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* >::iterator itr;
		for ( itr = m_listOfUpdateDetails.begin();
		itr != m_listOfUpdateDetails.end();
		itr++ )
		{
			delete (*itr);
			(*itr) = 0;
		}
		m_listOfUpdateDetails.clear();

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificSimplePointAgent = NULL;
		FUNCTION_EXIT;
	}


	unsigned long SimplePointObserver::getObserverID() const
	{
		return m_observerID;
	}


	SimplePoint * SimplePointObserver::findExistingSimplePoint ( unsigned long entityKey )
	{
		FUNCTION_ENTRY("findExistingSimplePoint()");
		// stop multi threads adding/removing data points while we are searching
		TA_THREADGUARD( m_observedSimplePointListLock );

		SimplePoint * dp = NULL;

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, SimplePoint * >::iterator itr = m_listOfObservedSimplePoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedSimplePoints.end() )
		{
			dp = itr->second;
		}
		// else do nothing
		FUNCTION_EXIT;

		return dp;
	}


	void SimplePointObserver::removeSimplePoint ( unsigned long entityKey )
	{
		FUNCTION_ENTRY("removeSimplePoint()");
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedSimplePointListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, SimplePoint * >::iterator itr = m_listOfObservedSimplePoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedSimplePoints.end() )
		{
			// tell the SimplePoint we are not interest in receiving callback of
			// new update details
			itr->second->deregisterForUpdateDetails ( this );

			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedSimplePoints.erase ( itr );
		}
		// else do nothing
		FUNCTION_EXIT;
	}


	void SimplePointObserver::addSimplePointsToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys )
	{
		FUNCTION_ENTRY("addSimplePointsToObserver()");
		if (entityKeys.length() > 0)
		{
			boost::shared_ptr<DpObserverUpdate> dpKeys (new DpObserverUpdate);

			// for each of the entity key specified in the key sequence
			for (unsigned int i = 0; i < entityKeys.length(); i++)
			{
				dpKeys->keys.push_back(entityKeys[i]);
			}
			// else do nothing if already in the list

			insert(dpKeys);
		}		
		FUNCTION_EXIT;
	}

	void SimplePointObserver::removeSimplePointsFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence & entityKeys )
	{
		FUNCTION_ENTRY("removeSimplePointsFromObserver()");
		// for each of the entity key specified in the key sequence
		for (unsigned int i = 0; i < entityKeys.length(); i++)
		{
			// remove the data point whose pkey matches with pkey specified in the sequence
			removeSimplePoint ( entityKeys[i] );
		}
		FUNCTION_EXIT;
	}

	void SimplePointObserver::processEvent( boost::shared_ptr<DpObserverUpdate> localList )
	{
		FUNCTION_ENTRY("processEvent()");
        std::stringstream logMessage;
        logMessage << "processSimplePointTempList : Processing " << localList->keys.size() << " DataPoints";
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );

		// for each of the data points in the local list
		unsigned int counter = 0;
		std::vector < unsigned long > ::iterator keyItr;
		for ( keyItr = localList->keys.begin();
			  keyItr != localList->keys.end();
			  keyItr++ )
		{
			unsigned long entityKey = (*keyItr);
			// First find out if this dn is already observed
			SimplePoint * existingSimplePoint = 0;
			existingSimplePoint = findExistingSimplePoint ( entityKey );

			// if found none
			if ( 0 == existingSimplePoint )
			{
				// get the data point from the specific agent
				SimplePoint * pSimplePoint = 0;
				pSimplePoint = m_specificSimplePointAgent->getSimplePoint (entityKey);

				// if found a data point with matched pkey
				if ( 0 != pSimplePoint )
				{
					// peter low: register first then only get updates
					registerForUpdates ( pSimplePoint );
					
					// get its complete internal state			
					try
					{
						TA_Base_Bus::DataPointDisplayState dpCompleteInternalState = pSimplePoint->getDisplayState();
						
						// CONVERT dpCompleteInternalState --> dpInternalState
						//if ( NULL != dpCompleteInternalState.get() )
						{
							// save the data point complete state to updateDetails
							TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState updateDetail;
							convertDpDisplayUpdate(dpCompleteInternalState, updateDetail);							
							saveSimplePointUpdateDetails ( pSimplePoint->getEntityKey(), updateDetail );
                            counter++;
						}				
						/*else
						{
							LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "SimplePoint::getState returned NULL pointer for pkey %d",
										 entityKey);
						} */
					}
					catch ( ... )
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve SimplePoint State for key %d",
									 entityKey);
					}
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "processEvent() : No SimplePoint for entity: %d",
								 entityKey);
				}
				// else do nothing

				// take a little break after processing 20 data points
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


	void SimplePointObserver::registerForUpdates ( SimplePoint * pSimplePoint )
	{
		FUNCTION_ENTRY("registerForUpdates()");
		//here is no need to check whether the dp is a Existing one. 
		//this checking already done by processEvent()
		unsigned long dpkey = pSimplePoint->getEntityKey();

		{
			// stop multi threads adding/removing data points while we are modifying
			TA_THREADGUARD( m_observedSimplePointListLock );
			// save the reference of data points to internal list
			m_listOfObservedSimplePoints [ dpkey ] = pSimplePoint;
		}

		// register to the SimplePoint for callback of latest update details
		pSimplePoint->registerForUpdateDetails ( this );
		FUNCTION_EXIT;
	}


	void SimplePointObserver::processSimplePointUpdate( unsigned long pkey, 
		boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails )
	{
		//
		// callback from one of the SimplePoint supposedly being assigned to this observer
		//
		std::stringstream ss;
		ss << "processSimplePointUpdate: Observer ID "  
		   << m_observerID << "--> received an update from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );


		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		SimplePoint * existingSimplePoint = 0;
		existingSimplePoint = findExistingSimplePoint ( pkey );

		// if found one
		if ( NULL != existingSimplePoint )
		{
			//convet it from cpp to corba first.
			TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState dpUpdate;

			convertDpDisplayUpdate(*updateDetails, dpUpdate);

			saveSimplePointUpdateDetails ( pkey, dpUpdate );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


	void SimplePointObserver::saveSimplePointUpdateDetails( unsigned long pkey, 
		TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& dpUpdate )
	{
		std::stringstream ss;
		ss << "saveSimplePointUpdateDetails: Observer ID "  << m_observerID << "--> saving updates from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// create instance of SimplePointUpdate
		TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* dpUpdateDetail = 
			new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState;
		// populate the new SimplePointUpdate with the info from the SimplePoint
		dpUpdateDetail->alarmAckState = dpUpdate.alarmAckState;
		dpUpdateDetail->alarmState = dpUpdate.alarmState;
		dpUpdateDetail->dataPointStatus = dpUpdate.dataPointStatus;
		dpUpdateDetail->entityKey = dpUpdate.entityKey;
		dpUpdateDetail->value = dpUpdate.value;
		// stop multi threads accessing the update details list
		TA_THREADGUARD( m_updateDetailsLock );

		// save the new SimplePointUpdate to the list of current updates
		m_listOfUpdateDetails.push_back ( dpUpdateDetail );

		FUNCTION_EXIT;
	}


	unsigned long SimplePointObserver::getIdleTime()
	{
		return m_purgeExistingUpdatesTimer->timeElapsed();
	}
}
