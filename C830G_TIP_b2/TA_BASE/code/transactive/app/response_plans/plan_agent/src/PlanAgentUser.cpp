/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#include "core/threads/src/Thread.h"
#include "core/threads/src/ThreadMemberFunction.h"
#include "core/exceptions/src/CorbaException.h" //like++. for "TA_THROW_CORBA"

//++limin, failover
const int DEFAULT_ACTIVEPLAN_EXPIRYTIME = 3; // 3 Minutes

namespace TA_Base_App
{

    typedef TA_Base_Core::ThreadedMemberFunction< TA_Base_App::PlanAgentUser > AgentThreadedMemberFunction;

	//
	// Static member variables
	//
	PlanAgentUser* PlanAgentUser::m_instance	= NULL;
	
	PlanAgentUser::PlanAgentUser()
		:m_agent(NULL), 
		m_genericAgent(NULL),
        m_agentControlMode(false),
        m_contorlDataSynced(false),
        m_bFailToSyncState(false),
        m_synMonitorInitialization(0),
		m_entityKey(0),
		m_entityTypeKey(0),
		m_subsystemKey(0),
        m_locationKey(0),
		m_assetName(""),
		m_activePlanExpiryTime(0),
		m_activePlanThreadPoolSize(0),
		m_pausedPlanSleepTime(10000) //microsecs	
	{
	}

	void PlanAgentUser::initAgentUser( int argc, char* argv[] )
	{
		// the plan agent must be constructed before the generic agent
		m_agent = new TA_Base_App::PlanAgentCorbaDefImpl(this);
		m_genericAgent = new TA_Base_Bus::GenericAgent( argc, argv, this, NULL, AGENT_USER_TYPE_SCADA );
	
		initialisePlanAgentConfiguration();
	}

	PlanAgentUser::~PlanAgentUser()
	{
        

		if ( NULL != m_agent )
		{
			m_agent->deactivateAndDeleteServant();
		}

		// must be deleted here and not in agentTerminate
		if (  NULL != m_genericAgent )
		{
			m_genericAgent->deactivateAndDeleteServant();
		}
	}


