/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/OpcSimplePointAgentCorbaDef_Impl.cpp $
  * @author  HoaVu
  * @version $Revision: #6 $
  * Last modification : $DateTime: 2014/07/11 18:24:37 $
  * Last modified by : $Author: haijun.li $
  *
  *	OpcSimplePointAgentCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing SimplePoint configurations
  * and updates to remote clients.
  *
  */

#include "SimplePoint.h"
#include "SimplePointObserver.h"
#include "SimplePointDisplayClient.h"
#include "SimplePointStateContainer.h"
#include "OpcSimplePointAgentCorbaDef_Impl.h"
#include "ISimplePointAgent.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/naming/src/Naming.h"

#include "time.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	OpcSimplePointAgentCorbaDef_Impl::OpcSimplePointAgentCorbaDef_Impl ( const std::string & agentName, TA_Base_Bus::ISimplePointAgent * specificDataPointAgent)
	:
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::Monitor ),
	m_generatedObservedID ( time( NULL ) ) ,
	m_specificAgent ( specificDataPointAgent ),	
    m_generatedClientID( time( NULL ) ),
    m_stateContainer(NULL)
	{
 		FUNCTION_ENTRY("OpcSimplePointAgentCorbaDef_Impl");
        m_listOfDisplayClients.clear();
		m_listOfObservers.clear();

		// activate the ServantBase of this class
		activateServantWithName ( m_agentName + TA_Base_Bus::DP_BULK_POLLED_INTERFACE);

        m_stateContainer = new SimplePointStateContainer(m_specificAgent);

		FUNCTION_EXIT;
	}


	OpcSimplePointAgentCorbaDef_Impl::~OpcSimplePointAgentCorbaDef_Impl()
	{
 		FUNCTION_ENTRY("~OpcSimplePointAgentCorbaDef_Impl");
		m_operationMode = TA_Base_Core::Monitor;
        // ensure the timer is no longer scheduled to call back on a NULL object
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        
		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificAgent = NULL;
        {
			// destroy all observers
        	TA_THREADGUARD( m_observerListLock );
		    // destroy all observers
		    std::map < unsigned long, SimplePointObserver * >::iterator itr;
		    for ( itr = m_listOfObservers.begin();
			      itr != m_listOfObservers.end();
			      itr++ )
		    {
			    delete itr->second;
				itr->second = NULL;
		    }
		    m_listOfObservers.clear();
        }

        cleanupDisplayContainer();

		// CL-17309 ++
		deactivateServant();
		// this will cause recursion call of destructor
		//deactivateAndDeleteServant();
		// ++ CL-17309
		FUNCTION_EXIT;
	}

	//TD15618, use timer to trigger if the observers are timeout
	void OpcSimplePointAgentCorbaDef_Impl::threadedTimerExpired(long timerId, void* userData)
	{
 		FUNCTION_ENTRY("threadedTimerExpired");
        // flag to indicate if all observers are gone
        bool allObserversEmpty = true;
        
        {
		    // stop multi threads adding/removing observers while we are doing this
		    TA_THREADGUARD( m_observerListLock );

		    // for each of the existing observers in the list
			std::map < unsigned long, SimplePointObserver * >::iterator itObserver = m_listOfObservers.begin();
			while ( itObserver != m_listOfObservers.end() )
			{
			   // if the observer indicates that the latest updates from the SimpleNode
			   // instances that assigned to it have not been fetched by a remote client
			   // for the idle time
					  if ( TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS < itObserver->second->getIdleTime() )
			    {
				    // if this happens, destroy the observer
				    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
						"Observer [%lu] timeout [%lu ms] expired and its updates will be deleted as well as itself", 
						itObserver->second->getObserverID(), TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS);
			
					// destroy the existing observer from the list
					delete itObserver->second;
					itObserver->second = NULL;
					m_listOfObservers.erase( itObserver++ );
				}
				else
				{
					++itObserver;
				}
			}
            
            allObserversEmpty = m_listOfObservers.empty();
		}
        
        {
		    TA_THREADGUARD( m_clientListLock );
              
            if (m_stateContainer != NULL) 
			{        
				std::map < unsigned long, SimplePointDisplayClient * >::iterator itDisplayClient = m_listOfDisplayClients.begin();
				while ( itDisplayClient != m_listOfDisplayClients.end() )
				{
			        if ( TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS < itDisplayClient->second->getIdleTime() )
			        {
				        // if this happens, destroy the observer
				        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
							"Clinet [%lu] timeout [%lu ms] expired and its updates will be deleted as well as itself", 
							itDisplayClient->second->getClientID(), TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS);                        

						m_stateContainer->removeDisplayClient(itDisplayClient->first);

						delete itDisplayClient->second;
						itDisplayClient->second = NULL;
						m_listOfDisplayClients.erase( itDisplayClient++ );
					}
					else
					{
						++itDisplayClient;
			        }
		        }
            }
            
            allObserversEmpty = allObserversEmpty && m_listOfDisplayClients.empty();
        }
        
        if ( ( TA_Base_Core::Control != m_operationMode ) && 
            ( true == allObserversEmpty ) )
        {
            TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        }
		FUNCTION_EXIT;
	}


	unsigned long OpcSimplePointAgentCorbaDef_Impl::generateNextObserverID()
	{
		// increment and return the ID
	  return ++m_generatedObservedID;
	}


	unsigned long OpcSimplePointAgentCorbaDef_Impl::generateNextClientID()
	{
		// increment and return the ID
		m_generatedClientID = m_generatedClientID + 1;
		return m_generatedClientID;
	}

	SimplePointDisplayClient * OpcSimplePointAgentCorbaDef_Impl::findDisplayClient ( unsigned long clientId )
	{
 		FUNCTION_ENTRY("findDisplayClient");
		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		SimplePointDisplayClient * existingClient = 0;

		// find if the specified observerId is already in the internal list
		std::map < unsigned long, SimplePointDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( clientId );

		// if found one
		if ( itr != m_listOfDisplayClients.end() )
		{
			existingClient = itr->second;
		}
		FUNCTION_EXIT;

		return existingClient;
	}


	CORBA::ULong OpcSimplePointAgentCorbaDef_Impl::regiesterDisplayDataPointClient()
    {
 		FUNCTION_ENTRY("regiesterDisplayDataPointClient");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "regiesterDisplayDataPointClient");

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		unsigned long cleintID = generateNextClientID();
        if (m_stateContainer != NULL) 
        {				    
		    // create an instance of SimplePointObserver and put into the list for further
		    // references
		    m_listOfDisplayClients [ cleintID ] = new SimplePointDisplayClient ( cleintID );

            m_stateContainer->addDisplayClient(cleintID, m_listOfDisplayClients [ cleintID ]);

            std::stringstream ss;
		    ss << m_agentName << "-->register display SimplePoint client: has created Client ID " << cleintID;
		    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "finished to regiesterDisplaySimplePointClient, clientId[%d]", cleintID);
		FUNCTION_EXIT;
		return cleintID;
    }


    void OpcSimplePointAgentCorbaDef_Impl::unregiesterDisplayDataPointClient(CORBA::ULong cleintId)
	{
 		FUNCTION_ENTRY("unregiesterDisplayDataPointClient");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "unregiesterDisplaySimplePointClient, clientId[%d]", cleintId);

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

        if (m_stateContainer != NULL) 
        {
		    // find if the specified observerId is already in the internal list
		    std::map < unsigned long, SimplePointDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( cleintId );

		    // if found one
		    if ( itr != m_listOfDisplayClients.end() )
		    {
			    // destroy the existing observer from the list
                m_stateContainer->removeDisplayClient(cleintId);
			    delete itr->second;
				itr->second = NULL;
			    m_listOfDisplayClients.erase ( itr );
		    }
		    // if found none
		    else
		    {
			    throw TA_Base_Core::BadParameterException();
		    }
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "finished to unregiesterDisplaySimplePointClient, clientId[%d]", cleintId);
		FUNCTION_EXIT;
	}

    
    DataPointCorbaTypes::DisplayStateSequence* OpcSimplePointAgentCorbaDef_Impl::getDisplayDataPointUpdate(CORBA::ULong clientId, CORBA::Boolean getall)
    {
		std::stringstream ss;
		ss << m_agentName << "-->getDisplayDataPointUpdate for Client ID "  << clientId;
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

        if (m_stateContainer != NULL) 
        {
            // find if the specified clientid is already in the internal list
		    SimplePointDisplayClient * existingClient = 0;
		    existingClient = findDisplayClient ( clientId );

		    // if found one
		    if ( 0 != existingClient )
		    {
			    // get and return the data point updates from the observer
                if (getall)
                {
                    return m_stateContainer->getAllSimplePointDisplayState();
                }
                else
                {
                    return existingClient->getCurrrentDisplayUpdates();
                }
		    }
		    // if found none
		    else
		    {
			    throw TA_Base_Core::BadParameterException();
		    }
        }
        else
        {
			throw TA_Base_Core::OperationModeException();
        }
		FUNCTION_EXIT;
    }


    void OpcSimplePointAgentCorbaDef_Impl::cleanupDisplayContainer()
    {
 		FUNCTION_ENTRY("cleanupDisplayContainer");
       if (m_stateContainer != NULL)
        {
		    TA_Base_Core::ThreadGuard guard( m_clientListLock );
		    // destroy all observers
		    std::map < unsigned long, SimplePointDisplayClient * >::iterator itrc;
		    for ( itrc = m_listOfDisplayClients.begin();
			      itrc != m_listOfDisplayClients.end();
			      ++itrc )
		    {
                m_stateContainer->removeDisplayClient(itrc->first);
			    delete itrc->second;
				itrc->second = NULL;
		    }
		    m_listOfDisplayClients.clear();
            //clean up the state container too
            delete m_stateContainer;
            m_stateContainer = NULL;
        }
		FUNCTION_EXIT;
    }


	void OpcSimplePointAgentCorbaDef_Impl::cleanupObserversAndClients()
	{
		FUNCTION_ENTRY("cleanupObserversAndClients");
		{
			TA_THREADGUARD( m_observerListLock );
			
		    std::map < unsigned long, SimplePointObserver * >::iterator itr;
		    for ( itr = m_listOfObservers.begin();
			      itr != m_listOfObservers.end();
			      itr++ )
		    {
			    delete itr->second;
				itr->second = NULL;
		    }
		    m_listOfObservers.clear();
		}
	
		{
			TA_Base_Core::ThreadGuard guard( m_clientListLock );
			
		    std::map < unsigned long, SimplePointDisplayClient * >::iterator itrc;
		    for ( itrc = m_listOfDisplayClients.begin();
			      itrc != m_listOfDisplayClients.end();
			      ++itrc )
		    {
	            m_stateContainer->removeDisplayClient(itrc->first);
			    delete itrc->second;
		    }
		    m_listOfDisplayClients.clear();
		}
		FUNCTION_EXIT;
	}

	SimplePointObserver * OpcSimplePointAgentCorbaDef_Impl::findObserver ( unsigned long observerId )
	{
        FUNCTION_ENTRY("findObserver");

		// stop multi threads adding/removing observers at the same time
		TA_THREADGUARD( m_observerListLock );

		SimplePointObserver * existingObserver = 0;

		// find if the specified observerId is already in the internal list
		std::map < unsigned long, SimplePointObserver * >::iterator itr = m_listOfObservers.find ( observerId );

		// if found one
		if ( itr != m_listOfObservers.end() )
		{
			existingObserver = itr->second;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No observer!");
			throw TA_Base_Core::BadParameterException();
        }

        FUNCTION_EXIT;
		return existingObserver;
	}


	CORBA::ULong OpcSimplePointAgentCorbaDef_Impl::regiesterDataPointSynState()
    {
		FUNCTION_ENTRY("regiesterDataPointSynState");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function regiesterDataPointSynState() is not supported." 
			" The control-monitor synchronization is not required as of now.");
		throw TA_Base_Core::OperationNotSupportedException();
		FUNCTION_EXIT;
		return 0;
    }
		

	CORBA::ULong OpcSimplePointAgentCorbaDef_Impl::createObserver()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function createObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}


	void OpcSimplePointAgentCorbaDef_Impl::destroyObserver(CORBA::ULong observerId)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function destroyObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}


	void OpcSimplePointAgentCorbaDef_Impl::addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function addToObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}


	void OpcSimplePointAgentCorbaDef_Impl::removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function removeFromObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}


	DataPointCorbaTypes::DynamicSyncUpdateSequence* OpcSimplePointAgentCorbaDef_Impl::getUpdates(CORBA::ULong observerId)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function getUpdates() is not supported." 
			" The control-monitor synchronization is not required as of now.");
		throw TA_Base_Core::OperationNotSupportedException();
	}


	void OpcSimplePointAgentCorbaDef_Impl::setToControl()
	{
        FUNCTION_ENTRY("setToControl");

		m_operationMode = TA_Base_Core::Control;

		TA_ASSERT(m_stateContainer != NULL, "Failed to create SimplePoint container in OpcSimplePointAgentCorbaDef_Impl.");
    
		// the timer may not have been stopped, it is stopped once all observers are deleted
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );

		// start the periodic timer
		TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this,
			TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS / 2, false );
        
        FUNCTION_EXIT;
	}


	void OpcSimplePointAgentCorbaDef_Impl::setToMonitor()
	{
        FUNCTION_ENTRY("setToMonitor");

		m_operationMode = TA_Base_Core::Monitor;
        m_generatedObservedID = time ( NULL );
        m_generatedClientID = time ( NULL );

        FUNCTION_EXIT;
	}

	void OpcSimplePointAgentCorbaDef_Impl::setToStandby()
	{
		FUNCTION_ENTRY("setToStandby");

		m_operationMode = TA_Base_Core::Standby;

		m_generatedObservedID = time ( NULL );
		m_generatedClientID = time ( NULL );
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
		FUNCTION_EXIT;
	}
	
	void OpcSimplePointAgentCorbaDef_Impl::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		FUNCTION_ENTRY("setOperationMode");
		m_operationMode = mode;
		if (TA_Base_Core::Control == m_operationMode)
		{
			setToControl();
		}
		else if (TA_Base_Core::Monitor == m_operationMode)
		{
			setToMonitor();
		}
		else
		{
			TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
			cleanupObserversAndClients();
		}
		FUNCTION_EXIT;
	}
	
}
