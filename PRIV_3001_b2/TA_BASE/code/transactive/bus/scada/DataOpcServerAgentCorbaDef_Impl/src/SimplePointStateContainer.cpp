/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/SimplePointStateContainer.cpp $
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
#include "SimplePoint.h"
#include "OpcSimplePointAgentCorbaDef_Impl.h"
#include "SimplePointStateContainer.h"
#include "ISimplePointAgent.h"

using TA_Base_Core::DebugUtil;

//TD15618, don't purge anything before the observer is destroyed. 
//The Agent will destroy this observer after timeout.
namespace TA_Base_Bus
{
	SimplePointStateContainer::SimplePointStateContainer ( TA_Base_Bus::ISimplePointAgent * specificSimplePointAgent)
	:m_specificSimplePointAgent ( NULL )
	{
 		FUNCTION_ENTRY("SimplePointStateContainer");
		m_listOfObservedSimplePoints.clear();
        m_listOfSimplePointDisplayState.clear();

		// save the pointer to the data point agent for later usage
		m_specificSimplePointAgent = specificSimplePointAgent;

        loadAllSimplePoints();
        FUNCTION_EXIT;
	}

	SimplePointStateContainer::~SimplePointStateContainer()
	{
 		FUNCTION_ENTRY("~SimplePointStateContainer");
        {
            TA_THREADGUARD( m_observedSimplePointListLock );
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
        }


        {
            TA_THREADGUARD( m_clientListLock );
            //clear the clinet list first
            m_listOfSimplePointClients.clear();
        }

        {
            TA_THREADGUARD( m_listOfSimplePointDisplayStateLock );
            //remove the SimplePoint observer and display list
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator itrds;
            for (itrds = m_listOfSimplePointDisplayState.begin(); itrds != m_listOfSimplePointDisplayState.end(); itrds++)
            {
                removeSimplePoint(itrds->first);
			    delete (itrds->second);
			    itrds->second = NULL;
            }
        }

		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificSimplePointAgent = NULL;
        FUNCTION_EXIT;
	}

    SimplePoint * SimplePointStateContainer::findExistingSimplePoint ( unsigned long entityKey )
	{
 		FUNCTION_ENTRY("findExistingSimplePoint");
		// stop multi threads adding/removing data points while we are searching
		TA_THREADGUARD( m_observedSimplePointListLock );

		SimplePoint * dp = 0;

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

	void SimplePointStateContainer::removeSimplePoint ( unsigned long entityKey )
	{
 		FUNCTION_ENTRY("removeSimplePoint");
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedSimplePointListLock );

		// find if the specified entity key matches with the pkey of one of the existing
		// data points in the list
		std::map < unsigned long, SimplePoint * >::iterator itr = m_listOfObservedSimplePoints.find ( entityKey );

		// if found one
		if ( itr != m_listOfObservedSimplePoints.end() )
		{
			itr->second->deregisterForUpdateDetails ( this );
			// remove the iterator pointing to the data point, without deleting
			// the data point itself since it is not created and managed by this class
			m_listOfObservedSimplePoints.erase ( itr );
		}
        FUNCTION_EXIT;
		// else do nothing
	}

    void SimplePointStateContainer::registerForUpdates ( TA_Base_Bus::SimplePoint * SimplePoint )
	{
 		FUNCTION_ENTRY("registerForUpdates");
		// stop multi threads adding/removing data points while we are modifying
		TA_THREADGUARD( m_observedSimplePointListLock );

        unsigned long dpkey = SimplePoint->getEntityKey();
		// save the reference of data points to internal list
		m_listOfObservedSimplePoints [ dpkey ] = SimplePoint;

		// register to the SimplePoint for callback of latest update details
		SimplePoint->registerForUpdateDetails ( this );
        FUNCTION_EXIT;
	}


