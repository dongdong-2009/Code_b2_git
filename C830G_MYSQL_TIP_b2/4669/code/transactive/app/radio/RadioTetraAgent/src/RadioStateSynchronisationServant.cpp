/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.cpp $
  * @author:  John Dalin
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2011/01/17 11:40:55 $
  * Last modified by:  $Author: builder $
  *
  * The RadioStateSynchronisationServant is a class that is used for synchronise state data between
  * the Radio Tetra Agent that is in control mode and the Radio Tetra Agent that is in monitor
  * mode.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4503)
#endif // _MSC_VER
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioTetraAgentStateUpdate_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.h"
#include "RadioStateUpdateBroadcaster.h"
#include "RadioStateMessageConverter.h"
#include "bus/generic_agent/src/GenericAgent.h"

#pragma warning (disable : 4355)


//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
        typedef void ( T::* MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
    public:
        
        static ThreadedMemberFunction& alloc( T& o, MemberFunction mf = NULL )
        {
			TA_Base_Core::ThreadGuard( lock() );
            
            static ThreadedMemberFunction* instance = NULL;
            
            instance = new ThreadedMemberFunction( o, instance );
            
            if ( NULL != mf )
            {
                instance->add( mf );
                
                instance->start();
            }
            
            return *instance;
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
    protected:
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member function(s) to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ThreadedMemberFunction: calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            clearPrivious();
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o, ThreadedMemberFunction* privious )
            : m_object( o ),
            m_privious( privious )
        {
        }
        
        ThreadedMemberFunction( const ThreadedMemberFunction& );
        ThreadedMemberFunction& operator= ( const ThreadedMemberFunction& );
        
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        void clearPrivious()
        {
            delete m_privious;
            m_privious = NULL;
        }
        
        static TA_Base_Core::NonReEntrantThreadLockable& lock()
        {
            static TA_Base_Core::NonReEntrantThreadLockable lock;
            return lock;
        }
        
    private:
        
        T& m_object;
        MemberFunctionList m_members;
        ThreadedMemberFunction* m_privious;
    };
    
    typedef ThreadedMemberFunction< TA_IRS_App::RadioStateSynchronisationServant > RadioStateSynchronisationServantThreadedMemberFunction;
}
//limin++, failover


namespace TA_IRS_App
{
    
    RadioStateSynchronisationServant::RadioStateSynchronisationServant(IRadioTetraAgentState& radioState,
        TA_Base_Core::RadioStateSynchronisationEntityDataPtr entityData)
        : m_radioTetraAgentState(radioState)
        , m_radioStateUpdateReceiver(radioState)
        , m_radioFullStateReceiver(radioState, *this, m_radioStateUpdateReceiver)
        , m_state(INITIAL)
        , m_subscribed(false)
		, m_monAgentState(false)
    {
        FUNCTION_ENTRY("Constructor");

        m_objectName = entityData->getName();
        m_entityKey = entityData->getKey();
        m_entityLocation = entityData->getLocation();
        m_entitySubsystem = entityData->getSubsystem();
        m_entityTypeKey = entityData->getTypeKey();

        // Setup the peer radio agent connection used for synchronisation
        m_peerRadioTetraAgent.setEntityName(m_objectName,true);

        // Let the broadcaster know the entity key
        RadioStateUpdateBroadcaster::getInstance()->configure(m_entityKey);

        FUNCTION_EXIT;
    }
    
