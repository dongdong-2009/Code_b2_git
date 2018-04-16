/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/OpcSimpleNodeAgentCorbaDef_Impl.cpp $
  * @author:  Dhanshri Mokashi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2016/07/01 18:24:37 $
  * Last modified by:  $Author: Dhanshri Mokashi $
  *
  *	OpcSimpleNodeAgentCorbaDef_Impl is an abstract base class that implements
  * the ISimpleNodeAgentCorbaDef interface, providing SimpleNode configurations
  * and updates to OPCBridge services serving MFTs.
  */

#include "OpcSimpleNodeAgentCorbaDef_Impl.h"
#include "SimpleNode.h"
#include "ISimpleNodeAgent.h"
#include "SimpleNodeObserver.h"
#include "SimpleNodeDisplayClient.h"
#include "SimpleNodeStateContainer.h"
#include "ISimpleNodeObserverNotification.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/naming/src/Naming.h"

using TA_Base_Core::DebugUtil;
//TD15618, use timer to trigger if the observers are timeout
namespace TA_Base_Bus
{

	OpcSimpleNodeAgentCorbaDef_Impl::OpcSimpleNodeAgentCorbaDef_Impl( const std::string & agentName, TA_Base_Bus::ISimpleNodeAgent * specificSimpleNodeAgent )
	:
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::NotApplicable ),
	m_generatedObservedID ( time( NULL ) ),
	m_specificAgent( specificSimpleNodeAgent ),
    m_generatedClientID( time( NULL ) ),
    m_stateContainer(NULL),
	m_servantActivated(false)
	{
        FUNCTION_ENTRY("OpcSimpleNodeAgentCorbaDef_Impl::OpcSimpleNodeAgentCorbaDef_Impl()");
		m_listOfDisplayClients.clear();
		m_listOfObservers.clear();

        m_stateContainer = new SimpleNodeStateContainer(m_specificAgent);
        
		FUNCTION_EXIT;
	}


	OpcSimpleNodeAgentCorbaDef_Impl::~OpcSimpleNodeAgentCorbaDef_Impl()
	{
		FUNCTION_ENTRY("OpcSimpleNodeAgentCorbaDef_Impl::~OpcSimpleNodeAgentCorbaDef_Impl()");
		m_operationMode = TA_Base_Core::Monitor;
        // ensure the timer is no longer scheduled to call back on a NULL object
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        
		// clear the reference to the data node agent, without actually deleting
		// the object
		m_specificAgent = NULL;
        {
			// destroy all observers
        	TA_THREADGUARD( m_observerListLock );
		    // destroy all observers
		    std::map < unsigned long, SimpleNodeObserver * >::iterator itr;
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
		if (m_servantActivated)
		{
			deactivateServant();
		}
		// ++ CL-17309
		//deactivateAndDeleteServant();
		FUNCTION_EXIT;
	}


	void OpcSimpleNodeAgentCorbaDef_Impl::threadedTimerExpired(long timerId, void* userData)
	{
        FUNCTION_ENTRY("threadedTimerExpired()");
		// flag to indicate if all observers are gone
        bool allObserversEmpty = true;
        
        {
            // stop multi threads adding/removing observers while we are doing this
            TA_THREADGUARD( m_observerListLock );

		    // for each of the existing observers in the list
            std::map< unsigned long, SimpleNodeObserver * >::iterator itObserver = m_listOfObservers.begin();
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
		        std::map < unsigned long, SimpleNodeDisplayClient * >::iterator itDisplayClient = m_listOfDisplayClients.begin();
                while ( itDisplayClient != m_listOfDisplayClients.end() )
                {
			        if ( TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS < itDisplayClient->second->getIdleTime() )
			        {
				        // if this happens, destroy the observer
				        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
							"Clinet [%lu] timeout [%lu ms] expired and its updates will be deleted as well as itself", 
							itDisplayClient->second->getClientID(), TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS);

                        m_stateContainer->removeDisplayClient( itDisplayClient->first );

				        delete itDisplayClient->second;
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


	unsigned long OpcSimpleNodeAgentCorbaDef_Impl::generateNextObserverID()
	{
		// increment and return the ID
		return ++m_generatedObservedID;
	}

	unsigned long OpcSimpleNodeAgentCorbaDef_Impl::generateNextClientID()
	{
		// increment and return the ID
		m_generatedClientID = m_generatedClientID + 1;
		return m_generatedClientID;
	}

	SimpleNodeDisplayClient * OpcSimpleNodeAgentCorbaDef_Impl::findDisplayClient ( unsigned long clientId )
	{
		FUNCTION_ENTRY("findDisplayClient()");
		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		SimpleNodeDisplayClient * existingClient = 0;

		// find if the specified observerId is already in the internal list
		std::map < unsigned long, SimpleNodeDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( clientId );

		// if found one
		if ( itr != m_listOfDisplayClients.end() )
		{
			existingClient = itr->second;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No Client object for clientId %lu", clientId);	
		}
		FUNCTION_EXIT;
		return existingClient;
	}

	CORBA::ULong OpcSimpleNodeAgentCorbaDef_Impl::regiesterDisplayDataNodeClient() 
    {
		FUNCTION_ENTRY("regiesterDisplaySimpleNodeClient");

        // stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		unsigned long cleintID = generateNextClientID();
        if (m_stateContainer != NULL) 
        {				    
		    // create an instance of SimpleNodeObserver and put into the list for further
		    // references
		    m_listOfDisplayClients [ cleintID ] = new SimpleNodeDisplayClient ( cleintID );

            m_stateContainer->addDisplayClient(cleintID, m_listOfDisplayClients [ cleintID ]);

            std::stringstream ss;
		    ss << m_agentName << "-->register display SimpleNode client: has created Client ID " << cleintID;
		    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );
        }
		FUNCTION_EXIT;
		return cleintID;
    }

    void OpcSimpleNodeAgentCorbaDef_Impl::unregiesterDisplayDataNodeClient(CORBA::ULong cleintId)
	{
		FUNCTION_ENTRY("unregiesterDisplaySimpleNodeClient");

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

        if (m_stateContainer != NULL) 
        {
		    // find if the specified observerId is already in the internal list
		    std::map < unsigned long, SimpleNodeDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( cleintId );

		    // if found one
		    if ( itr != m_listOfDisplayClients.end() )
		    {
			    // destroy the existing observer from the list
                m_stateContainer->removeDisplayClient(cleintId);
			    delete itr->second;
			    m_listOfDisplayClients.erase ( itr );
		    }
		    // if found none
		    else
		    {
			    throw TA_Base_Core::BadParameterException();
		    }
        }

		FUNCTION_EXIT;
	}
    
    DataNodeCorbaTypes::DataNodeDisplayStateSequence* OpcSimpleNodeAgentCorbaDef_Impl::getDisplayDataNodeUpdate(CORBA::ULong clientId, CORBA::Boolean getall)
    {
		std::stringstream ss;
		ss << m_agentName << "-->getDisplayDataNodeUpdate for Client ID "  << clientId;
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

        if (m_stateContainer != NULL) 
        {
            // find if the specified clientid is already in the internal list
		    SimpleNodeDisplayClient * existingClient = 0;
		    existingClient = findDisplayClient ( clientId );

		    // if found one
		    if ( 0 != existingClient )
		    {
			    // get and return the data point updates from the observer
                if (getall)
                {
					FUNCTION_EXIT;
					return m_stateContainer->getAllSimpleNodeDisplayState();
                }
                else
                {
					FUNCTION_EXIT;
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

    void OpcSimpleNodeAgentCorbaDef_Impl::cleanupDisplayContainer()
    {
		FUNCTION_ENTRY("cleanupDisplayContainer()");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"m_listOfDisplayClients.size() = %d", m_listOfDisplayClients.size());
		if (m_stateContainer != NULL)
        {
		    TA_Base_Core::ThreadGuard guard( m_clientListLock );
		    // destroy all observers
		    std::map < unsigned long, SimpleNodeDisplayClient * >::iterator itrc;
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

	SimpleNodeObserver * OpcSimpleNodeAgentCorbaDef_Impl::findObserver ( unsigned long observerId )
	{
        FUNCTION_ENTRY("findObserver");

		// stop multi threads adding/removing observers at the same time
		TA_THREADGUARD( m_observerListLock );

		SimpleNodeObserver * existingObserver = 0;

		// find if the specified observerId is already in the internal list
		std::map< unsigned long, SimpleNodeObserver * >::iterator itObserver = m_listOfObservers.find( observerId );
		// if found one
		if ( itObserver != m_listOfObservers.end() )
		{
			existingObserver = itObserver->second;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No observer!");
			throw TA_Base_Core::BadParameterException();
        }

        FUNCTION_EXIT;
		return existingObserver;
	}

	CORBA::ULong OpcSimpleNodeAgentCorbaDef_Impl::regiesterDataNodeSynState()
    {
		FUNCTION_ENTRY("regiesterSimpleNodeSynState");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function regiesterDataNodeSynState() is not supported." 
			" The control-monitor synchronization is not required as of now.");
		throw TA_Base_Core::OperationNotSupportedException();

		FUNCTION_EXIT;
		return 0;
    }

	//
	// SimpleNodeAgentCorbaDef implementation
	//

	CORBA::ULong OpcSimpleNodeAgentCorbaDef_Impl::createObserver()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function createObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}

	void OpcSimpleNodeAgentCorbaDef_Impl::destroyObserver (CORBA::ULong observerId)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function destroyObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}

	void OpcSimpleNodeAgentCorbaDef_Impl::addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function addToObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}

	void OpcSimpleNodeAgentCorbaDef_Impl::removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function removeFromObserver() is not supported.");
		throw TA_Base_Core::OperationNotSupportedException();
	}

	DataNodeCorbaTypes::SyncUpdateSequence* OpcSimpleNodeAgentCorbaDef_Impl::getUpdates(CORBA::ULong observerId)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Corba function getUpdates() is not supported." 
			" The control-monitor synchronization is not required as of now.");
		throw TA_Base_Core::OperationNotSupportedException();
	}

	void OpcSimpleNodeAgentCorbaDef_Impl::setToControl()
	{
        FUNCTION_ENTRY("setToControl");
		m_operationMode = TA_Base_Core::Control;

		if ( !m_servantActivated )
		{
			m_servantActivated = true;
			activateServantWithName ( m_agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE );
		}

        //todo: check whether the container is constructed or not
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        
		// start the periodic timer
		TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 
			TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS / 2, false );
        FUNCTION_EXIT;
	}

	void OpcSimpleNodeAgentCorbaDef_Impl::setToMonitor()
	{
        FUNCTION_ENTRY("setToMonitor");

		if ( !m_servantActivated )
		{
			m_servantActivated = true;
			activateServantWithName ( m_agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE );
		}

		m_operationMode = TA_Base_Core::Monitor;
		m_generatedObservedID = time ( NULL );
		
        m_generatedClientID = time ( NULL );

		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        FUNCTION_EXIT;
	}
        
	void OpcSimpleNodeAgentCorbaDef_Impl::setToStandby()
	{
		FUNCTION_ENTRY("setToStandby");

		if ( m_servantActivated )
		{
			m_servantActivated = false;
			deactivateServant();
		}

		m_operationMode = TA_Base_Core::Standby;
		m_generatedObservedID = time ( NULL );
		m_generatedClientID = time ( NULL );
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
		FUNCTION_EXIT;
	}

	void OpcSimpleNodeAgentCorbaDef_Impl::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		FUNCTION_ENTRY("setOperationMode()");
		if ( m_operationMode != mode )
		{
			m_operationMode = mode;

			switch (m_operationMode)
			{
			case TA_Base_Core::Monitor:
				setToMonitor();
				break;
			case TA_Base_Core::Control:
				setToControl();
				break;
			case TA_Base_Core::Standby:
				setToStandby();
				break;
			default:
				break;
			}
		}
		FUNCTION_EXIT;
	}
}