	void PlanAgentUser::run( void )
	{
		TA_ASSERT( NULL != m_agent, "Plan Agent is NULL" );
		TA_ASSERT( NULL != m_genericAgent, "Generic Agent is NULL" );
		TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENTITYNAME), "--entity-name not set" );
		
		m_agent->activateServantWithName(getAgentEntityName());
			//TA_Base_Core::Naming::registerName( m_agent->_this(), getAgentEntityName() );

		m_genericAgent->run();
	}

	void PlanAgentUser::agentTerminate()
	{
		TA_ASSERT( NULL != m_agent, "Plan Agent is NULL" );

		// Wait for any running plans to end. All plans will run to
		// completion before this method returns.
		PlanExecutor::instance()->waitForRunningPlans();
		
		// stop accepting new connections
		//	TA_Base_Core::Naming::removeNameRegistration( getAgentEntityName() );

		// Need to clean up the message senders before the
		// StructuredEventSupplier is ripped out from underneath us
		// by GenericAgent. Otherwise we would normally leave the
		// ActivePlanBulletinBoard singleton to do it's own cleaning up...
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        ActivePlanBulletinBoard::instance()->deleteMessageSenders();
        AAMessageSender::instance()->deleteMessageSenders();

		//remove subscriber to datapoint updates
		TA_Base_App::DataPointUpdateStateSubscriber::getInstance().unsubscribeToMessages ();
		TA_Base_App::DataPointUpdateStateSubscriber::destroyInstance (); 	
		
		m_agent->removeNamedObjectRegistration();
		m_agent->deactivateAndDeleteServant();
		m_agent = NULL;
	}

	TA_Base_Bus::IEntity* PlanAgentUser::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		return NULL;
    }

    void PlanAgentUser::initialActivePlanStateFromControlAgent()
    {
        // Clear semaphore, only wait after state update request sent
        while ( m_synMonitorInitialization.tryWait() )
        {
            // Do nothing
        }

        m_contorlDataSynced = false;
        m_bFailToSyncState = false;

        const int ADVANCE_RETRY_TIMES = 30;
        const unsigned long SYNC_DATA_FROM_CONTROL_TIMEOUT = 1000; // milliseconds
        const unsigned long ADVANCE_SYNC_DATA_TIMEOUT = 10000; // milliseconds
        int nRetry = 0;
        unsigned long ulInterval = SYNC_DATA_FROM_CONTROL_TIMEOUT;

        // Wait for data synchronized from Control Mode Agent
        while ( !m_contorlDataSynced )
        {
            if ( m_agentControlMode )
            {
                break;
            }

            ActivePlanBulletinBoard::instance()->sendStateUpdateRequestToControlAgent();

            if ( ADVANCE_RETRY_TIMES < nRetry )
            {
                LOG_GENERIC(SOURCE_INFO, INFO_LOG_LEVEL, "-Sync-: No update after retry %d times", nRetry );
                // Advance strategy, wait a much longer time
                ulInterval = ADVANCE_SYNC_DATA_TIMEOUT;
                m_bFailToSyncState = true;
            }
            ++nRetry;

            if ( m_synMonitorInitialization.timedWait( ulInterval ))
            {
                LOG_GENERIC(SOURCE_INFO, INFO_LOG_LEVEL, "-Sync-: Semaphore is signaled" );
                nRetry = 0;
                m_bFailToSyncState = false;
                ulInterval = SYNC_DATA_FROM_CONTROL_TIMEOUT;
            }
        }
    }
	
	void PlanAgentUser::agentSetMonitor()
	{
        if ( !m_agentControlMode )
        {
            return;
        }

		m_agentControlMode = false;

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

//         ActivePlanBulletinBoard::instance()->cullAllPlans();
        
//         registerForMonitorMessages();

        ActivePlanBulletinBoard::instance()->setControlMode( false );

        // Call new clear functions for fast cleaning and less error
        PlanExecutor::instance()->clearAllPlans();

        ActivePlanBulletinBoard::instance()->clearAllPlanStates();

        registerForMonitorMessages();

        AgentThreadedMemberFunction& thrSyncDataFromControl = AgentThreadedMemberFunction::alloc( *this );
        thrSyncDataFromControl.add( &PlanAgentUser::initialActivePlanStateFromControlAgent );
        thrSyncDataFromControl.start();
	}

	void PlanAgentUser::agentSetControl()
	{
		if ( m_agentControlMode )
        {
            return;
        }

		m_agentControlMode = true;

        // Wakeup and terminate StateUpdate synchronization thread
        m_synMonitorInitialization.post();

		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        registerForControlMessages();

        ActivePlanBulletinBoard::instance()->setControlMode( true );

        PlanExecutor::instance()->signalAllPlansIntoControl();
	}


	void PlanAgentUser::registerForMonitorMessages()
	{
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate, this,
			getAgentEntityName() );

			LOG( SourceInfo, GENERIC_LOG_TYPE, INFO_LOG_LEVEL,
				 "Monitoring Plan Agent subscribed to StateUpdateMessage");
	}

	void PlanAgentUser::registerForControlMessages()
	{
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest, this,
			getAgentEntityName() );

			LOG( SourceInfo, GENERIC_LOG_TYPE, INFO_LOG_LEVEL,
				 "Control Plan Agent subscribed to PlanAgentStateUpdateRequest");
	}

    void PlanAgentUser::notifyGroupOffline( const std::string& group )
	{
	}

    void PlanAgentUser::notifyGroupOnline( const std::string& group )
	{
	}

	void PlanAgentUser::registerForStateUpdates()
	{
        // No need to subscript message here, agentSetControl and agentSetMonitor will do
// 		if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
// 		{
//             //limin++--, failover
//             AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
//             thrd.add( &PlanAgentUser::registerForControlMessages );
//             thrd.start();
// 			//registerForControlMessages();
//             //limin++--, failover
// 		}
// 		else if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
// 		{
//             //limin++--, failover
//             AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
//             thrd.add( &PlanAgentUser::registerForMonitorMessages );
//             thrd.start();
// 			//registerForMonitorMessages();
//             //limin++--, failover
// 		}
	}

    void PlanAgentUser::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
        unsigned long updateEventPKey = updateEvent.getKey();

        if ( m_entityKey != updateEventPKey ) return;

	    updatePlanAgentConfiguration ( updateEvent );

	    return;
	}


    void PlanAgentUser::updatePlanAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        // get configuration data for the Agent
        TA_Base_Core::PlanAgentDataPtr entityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PlanAgentData>
            ( m_genericAgent->getAgentEntityConfiguration() );

        if ( entityData.get() == NULL ) return;

        // force a Re-read
        entityData->invalidate();

	    const std::vector< std::string > changes = updateEvent.getChangedParams();

        std::vector< std::string >::const_iterator it;
        
        for ( it = changes.begin(); it != changes.end(); it++ )
        {
            if ( TA_Base_Core::PlanAgentData::ACTIVEPLAN_EXPIRYTIME.compare(*it) == 0)
            {
                m_activePlanExpiryTime = 60 * (entityData->getActivePlanExpiryTime());
            }
				
            if ( TA_Base_Core::PlanAgentData::ACTIVEPLAN_PAUSED_SLEEP_TIME.compare(*it) == 0)
            {
                m_pausedPlanSleepTime = entityData->getActivePlanPausedSleepTime();
            }

        }        
    }


    void PlanAgentUser::initialisePlanAgentConfiguration()
    {

        TA_ASSERT( m_agent != NULL, "Generic Agent has not been intitialised.");
        TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

        if (entityData.get() == NULL)
        {
			LOG( SourceInfo, GENERIC_LOG_TYPE, FATAL_LOG_LEVEL,
				 "IEntityData* entityData = m_genericAgent->getAgentEntityConfiguration() returned NULL");
            //Throw exception
            return;
        }

		TA_Base_Core::PlanAgentDataPtr agentConfig = 
			boost::dynamic_pointer_cast<TA_Base_Core::PlanAgentData>(entityData);

        if (agentConfig.get() == NULL)
        {
			LOG( SourceInfo, GENERIC_LOG_TYPE, FATAL_LOG_LEVEL,
				 "dynamic_cast <PlanAgentData*> ( entityData ) returned NULL");

            //Throw exception
            return;
        }
		//
		// Need these keys for the MessageSender's
		//
		m_entityKey     = agentConfig->getKey();
		m_subsystemKey  = agentConfig->getSubsystem();
		m_entityTypeKey = agentConfig->getTypeKey();
		m_agentEnityName = agentConfig->getName();
		m_locationKey = agentConfig->getLocation();
		m_assetName = agentConfig->getAssetName();

        // Maximum time active plan instances are kept in the list after they finish execution.
        // Stored in agent configuration as minutes, converted to and used as seconds.
        m_activePlanExpiryTime = 60 * (agentConfig->getActivePlanExpiryTime() );

        if ( m_activePlanExpiryTime == 0 )
        {
            m_activePlanExpiryTime = DEFAULT_ACTIVEPLAN_EXPIRYTIME;
        }

        // TD10997: Number of thread servicing plan execution requests. Defaults to 100 if not configured.
        m_activePlanThreadPoolSize = agentConfig->getActivePlanThreadPoolSize();

		  //TD 13527 number of microsecs operator-response timed out plans and
		  //interactive plans waiting for operator response go to sleep for
		  m_pausedPlanSleepTime = agentConfig->getActivePlanPausedSleepTime ();

		//Initialise the Agent Mode
		std::string mode ( "" );
		mode = TA_Base_Core::RunParams::getInstance().get ( "OperationMode" );

		// if defined as "Control"
		if ( RPARAM_CONTROL == mode )
		{
			m_agentControlMode = false;
			agentSetControl();
		}
		else if ( RPARAM_MONITOR == mode )
		{
			m_agentControlMode = true;
			agentSetMonitor();
		}

        //Update the plans_seq
        //Keep it commented during development
        //PlanDataWriter::instance()->UpdatePlansDBSequence();
    }

	void PlanAgentUser::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
        static TA_Base_Core::UpdateId lastPasUpdateId = 0;
        static TA_Base_Core::UpdateId lastRequestId = 0;

		TA_Base_Core::PASUpdateRequest* request;
		if ( m_agentControlMode )
		{
            if ( 0 == TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest.getTypeKey().compare(message.messageTypeKey) )
            {
			    if ( !(message.stateUpdateInfo >>= request) )
                {
                    LOG_GENERIC( SourceInfo, WARN_LOG_LEVEL, "-Msg-: Unknown message received in Control mode. Ignored" );
				    return;
                }

			    //Valid request from Monitoring Agent
                TA_Base_Core::UpdateId receivedRequestId = request->pasRequestId;
                if ( receivedRequestId <= lastRequestId )
                {
                    LOG_GENERIC( SOURCE_INFO, WARN_LOG_LEVEL, "-Msg-: Duplicate Request from Monitor PlanAgent received with requestId=%ul.", receivedRequestId);
                    return;
                }

                lastRequestId = receivedRequestId;

                LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Msg-: Request for StateUpdates received from Monitor PlanAgent.");

                // Always catch everything when receiveSpecialisedMessage
                try
                {
			        ActivePlanBulletinBoard::instance()->sendAllStateUpdatesToMonitorAgent();
                }
	            catch( const std::exception& e )
	            {
                    LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "-Msg-: Caught std::exception when send all state to monitor agent : %s: ",  e.what() );
                }
                catch( const CORBA::Exception& e )
                {
                    LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "-Msg-: Caught CORBA::Exception when send all state to monitor agent : %s: ", 
                        TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
	            }
                catch (...)
                {
                    LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "-Msg-: Caught unknown exception when send all state to monitor agent" );
                }

                LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "-Msg-: send All State Updates To Monitor Agent done.");
			    return;
            }

            return;
		}
        else
        {
		    // Here means the Agent is running in Monitor mode
		    TA_Base_Core::PASUpdateData* updateData;
            TA_Base_Core::PASUpdateDataSeq* updateDataSeq;

		    if ( (message.stateUpdateInfo >>= updateData) )
            {
                if ( !m_contorlDataSynced )
                {
                    if ( m_bFailToSyncState )
                    {
                        m_synMonitorInitialization.post();
                    }
                    LOG_GENERIC( SourceInfo, WARN_LOG_LEVEL, "-Msg-: Received state update message when Monitor mode data not ready, ignored" );
                    return;
                }
                handlePASUpdateData( updateData, lastPasUpdateId );
                return;
            }
            // All states update message only get when agent is set to monitor mode
            else if ( (message.stateUpdateInfo >>= updateDataSeq) )
            {
                // m_contorlDataSynced is thread safe since receiveSpecialisedMessage is call in one thread
                if ( m_contorlDataSynced )
                {
                    LOG_GENERIC( SourceInfo, WARN_LOG_LEVEL, "-Msg-: All state update message already received, ignored" );
                    return;
                }
                // Can set to true here since the function is called serially in one thread.
                m_contorlDataSynced = true;
                m_synMonitorInitialization.post();
                LOG_GENERIC( SourceInfo, INFO_LOG_LEVEL, "-Msg-: Received all state update message" );
                for ( int nSeqLoop = 0; nSeqLoop < updateDataSeq->length(); ++nSeqLoop )
                {
                    handlePASUpdateData( &((*updateDataSeq)[nSeqLoop]), lastPasUpdateId );
                }
                LOG_GENERIC( SourceInfo, INFO_LOG_LEVEL, "-Msg-: process all state update message done" );
                return;
            }
            else
            {
                LOG_GENERIC( SourceInfo, WARN_LOG_LEVEL, "-Msg-: Unknown message received in Monitor mode. Ignored" );
			    return;
            }
        }
	}

    void PlanAgentUser::handlePASUpdateData( TA_Base_Core::PASUpdateData* updateData, TA_Base_Core::UpdateId& lastPasUpdateId )
    {
        TA_Base_Core::UpdateId receivedPASUpdateId = updateData->pasUpdateId;

        LOG_GENERIC( SourceInfo, INFO_LOG_LEVEL, "-Msg-: PAS Update message received ( update id: %u ), ( type: %u )", updateData->pasUpdateId, updateData->update._d() );

        if ( receivedPASUpdateId == lastPasUpdateId )
        {            
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "-Msg-: Duplicate StateUpdates received ( update id: %u )", receivedPASUpdateId);
            return;
        }
        else if ( receivedPASUpdateId < lastPasUpdateId )
        {
            if ( receivedPASUpdateId != TA_Base_Core::MIN_UPDATE_ID )
            {
                LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "-Msg-: StateUpdates ID rolled over to incorrect value: %u", receivedPASUpdateId);
                return;
            }
        }

        lastPasUpdateId = receivedPASUpdateId;

        ActivePlanBulletinBoard::instance()->setPasUpdateId(receivedPASUpdateId);

        try
        {
		    switch ( updateData->update._d() )
		    {
			    case TA_Base_Core::PASUT_PLAN_EXECUTION_HISTORY:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: ExecutionHistory. Number:%u", receivedPASUpdateId);
					    TA_Base_Core::ExecutionHistoryUpdate* historyUpdate = &(updateData->update.executionHistory());
                        ActivePlanBulletinBoard::instance()->processExecutionHistoryUpdate( *historyUpdate );
				    }
				    break;
				    
			    case TA_Base_Core::PASUT_PLAN_CUSTOMISATION_HISTORY:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: CustomisationHistory. Number:%u", receivedPASUpdateId);
					    TA_Base_Core::CustomisationHistoryUpdate* customisationUpdate = &(updateData->update.customisationHistory());
                        ActivePlanBulletinBoard::instance()->processCustomisationHistoryUpdate( *customisationUpdate );
                        PlanExecutor::instance()->processCustomisationHistoryUpdate( *customisationUpdate );
				    }
				    break;


			    case TA_Base_Core::PASUT_REGISTERED_PLAN_MANAGER:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: PM registration. Number:%u", receivedPASUpdateId);
					    TA_Base_Core::RegisterPlanManagerUpdate* registerUpdate = &(updateData->update.planManager());
					    ActivePlanBulletinBoard::instance()->registerPlanManager( registerUpdate->session.in(), registerUpdate->pmId.in(), registerUpdate->planManager);
				    }
				    break;

			    case TA_Base_Core::PASUT_DEREGISTERED_PLAN_MANAGER:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: PM deregistration. Number:%u", receivedPASUpdateId);
                        TA_Base_Core::DeregisterPlanManagerUpdate* deregUpdate = &(updateData->update.deregisterPlanManager());
					    std::string session = deregUpdate->session.in();
                        std::string pmId = deregUpdate->pmId.in();
					    ActivePlanBulletinBoard::instance()->deregisterPlanManager( session, pmId);
				    }
				    break;

                case TA_Base_Core::PASUT_TAKE_CONTROL:
                    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: TakeControl. Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanStateUpdate tku = updateData->update.takeControl();

                        PlanExecutor::instance()->reassign(tku.session.in(), tku.pmId.in(), tku.plan);
                    }
                    break;

			    case TA_Base_Core::PASUT_RUN_PLAN:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: runPlan(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanStateUpdate* rpsu = &(updateData->update.runPlanUpdate());

                        if ( ActivePlanBulletinBoard::instance()->isActive( rpsu->plan ) ) 
						{
							return;
						}
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: runPlan(). Number:%u, plan is not active!", receivedPASUpdateId);
					    PlanExecutor::instance()->run( rpsu->session.in(), rpsu->pmId.in(), rpsu->plan, rpsu->manuExec, true );
				    }
				    break;

			    case TA_Base_Core::PASUT_CUSTOMISE_PLAN:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: customisePlan(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanStateUpdate* rpsu = &(updateData->update.customisePlanUpdate());	
					    PlanExecutor::instance()->customise( rpsu->session.in(), rpsu->pmId.in(), rpsu->plan, true );

				    }
				    break;

			    case TA_Base_Core::PASUT_PAUSE_PLAN:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: pausePlan(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanStateUpdate* rpsu = &(updateData->update.pausePlan());
					    PlanExecutor::instance()->pause(rpsu->plan);
				    }
				    break;

			    case TA_Base_Core::PASUT_RESUME_PLAN:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: resumePlan(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanStateUpdate* rpsu = &(updateData->update.resumePlan());
                        //TODO: fix resume
					    PlanExecutor::instance()->resume(rpsu->plan);
				    }
				    break;

			    case TA_Base_Core::PASUT_STOP_PLAN:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: stopPlan(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanStateUpdate* rpsu = &(updateData->update.stopPlan());
                        std::string reason = rpsu->remark.in();
					    PlanExecutor::instance()->stop(rpsu->plan, reason);
				    }
				    break;

			    case TA_Base_Core::PASUT_CUSTOMISE_STEP:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: customiseStep(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::PlanCustomiseStepUpdate* pcsu = &(updateData->update.customisedStep());
					    PlanExecutor::instance()->customiseStep(pcsu->plan, pcsu->detail);
				    }
				    break;

			    case TA_Base_Core::PASUT_GET_EDIT_PLAN_LOCK:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: getEditLock(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::EditLockUpdate* elu = &(updateData->update.getLockUpdate());
					    PlanDataWriter::instance()->getEditPlanLock(elu->session.in(), elu->pmId.in(), elu->plan, true);
				    }
				    break;

			    case TA_Base_Core::PASUT_RELEASE_EDIT_PLAN_LOCK:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: releaseEditLock(). Number:%u", receivedPASUpdateId);
					    TA_Base_Core::EditLockUpdate* elu = &(updateData->update.releaseLockUpdate());
					    PlanDataWriter::instance()->releaseEditPlanLock(elu->session.in(), elu->plan, true);
				    }
				    break;

			    case TA_Base_Core::PASUT_ACTIVE_PLAN_UPDATE:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: activePlanUpdate(). Number:%u", receivedPASUpdateId);
                        TA_Base_Core::ActivePlanDetail apu = updateData->update.planDetail();
                        PlanExecutor::instance()->updateActivePlanDetail(apu);
				    }
				    break;

                case TA_Base_Core::PASUT_UPDATE_ID:
                    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: updateId. Number:%u", receivedPASUpdateId);
                        TA_Base_Core::UpdateId id = updateData->update.activeUpdateId();					
					    ActivePlanBulletinBoard::instance()->setUpdateId(id);
                    }
                    break;

			    case TA_Base_Core::PASUT_PAUSED_FOR_OPERATOR_UPDATE:
				    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: OperatorPausedState. Number:%u", receivedPASUpdateId);
					    TA_Base_Core::ActivePlanDetail apd = updateData->update.operatorPausedPlanDetail();
                        PlanExecutor::instance()->pauseForOperator( apd.plan );
                    }
				    break;

                case TA_Base_Core::PASUT_ACTIVE_PLANS:
                    {
                        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "StateUpdate: PASUT_ACTIVE_PLANS. Number:%u", receivedPASUpdateId);
                        TA_Base_Core::ActivePlanDetails apds = updateData->update.planDetails();
                        int nPlans = apds.length();
                        for ( int i = 0; i < nPlans; i++)
                        {
                            PlanExecutor::instance()->updateActivePlanDetail(apds[i]);
                        }
                    }

                default:
                    LOG_GENERIC( SOURCE_INFO, WARN_LOG_LEVEL, "-Msg-: Error: StateUpdate of unknown type received.");
                    break;
		    }
        }
	    catch( const std::exception& e )
	    {
		    LOG(SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Failed to process state update.");
	    }
	    catch( const CORBA::Exception& e )
	    {
		    LOG(SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Failed to process state update.");
	    }
        catch( ... )
	    {
		    LOG(SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Failed to process state update." );
	    }
    }

	void PlanAgentUser::checkOperationMode()
	{
		TA_ASSERT(this->m_genericAgent != NULL, "m_genericAgent should not be null");

		if (m_genericAgent->getOperationMode() == TA_Base_Core::Monitor)
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException(RPARAM_MONITOR));
		}
	}

} // TA_Base_App