    RadioStateSynchronisationServant::~RadioStateSynchronisationServant()
    {
        FUNCTION_ENTRY("Destructor");
    
        stop();

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType* RadioStateSynchronisationServant::requestFullState()
    {
        FUNCTION_ENTRY("requestFullState");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Monitor agent is requesting full state" );
        RadioTetraAgentType agentState;
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType* newPeerAgentState =
            new TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType();

		// TD9016 move getFullState() to RadioStateSynchronisationServant to prevent deadlock in RadioTetraAgent class
        //m_radioTetraAgentState.getFullState(agentState); 
		getFullState(agentState); 
        
		std::ostringstream data;
		data << std::endl
			 << "RadioServant --> Entity: " 
			 << agentState.radioServant.entityKey
			 << std::endl;

		RadioType::const_iterator it = agentState.radioServant.radio.begin();

		for(  ; it != agentState.radioServant.radio.end(); it ++ )
		{
			data << "CallStack " << it->first << ": "
				<< it->second.radioCallStack.size()
				<< " --- Control connection "<< it->first << " is "
				<< static_cast<int>(it->second.controlConnectionStatus) << std::endl;
		}

		for ( unsigned int i =0; i < agentState.radioSessionServants.size(); i++ )
		{
			RadioSessionServantType type = agentState.radioSessionServants[i];
			data << "RadioSessionServent --> Entity: " << type.entityKey << " "
				 << "SessionName: " << type.radioSession.sessionEntityName << " "
				 << "SessionRef: " << type.radioSession.sessionRef << " "
				 << "SessionStatus: " << static_cast<ESessionState>(type.radioSession.sessionStatus) << std::endl;
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Agent's Full State... %s", data.str().c_str() );

        RadioStateMessageConverter::convertRadioTetraAgentNormalToCorba(agentState, *newPeerAgentState);

        return newPeerAgentState;

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::receiveSpecialisedMessage(
        const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
        
        // determine message type then process message
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack* updateCallInRadioCallStackCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack* removeCallFromRadioCallStackCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession* updateRadioSessionCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression* updateMonitoredCallProgressionCorba = 0;	
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails* removeMonitoredSubscriberDetailsCorba = 0;	
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap* removeSessionFromAudioMapCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap* updateAudioEntryInSessionAudioMapCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars* updateRadioResourceManagerVarsCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest* updateAuthorisationRequestCorba = 0;	
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest* removeAuthorisationRequestCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest* connectionSwitchRequest = 0;

	    if ( 0 != ( message.stateUpdateInfo >>= updateCallInRadioCallStackCorba ) )   // Is this an Update Call In Radio Call Stack message?
		{
            RadioUpdateCallInRadioCallStack* updateCallInRadioCallStack =
                new RadioUpdateCallInRadioCallStack();

            RadioStateMessageConverter::convertRadioUpdateCallInRadioCallStackCorbaToNormal(*updateCallInRadioCallStackCorba,
                                                                *updateCallInRadioCallStack);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateCallInRadioCallStack);
		}
	    else if ( 0 != ( message.stateUpdateInfo >>= removeCallFromRadioCallStackCorba ) )   // Is this a Remove Call From Radio Call Stack message?
		{
            RadioRemoveCallFromRadioCallStack* removeCallFromRadioCallStack =
                new RadioRemoveCallFromRadioCallStack();
            RadioStateMessageConverter::convertRadioRemoveCallFromRadioCallStackCorbaToNormal(*removeCallFromRadioCallStackCorba,
                                                                  *removeCallFromRadioCallStack);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeCallFromRadioCallStack);
		}
	    else if ( 0 != ( message.stateUpdateInfo >>= updateRadioSessionCorba ) )   // Is this an Update Radio Session message?
		{
			RadioUpdateRadioSession* updateRadioSession = 
				new RadioUpdateRadioSession();
            RadioStateMessageConverter::convertRadioUpdateRadioSessionCorbaToNormal(*updateRadioSessionCorba,
                                                                  *updateRadioSession);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateRadioSession);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateMonitoredCallProgressionCorba ) )   // Is this an Update Monitored CallProgression message?
		{
			RadioUpdateMonitoredCallProgression* updateMonitoredCallProgression = 
							new RadioUpdateMonitoredCallProgression();
            RadioStateMessageConverter::convertRadioUpdateMonitoredCallProgressionCorbaToNormal(*updateMonitoredCallProgressionCorba,
                                                                  *updateMonitoredCallProgression);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateMonitoredCallProgression);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= removeMonitoredSubscriberDetailsCorba ) )   // Is this an Update Monitored CallProgression message?
		{
			RadioRemoveMonitoredSubscriberDetails* removeMonitoredSubscriberDetails = 
							new RadioRemoveMonitoredSubscriberDetails();
            RadioStateMessageConverter::convertRadioRemoveMonitoredSubscriberDetailsCorbaToNormal(*removeMonitoredSubscriberDetailsCorba,
                                                                  *removeMonitoredSubscriberDetails);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeMonitoredSubscriberDetails);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= removeSessionFromAudioMapCorba ) )   // Is this RemoveSessionFromAudioMap Message?
		{
            RadioRemoveSessionFromAudioMap* removeSessionFromAudioMap =
                new RadioRemoveSessionFromAudioMap();
            RadioStateMessageConverter::convertRadioRemoveSessionFromAudioMapCorbaToNormal(*removeSessionFromAudioMapCorba,
                                                                  *removeSessionFromAudioMap);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeSessionFromAudioMap);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateAudioEntryInSessionAudioMapCorba ) )   // Is this UpdateAudioEntryInSessionAudioMap Message?
		{
            RadioUpdateAudioEntryInSessionAudioMap * updateAudioEntryInSessionAudioMap  =
                new RadioUpdateAudioEntryInSessionAudioMap ();
            RadioStateMessageConverter::convertRadioUpdateAudioEntryInSessionAudioMapCorbaToNormal(*updateAudioEntryInSessionAudioMapCorba,
                                                                  *updateAudioEntryInSessionAudioMap );
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateAudioEntryInSessionAudioMap);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateRadioResourceManagerVarsCorba ) )   // Is this UpdateRadioResourceManagerVars Message?
		{
            RadioUpdateRadioResourceManagerVars * updateRadioResourceManagerVars  =
                new RadioUpdateRadioResourceManagerVars ();
            RadioStateMessageConverter::convertRadioUpdateRadioResourceManagerVarsCorbaToNormal(*updateRadioResourceManagerVarsCorba,
                                                                  *updateRadioResourceManagerVars );
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateRadioResourceManagerVars);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateAuthorisationRequestCorba ) )   // Is this UpdateAuthorisationRequest Message?
		{
            RadioUpdateAuthorisationRequest * updateAuthorisationRequest  =
                new RadioUpdateAuthorisationRequest ();
            RadioStateMessageConverter::convertRadioUpdateAuthorisationRequestCorbaToNormal(*updateAuthorisationRequestCorba,
                                                                  *updateAuthorisationRequest );
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateAuthorisationRequest);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= removeAuthorisationRequestCorba ) )   // Is this Remove AuthorisationRequest Message?
		{
            RadioRemoveAuthorisationRequest * removeAuthorisationRequest  =
                new RadioRemoveAuthorisationRequest();
            RadioStateMessageConverter::convertRadioRemoveAuthorisationRequestCorbaToNormal(*removeAuthorisationRequestCorba,
                                                                  *removeAuthorisationRequest);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeAuthorisationRequest);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= connectionSwitchRequest ) )   // Is this Remove AuthorisationRequest Message?
		{
            RadioConnectionSwitch * radioConnectionSwitchUpdate  =
                new RadioConnectionSwitch();
            RadioStateMessageConverter::convertRadioConnectionSwitchCorbaToNormal(*connectionSwitchRequest,
				*radioConnectionSwitchUpdate);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*radioConnectionSwitchUpdate);
		}

        FUNCTION_EXIT;
    }

    
    bool RadioStateSynchronisationServant::getFullPeerState(RadioTetraAgentType& state)
    {
        FUNCTION_ENTRY("getFullState");

        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType_var peerAgentState;
        bool successful = false;

        // get state from peer agent
        try
        {
			// TD9016 add more logs
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Requesting Full State...");

			unsigned int retry = 0;

			while ( retry < 3 )
			{
				try
				{
					CORBA_CALL_RETURN( peerAgentState, m_peerRadioTetraAgent, requestFullState, () );
					successful=true;
					break;
				}
				catch ( CORBA::Exception& e)
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception while receiving full state",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
				}

				retry ++;
			}
            if(!successful)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to recieve full state!");
				return false;
			}
            // no exception, its a success
            /*successful = true;*/
			std::ostringstream data;

			data << std::endl 
				 << "RadioServant --> Entity: " 
				 << peerAgentState->radioServant.entityKey
				 << std::endl;

			for(  unsigned int i = 0; 
				i< peerAgentState->radioServant.radio.length(); 
				i ++ )
			{
				data << "Callstack " << i << ": " 
					<< peerAgentState->radioServant.radio[i].tcpServer.radioCallStack.length()
					<< " --- Control connection "<< i << " is "
					<< static_cast<int>(peerAgentState->radioServant.radio[i].tcpServer.controlConnectionStatus) << std::endl;
			}
	
			for ( unsigned int j =0; j < peerAgentState->radioSessionServants.length(); j++ )
			{
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionServantType type = peerAgentState->radioSessionServants[j];
				data << "RadioSessionServent --> Entity: " << type.entityKey << " "
					 << "SessionName: " << type.radioSession.sessionEntityName << " "
					 << "SessionRef: " << type.radioSession.sessionRef << " "
					 << "SessionStatus: " << static_cast<ESessionState>(type.radioSession.sessionStatus) << std::endl;
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received Full State... %s", data.str().c_str() );

//				// TD9016 Cannot apply the state when session refs received is 0
            state.radioSessionServants.clear();
            
            // convert the corba state to c++ state
            RadioStateMessageConverter::convertRadioTetraAgentCorbaToNormal(peerAgentState, state);
        }
        catch ( TA_Base_Core::TransactiveException& e)
	    {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException while receiving full state", e.what() );
	    }
        catch ( CORBA::Exception& e)
	    {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception while receiving full state",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	    }
	    catch (...)
	    {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while receiving full state" );
	    }

        FUNCTION_EXIT;
        return successful;
    }
    
    bool RadioStateSynchronisationServant::isValid()
    {
        FUNCTION_ENTRY("isValid");
        FUNCTION_EXIT;

        return true;
    }
    
    void RadioStateSynchronisationServant::start()
    {
        FUNCTION_ENTRY("start");

	    activateServantWithName(m_objectName);

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::stop()
    {
        FUNCTION_ENTRY("stop");
        stopSubscription();
	    deactivateServant();

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::update(const class TA_Base_Core::ConfigUpdateDetails& details)
    {
        FUNCTION_ENTRY("update");
        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::remove()
    {
        FUNCTION_ENTRY("remove");
        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::setToControlMode()
    {
        FUNCTION_ENTRY("setToControlMode");

        // TD13975 this function doesn't ever exit, causing the agent to be
        // stuck in "Changing To Control". Temporarily disabled to get through
        // FAT Phase1&2.
        //        return;
        //        FUNCTION_EXIT;

        //limin, for failover
        RadioStateSynchronisationServantThreadedMemberFunction::alloc( *this, &RadioStateSynchronisationServant::setToControlModeInternal );

        FUNCTION_EXIT;
    }


    //limin++, for failover
    void RadioStateSynchronisationServant::setToControlModeInternal()
    {
        FUNCTION_ENTRY( "setToControlModeInternal" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] RadioStateSynchronisationServant::setToControlMode" );

        TA_Base_Core::ThreadGuard guard(m_lockable);

        ServantState oldState = m_state;
        m_state = CONTROL;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Set Monitor Agent STATE = FALSE" );
		m_monAgentState = false; //TD15926

        if (oldState == MONITOR)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Old state in MONITOR" );
            bool acquired;

            // If still acquiring resume until complete or failure, otherwise continue
            if (m_radioFullStateReceiver.isAcquiring())
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] waitForAcquisition" );
                acquired = m_radioFullStateReceiver.waitForAcquisition();
            }
            else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] hasAcquired" );
                acquired = m_radioFullStateReceiver.hasAcquired();
            }
            
            // If full state acquired, finish processing single state updates
            // then notify the radio tetra agent state that it is complete
            if (acquired)
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Full State acquired" );
                stopSubscription();
                m_radioStateUpdateReceiver.waitUntilAllProcessed();
                m_radioStateUpdateReceiver.pauseProcessing();
                m_radioTetraAgentState.notifyStateUpdatesComplete();
            }
            else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Full State NOT acquired" );
                // Otherwise clear all state update messages that have been queued
                // and clear the entire state of the radio tetra agent as though
                // it has just started up in Control Mode from the initial state.
                m_radioFullStateReceiver.stopAcquisition();
                stopSubscription();
                m_radioStateUpdateReceiver.pauseProcessing();
                m_radioStateUpdateReceiver.clearStateUpdateMessages();
                m_radioTetraAgentState.clearFullState();
            }
        }

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::setToMonitorMode()
    {
        FUNCTION_ENTRY("setToMonitorMode");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        ServantState oldState = m_state;
        m_state = MONITOR;

        if (oldState == INITIAL)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[MONITOR] Start from INITIAL State" );
            // Start to receive state update messages from the peer agent
            // that is in Control Mode and acquire that full state
            startSubscription();
            m_radioFullStateReceiver.startAcquisition();
        }
        else if (oldState == CONTROL)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[MONITOR] Start from CONTROL State" );
            // Changing from Control To Monitor mode
            // Full state must be requested from the peer agent
            // as it is not known at this point whether the peer agent has
            // cleared its own state or not.
            m_radioStateUpdateReceiver.clearStateUpdateMessages();
			//clear the state once there is a valid state to apply
            //m_radioTetraAgentState.clearFullState(); 
            startSubscription();
            m_radioFullStateReceiver.startAcquisition();
        }        
		// TD15926
		try
		{
            CORBA_CALL( m_peerRadioTetraAgent, setMonitorAgentState, ( true ) );//limin
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[MONITOR] Unable to notify control agent" );
		}
		// TD15926

        FUNCTION_EXIT;
    }

    void RadioStateSynchronisationServant::startSubscription()
    {
        FUNCTION_ENTRY("startSubscription");

        if ( false == m_subscribed )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Radio Agent subscribing for state updates" );
            // subscribe to receive state update messages from peer agent
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage( 
                TA_Base_Core::RadioTetraAgentStateUpdate::Context,
		        this, TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) );
            m_subscribed = true;
        }           

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::stopSubscription()
    {
        FUNCTION_ENTRY("stopSubscription");

        if ( true == m_subscribed )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "Radio Agent unsubscribing for state updates" );
            // unsubscribe from receiving state update messages from peer agent
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
            m_subscribed = false;
        }

        FUNCTION_EXIT;
    }

	//TD15926
	bool RadioStateSynchronisationServant::isPeerAvailable()
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);

		if (m_state == MONITOR)
		{
			try
			{
				// TD15926 notify the controlling agent that 'I' am terminating
                CORBA_CALL( m_peerRadioTetraAgent, setMonitorAgentState, ( false ) );//limin
				return true; // monitor agent cannot detach session
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[MONITOR] Unable to notify control agent" );
				return false;
			}
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] isPeerAvailable = %d",m_monAgentState );		
		return m_monAgentState;
	}

	////////////////////////////////////////////////////////////////////////////////
	// *The methods below are added for TD9016 

	/* 
	 * This IDL method implementation is used to notify the agent(control) that the monitor agent is active.
	 */
	void RadioStateSynchronisationServant::setMonitorAgentState(bool state)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);
		TA_Base_Bus::GenericAgent::ensureControlMode();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Set Monitor Agent STATE = %d", state );
		m_monAgentState = state;
	}


	/** 
	 * This function is called when the monitoring agent has retrieved
	 * the full state from the controlling agent
	 */
	void RadioStateSynchronisationServant::applyFullState(const RadioTetraAgentType& state)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);
        std::vector<RadioSessionServant*> radioSessionServants = m_radioTetraAgentState.getRadioSessionServants();

		m_radioTetraAgentState.clearFullState(); 

        // apply the state on the radio session servant entities
        for ( RadioSessionServantTypeList::const_iterator sessionIt = state.radioSessionServants.begin();
        sessionIt != state.radioSessionServants.end(); ++sessionIt)
        {
            // find the entity with the entity key
            for (std::vector<RadioSessionServant*>::iterator rIt = radioSessionServants.begin();
            rIt != radioSessionServants.end(); rIt++)
            {
                if ( sessionIt->entityKey == (*rIt)->getEntityKey())
                {
                    (*rIt)->applyFullState(*sessionIt);
                    break;
                }
            }
        }

        m_radioTetraAgentState.getRadioServant()->applyFullState(state.radioServant);
	}

	void RadioStateSynchronisationServant::getFullState(RadioTetraAgentType& state)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->getFullState" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

        std::vector<RadioSessionServant*> radioSessionServants = m_radioTetraAgentState.getRadioSessionServants();
        m_radioTetraAgentState.getRadioServant()->getFullState(state.radioServant);

		// get the state of the radio session servant entities
		for (std::vector<RadioSessionServant*>::iterator rIt = radioSessionServants.begin(); 
			  rIt != radioSessionServants.end(); rIt++)
		{
			RadioSessionServantType radioSessionServantData;
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioSessionServant->getFullState");
			(*rIt)->getFullState(radioSessionServantData);
			state.radioSessionServants.push_back(radioSessionServantData);			
		}
	}
	//* TD9016

};  // namespace TA_IRS_App