	void SimplePointStateContainer::processSimplePointUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails )
	{
		//
		// callback from one of the SimplePoint supposedly being assigned to this observer
		//
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimplePointStateContainer::processSimplePointUpdate() - EK %u", pkey);
		std::stringstream ss;
		ss << "processSimplePointUpdate: --> received an update from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );

		// only proceed if the specified pkey matches with the pkey of the
		// data point assigned to this observer
		SimplePoint * existingSimplePoint = 0;
		existingSimplePoint = findExistingSimplePoint ( pkey );

		// if found one
		if ( 0 != existingSimplePoint )
		{
            TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState update;
            try
            {
				TA_Base_Bus::DataPointDisplayState dpDisplay = existingSimplePoint->getDisplayState();
				
				{
					convertDpDisplayUpdate(dpDisplay, update);
				}
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught exception trying to retrieve SimplePoint State for  key %d",
							 pkey);
			}

            saveSimplePointUpdateDetails ( pkey, update );
		}
		// else do nothing

		FUNCTION_EXIT;
	}


    void SimplePointStateContainer::saveSimplePointUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState & updateDetails )
    {
		std::stringstream ss;
		ss << "saveSimplePointUpdateDetails: --> saving updates from data point " << pkey;
		FUNCTION_ENTRY (ss.str().c_str() );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In SimplePointStateContainer::saveSimplePointUpdateDetails() - EK %u", pkey);

        {
		    TA_THREADGUARD( m_listOfSimplePointDisplayStateLock );
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * >::iterator itr;
			itr = m_listOfSimplePointDisplayState.find(pkey);
			if(itr != m_listOfSimplePointDisplayState.end())
			{
				*(itr->second) = updateDetails;
			}
        }
		// create instance of SimplePointUpdate
        boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> dpUpdate (new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState);

		// populate the new SimplePointUpdate with the info from the SimplePoint
		*dpUpdate = updateDetails;

        // notify all SimplePointDisplayClient this update
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        
        std::map < unsigned long, SimplePointDisplayClient * >::iterator itr = m_listOfSimplePointClients.end();
		for ( itr = m_listOfSimplePointClients.begin();
			  itr != m_listOfSimplePointClients.end();
			  itr++ )
		{
            // notify each client
            (*itr).second->saveSimplePointDisplayUpdateDetails(dpUpdate);
		}
		FUNCTION_EXIT;
    }

    void SimplePointStateContainer::addDisplayClient(unsigned long clientID, SimplePointDisplayClient * displayClient)
    {
 		FUNCTION_ENTRY("addDisplayClient");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In SimplePointStateContainer::addDisplayClient() - clientID %u", clientID);

        TA_Base_Core::ThreadGuard guard( m_clientListLock );

        m_listOfSimplePointClients[clientID] = displayClient;
        FUNCTION_EXIT;
    }

    void SimplePointStateContainer::removeDisplayClient(unsigned long clientID)
    {
 		FUNCTION_ENTRY("removeDisplayClient");
        TA_Base_Core::ThreadGuard guard( m_clientListLock );
        std::map < unsigned long, SimplePointDisplayClient * >::iterator itr;
        itr = m_listOfSimplePointClients.find(clientID);
        if (itr != m_listOfSimplePointClients.end())
        {
            m_listOfSimplePointClients.erase(itr);
        }
        FUNCTION_EXIT;
    }

    void SimplePointStateContainer::loadAllSimplePoints()
    {
        FUNCTION_ENTRY("loadAllSimplePoints");
        {
		    TA_THREADGUARD( m_observedSimplePointListLock );
            m_specificSimplePointAgent->getAllSimplePoints(m_listOfObservedSimplePoints);
        }
        
        {
		    TA_THREADGUARD( m_listOfSimplePointDisplayStateLock );
            if (m_listOfObservedSimplePoints.size() > 0)
            {
                std::map< unsigned long, SimplePoint * >::iterator itr;
                for (itr = m_listOfObservedSimplePoints.begin(); itr != m_listOfObservedSimplePoints.end(); ++itr)
                {
                    TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * dpUpdate = 0;
                    dpUpdate = new TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState();

					// peter low: register first then only get updates
					registerForUpdates ( itr->second );
                    
                    try
                    {
						TA_Base_Bus::DataPointDisplayState dpDisplay = (*itr).second->getDisplayState();
						
						//if ( 0 != dpDisplay.get() )
						{
							convertDpDisplayUpdate(dpDisplay, *dpUpdate);
						}

                        m_listOfSimplePointDisplayState.insert(std::pair<unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *>(itr->first, dpUpdate));
                    }
			        catch ( ... )
			        {
				        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught unknown exception while trying to retrieve SimplePoint State for key %d",
							         itr->first);
			        }
                }
           }
        }

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* SimplePointStateContainer::getAllSimplePointDisplayState()
    {
        FUNCTION_ENTRY("getAllSimplePointDisplayState");
        //todo: need to delete the new sequence or not
		TA_THREADGUARD( m_listOfSimplePointDisplayStateLock );
        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence * displaySequence = 0;
		displaySequence = new TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence();
		displaySequence->length ( m_listOfSimplePointDisplayState.size() );

        if (m_listOfSimplePointDisplayState.size() > 0)
        {
            std::map< unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *>::iterator itr;
		    unsigned int sequenceIndex = 0;
            for (itr = m_listOfSimplePointDisplayState.begin(); itr != m_listOfSimplePointDisplayState.end(); itr++)
            {
			    // copy it to the specified output sequence
               (*displaySequence)[sequenceIndex++] = (*(itr->second));
            }
        }
        FUNCTION_EXIT;
        return displaySequence;
    }    
}
