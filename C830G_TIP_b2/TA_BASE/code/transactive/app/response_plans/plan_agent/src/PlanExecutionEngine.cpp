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
  *  Implementation of PlanExecutionEngine.
  */

#include "PlanAgent.h"

namespace TA_Base_App
{

    const ACE_Message_Block::Message_Flags ACE_EVENT_SET_CONTROL = ACE_Message_Block::USER_FLAGS + 0x1;

	PlanExecutionEngine::PlanExecutionEngine() :
        m_activationQueue(),
		//m_aqTimeout(0,10000),
		m_mqTimeout(0,10000),
        m_mqMap(),
        m_mqMapLock(),
        m_consolidateMqLock()
	{
	}

	PlanExecutionEngine::~PlanExecutionEngine()
	{
        // MB_HANGUP is not be used any more - jian.huang
		// there should be one MB_HANGUP message left
// 		ACE_Message_Block* aceMsg;
// 		getq(aceMsg);
// 		aceMsg->release();

        if ( 0 != thr_count() )
        {
            terminateAndWait();
        }

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);

		    PlanMqMapIterator it;

            // User smart pointer to avoid object usage after deleting
		    for( it = m_mqMap.begin(); it != m_mqMap.end(); ++it )
		    {
                it->second->abandon();
// 			    delete (*it).second;
		    }

            m_mqMap.clear();
        }
	}

	int PlanExecutionEngine::open( void* )
	{
        if (( m_activationQueue.queue() != NULL ) && 
            ( m_activationQueue.queue()->state() == ACE_Message_Queue_Base::DEACTIVATED ))
        {
            m_activationQueue.queue()->activate();
            LOG_GENERIC(SOURCE_INFO, INFO_LOG_LEVEL, "-Perf-: Reactive activation queue successfully");
        }

        // PlanHouseKeeping enqueue
        m_activationQueue.enqueue( new PlanHouseKeepingMethodObject( *this ));

        // TD10997: The size of the plan execution thread pool is now configurable.
		return activate(THR_JOINABLE, PlanAgentUser::getInstance()->getActivePlanThreadPoolSize());
		//return activate(THR_JOINABLE, 1);
	}

	int PlanExecutionEngine::close( u_long flags /*=0*/ )
	{
		return 0;
	}

	int PlanExecutionEngine::svc( void )
	{
		try
		{
			while( true )
			{
				// check the activation queue
				//ACE_Time_Value aqAbsTime( ACE_OS::gettimeofday() + m_aqTimeout );
				//std::auto_ptr<ACE_Method_Object> mo(m_activationQueue.dequeue(&aqAbsTime));

                // Blocking dequeue
                //std::auto_ptr<ACE_Method_Object> mo(m_activationQueue.dequeue()); // jian.huang
				std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue());

				// No method to execute.
				if( 0 == mo.get() )
				{
					// Check the activation queue is OK.
// 					if ( EWOULDBLOCK != ACE_OS::last_error() )
// 					{
// 						std::ostringstream what;
// 						what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
// 						TA_THROW( ThirdPartyLibException( what.str() ) );
// 					}
                    
                    // jian.huang
                    // If m_activationQueue is deactivated, ACE_OS::last_error() should return ESHUTDOWN
                    // so all threads which are blocked at m_activationQueue.dequeue() should break out of the loop
                    // and this will instead of original getq method to notify idling threads to exit the loop
                    if ( ESHUTDOWN == ACE_OS::last_error() )
                    {
                        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "Idle thread receive hang up signal" );
                        break;
                    }
                    else
                    {
                        std::ostringstream what;
                        what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
                        LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, what.str().c_str() );
                        break;
                    }
					
                    // No need to get msg from message queue
// 					ACE_Message_Block* aceMsg;
// 					ACE_Time_Value mqAbsTime( ACE_OS::gettimeofday() + m_mqTimeout );
// 
// 					if( -1 == getq(aceMsg, &mqAbsTime) )
// 					{
// 						if ( EWOULDBLOCK != ACE_OS::last_error() )
// 						{
// 							std::ostringstream what;
// 							what << "ACE_Task::getq failed with errno " << ACE_OS::last_error();
// 							TA_THROW( ThirdPartyLibException( what.str() ) );
// 						}
// 					}
// 					else
// 					{
// 						if( ACE_Message_Block::MB_HANGUP == aceMsg->msg_type() )
// 						{
// 							LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received MB_HANGUP message." );
// 							put( aceMsg );
// 							ActivePlanBulletinBoard::instance()->setHangupMessageReceived();
// 							break;
// 						}
// 					}
                }
				else // Method to execute
				{
					// If we get to here we've got an object on the activation queue
                    try
                    {
                        if( -1 == mo->call() )
                        {
                            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Method call error." );
                            break;
                        }
                    }
                    catch (...) 
                    {
                        LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Unhandled when runPlanExecutionEngine::svc" );
                    }
				}

                // clean up work will be done in a single thread in PlanHouseKeepingMethodObject
//                 try
//                 {
//                     //as per TD 7183 
//                     ActivePlanBulletinBoard::instance()->pauseExpiredInteractivePlans ();
//                     
//                     // periodically cull stopped plan instances from
//                     // the bulletin board
//                     ActivePlanBulletinBoard::instance()->cullStoppedPlans();
//                     
//                     // Peridically clean up the message queue.
//                     consolidateMq();
//                 }
//                 catch( const std::exception& e )
//                 {
//                     LOG_EXCEPTION_CATCH( SOURCE_INFO, typeid(e).name(), e.what() );
//                 }
//                 catch( ... )
//                 {
//                     LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Unhandled." );
//                 }
			}
		}
		catch( const std::exception& e )
		{
			LOG_EXCEPTION_CATCH( SOURCE_INFO, typeid(e).name(), e.what() );
// 			TA_ASSERT( false, "Unhandled exception." );
		}
		catch( const CORBA::Exception& e )
		{
			LOG_EXCEPTION_CATCH( SOURCE_INFO, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
// 			TA_ASSERT( false, "Unhandled exception." );
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Unhandled." );
// 			TA_ASSERT( false, "Unhandled exception." );
		}

		return 0;
	}


    ACE_Future<TA_Base_Core::EActivePlanState> PlanExecutionEngine::run( 
        const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, 
        TA_Base_Core::EActivePlanState initialState, const bool manExecution, const bool bStartFromMonitorMode )
	{
		ACE_Future<TA_Base_Core::EActivePlanState> rval;

		// First we need to reserve an instance because we have to return an instance to 
		// the caller synchronously so that they can refer to this plan in subsequent
		// messages.

		ActivePlanBulletinBoard::instance()->reserveInstance( session, pmId, plan, initialState, rval , manExecution);

		// Turn the plan into an "Active" plan. The plan is newed here and is
		// passed around as an auto_ptr so ownership is constantly passed on
		// until the final method/object that holds ownership is destroyed.

        try
        {
// 		    std::auto_ptr<ActivePlan> activePlan( new ActivePlan( plan, initialState ) );

		    // Make a message queue for this plan.
		    makeMq( plan );

		    // Put it in the list of plans to be run.
// 		    m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
            m_activationQueue.enqueue( new RunPlanMethodObject( *this, session, pmId, plan, initialState, manExecution, bStartFromMonitorMode ));
        }
        catch ( const TA_Base_Core::PlanAgentException& ex)
        {
            ActivePlanBulletinBoard::instance()->undoReserveInstance( plan );
			TA_THROW(ex);
        }

        // Return a copy of the future to the caller so they can wait on the
        // result.
        
		return rval;
    }

    void PlanExecutionEngine::publishPlanStart( const std::string& session, const std::string& pmId, 
        const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EActivePlanState initialState, const bool manExecution )
    {
        // No update should be send before RunPlan or CustomisePlan is update in Monitor Agent
//        ActivePlanBulletinBoard::instance()->sendUpdateMessage( plan );

        std::ostringstream auditMessage;

        TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(plan.plan);

        //Send State Update message for Taking control
        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

        TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
        psu->session = CORBA::string_dup(session.c_str());
        psu->pmId = CORBA::string_dup(pmId.c_str());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
        psu->plan = plan;
        psu->manuExec = manExecution;

        if ( TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE == initialState )
        {
            pasUpdateData->update.runPlanUpdate(psu);
            if (ActivePlanBulletinBoard::instance()->isGRCPlan(plan))
            {
                if (ActivePlanBulletinBoard::instance()->isManualGRC(plan))
                {
                    TA_Base_Core::DescriptionParameters descriptionParameters;
                    TA_Base_Core::NameValuePair pair( "GRC", "Started Manually");
                    descriptionParameters.push_back( &pair );
                    
                    //TD14802++
                    //Add dummy pair to set Value column in Event Viewer to "STARTED"
                    TA_Base_Core::NameValuePair pair2( "DataPointValue", "STARTED");
                    descriptionParameters.push_back( &pair2 );
                    //++TD14802
                    
                    //CL-16389++
                    TA_Base_Core::NameValuePair pair3( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan ));
                    descriptionParameters.push_back( &pair3 );
                    
                    std::ostringstream tempStrm;
                    tempStrm << plan.instance;
                    TA_Base_Core::NameValuePair pair4( "InstanceNumber", tempStrm.str());
                    descriptionParameters.push_back( &pair4 );
                    //++CL-16389
                    
                    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanManualGRCStarted, descriptionParameters);	
                }
                else//automatic GRC
                {
                    TA_Base_Core::DescriptionParameters descriptionParameters;
                    TA_Base_Core::NameValuePair pair( "GRC", "Started Automatically");
                    descriptionParameters.push_back( &pair );
                    
                    //TD14802++
                    //Add dummy pair to set Value column in Event Viewer to "STARTED"
                    TA_Base_Core::NameValuePair pair2( "DataPointValue", "STARTED");
                    descriptionParameters.push_back( &pair2 );
                    //++TD14802
                    
                    //CL-18369++
                    TA_Base_Core::NameValuePair pair3( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan ));
                    descriptionParameters.push_back( &pair3 );
                    
                    std::ostringstream tempStrm;
                    tempStrm << plan.instance;
                    TA_Base_Core::NameValuePair pair4( "InstanceNumber", tempStrm.str());
                    descriptionParameters.push_back( &pair4 );
                    //++CL-18369
                    
                    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanAutoGRCStarted, descriptionParameters);	
                }
            }
            else//DSS plans
            {
                TA_Base_Core::DescriptionParameters descriptionParameters;
                
                //xufeng++ TD14927
                TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
                descriptionParameters.push_back( &pair0 );
                //++xufeng TD14927
                
                TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan ));
                descriptionParameters.push_back( &pair1 );
                
                std::ostringstream tempStrm;
                tempStrm << plan.instance;
                TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
                descriptionParameters.push_back( &pair2 );
                
                AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanExecution, descriptionParameters);
            }
        }
        else
        {
            TA_Base_Core::DescriptionParameters descriptionParameters;
            
            //xufeng++ TD14927
            TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
            descriptionParameters.push_back ( &pair0 );
            //++xufeng TD14927
            
            TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan ));
            descriptionParameters.push_back( &pair1 );
            
            std::ostringstream tempStrm;
            tempStrm << plan.instance;
            TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
            descriptionParameters.push_back( &pair2 );
            pasUpdateData->update.customisePlanUpdate(psu);
            AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanPending, descriptionParameters);
        }
        
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
        
        // Send update to Plan Manager although useless update will send to Monitor Agent too
        ActivePlanBulletinBoard::instance()->sendUpdateMessage( plan );
    }


    ACE_Future<TA_Base_Core::EActivePlanState> PlanExecutionEngine::spawn( 
        const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, 
        TA_Base_Core::ActivePlanId& childPlan, TA_Base_Core::EActivePlanState initialState, const bool bStartFromMonitorMode )
	{
		ACE_Future<TA_Base_Core::EActivePlanState> rval;

		// First we need to reserve an instance because we have to return an instance to 
		// the caller synchronously so that they can refer to this plan in subsequent
		// messages.

		ActivePlanBulletinBoard::instance()->reserveInstance( session, pmId, parentPlan, childPlan, initialState, rval);
		// Turn the plan into an "Active" plan. The plan is newed here and is
		// passed around as an auto_ptr so ownership is constantly passed on
		// until the final method/object that holds ownership is destroyed.

        try
        {
// 		    std::auto_ptr<ActivePlan> activePlan( new ActivePlan( childPlan, initialState ) );

		    // Make a message queue for this plan.
		    makeMq( childPlan );

		    // Put it in the list of plans to be run.
// 		    m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
            // Spawn by a parent plan, not a manual executed plan by default
            m_activationQueue.enqueue( new RunPlanMethodObject( *this, session, pmId, childPlan, initialState, false, bStartFromMonitorMode ));
        }
        catch ( const TA_Base_Core::PlanAgentException& ex)
        {
            ActivePlanBulletinBoard::instance()->undoReserveInstance( childPlan );
			TA_THROW(ex);
        }

        // Return a copy of the future to the caller so they can wait on the
        // result.
        
		return rval;
    }

	ACE_Future<TA_Base_Core::EActivePlanState> PlanExecutionEngine::runInterlocked( std::auto_ptr<ActivePlan> activePlan )
	{
		ACE_Future<TA_Base_Core::EActivePlanState> rval( ActivePlanBulletinBoard::instance()->getFuture( activePlan->getPlan() ) );
		m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
		return rval;
	}

	void PlanExecutionEngine::runImpl( std::auto_ptr<ActivePlan> activePlan )
	{
        // User smart pointer to control instance life cycle
		ActivePlanMessageQueuePtr mq = getMq( activePlan->getPlan() );

		while( activePlan->getIsActive() )
		{
			if( NULL == mq.get() )
			{
				std::string remark = "Plan failed";
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "[P:%lu,I:%lu]: NULL message queue!", activePlan->getPlan().plan, activePlan->getPlan().instance );
                try
                {
				    ActivePlanBulletinBoard::instance()->update( activePlan->getPlan(), TA_Base_Core::FAILED_ACTIVE_PLAN_STATE, remark );
                }
                catch (...)
                {
                    // Do nothing
                }
				break;
			}

            if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
            {
                std::string reason = "Hang up message received from System Controller.";
                activePlan->stop(reason);
                break;
		    }

            try
            {
                processMessages( *activePlan, mq );
            }
            catch ( ActivePlanMessageException& expWhat )
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, "ActivePlanMessageException", expWhat.what() );
                break;
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Caught exception when process messages." );
                break;
            }

			if( ActivePlan::INTERLOCKED_NSEM == activePlan->executeNextStep() )
			{
				// Note: DO NOT access activePlan after calling this.
                runInterlocked( activePlan );
				break;
			}
		}
	}

	void PlanExecutionEngine::processMessages( ActivePlan& activePlan, ActivePlanMessageQueuePtr mq )
    {
        ACE_Message_Block* aceMsg = NULL;
        ActivePlanMessageBlock* msg = NULL;
        IActivePlanMessage* controlMsg = NULL;

        ACE_Time_Value absTime( ACE_OS::gettimeofday() + m_mqTimeout );

        if ( -1 == mq->dequeue(aceMsg, &absTime) )
		{
			if ( EWOULDBLOCK != ACE_OS::last_error() )
			{
				std::ostringstream what;

				what << "[P:" << activePlan.getPlan().plan
					 << ",I:" << activePlan.getPlan().instance << "]:"
					 << " ACE_Message_Queue::dequeue failed with errno: " << ACE_OS::last_error()
					 << ".";

				TA_THROW( ThirdPartyLibException( what.str() ) );
			}
		}
		else // we have a message
        {
            if ( NULL == aceMsg )
            {
                LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "Dequeue a NULL ACE_Message_Block" );
                return;
            }

            if (( ACE_Message_Block::MB_PCEVENT == aceMsg->msg_type() ) &&
                ( ACE_EVENT_SET_CONTROL == ( aceMsg->flags() & ACE_EVENT_SET_CONTROL )))
            {
                LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Msg-: Received turn to control message for Plan [%d, %d]", 
                    activePlan.getPlan().plan, activePlan.getPlan().instance );
                activePlan.setStateSyncDone( true );
                aceMsg->release();
                return;
            }

            try
            {
                msg = dynamic_cast<ActivePlanMessageBlock*>( aceMsg );					
			}
            catch( const std::bad_cast& )
            {
                aceMsg->release();
                std::ostringstream what;

                what << "[P:" << activePlan.getPlan().plan
                    << ",I:" << activePlan.getPlan().instance << "]:"
                    << " Dynamic cast to ActivePlanMessageBlock failed.";

                TA_THROW( ActivePlanMessageException( what.str() ) );
            }

            controlMsg = msg->getControlMessage();

            if ( NULL != controlMsg )
            {
                controlMsg->execute( activePlan );
            }
            else
            {
                LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "NULL control message for ActivePlanMessageBlock" );
            }

            aceMsg->release();
		}
	}

	ActivePlanMessageQueuePtr PlanExecutionEngine::getMq( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
		{
			ConstPlanMqMapIterator it = m_mqMap.find(plan);

			if ( it != m_mqMap.end() )
			{
				return( (*it).second );
			}
		}

        ActivePlanMessageQueuePtr pNullPtr;
		return pNullPtr;
	}

	void PlanExecutionEngine::makeMq( const TA_Base_Core::ActivePlanId& plan )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
		{
			PlanMqMapIterator it = m_mqMap.find(plan);

			if ( it != m_mqMap.end() )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Removing duplicate message queue (%d) for [P:%lu,I:%lu]", it->second.get(), plan.plan, plan.instance );
                it->second->abandon();
// 				delete (*it).second;
// 				(*it).second = NULL;
				m_mqMap.erase( it );
			}

            ActivePlanMessageQueuePtr pNewMQ( new ActivePlanMessageQueue() );
            LOG_GENERIC( SOURCE_INFO, DEBUG_LOG_LEVEL, "Create message queue (%d) for [P:%lu,I:%lu]", pNewMQ.get(), plan.plan, plan.instance );
			m_mqMap.insert( PlanMqMap::value_type(plan, pNewMQ) );
		}
	}

	int PlanExecutionEngine::put( ACE_Message_Block *mb, ACE_Time_Value *tv )
	{
		if( ACE_Message_Block::MB_HANGUP == mb->msg_type() )
		{
			return( putq( mb, tv ) );
		}
		else if( ACE_Message_Block::MB_DATA == mb->msg_type() )
		{
			ActivePlanMessageBlock* apMb = NULL;

			try
			{
				apMb = dynamic_cast<ActivePlanMessageBlock*>( mb );

				ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
				{
					ConstPlanMqMapIterator it = m_mqMap.find( apMb->getPlan() );

					if ( it != m_mqMap.end() )
					{
                        try
                        {
                            return it->second->enqueue( mb, tv );
                        }
                        catch ( ActivePlanMessageException& )
                        {
                            return -1;
                        }
                        catch (...)
                        {
                            // When consolidateMq erase the mq from m_mqMap, here may catch an unknow exception in Windows and crash in Solaris
                            std::ostringstream what;
                            what << "mq dequeue error";
                            TA_THROW( ThirdPartyLibException( what.str() ) );
                        }
					}
					else
					{
						mb->release();
						std::ostringstream what;

						what << "[P:" << apMb->getPlan().plan
							 << ",I:" << apMb->getPlan().instance << "]:"
							 << " Message for unknown plan discarded.";

                        TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_MESSAGE_FOR_UNKNOWN_PLAN_DISCARDED, apMb->getPlan().plan, apMb->getPlan().instance) );
                        return -1;
					}
				}
			}
			catch( const std::bad_cast& )
			{
				mb->release();
				std::ostringstream what;
				what << " MB_DATA message type with invalid contents discarded.";
				TA_THROW( ActivePlanMessageException( what.str() ) );
			}
		}
		else
		{
			mb->release();
			std::ostringstream what;

			what << " Invalid message type (" << mb->msg_type()
				 << ") discarded.";

			TA_THROW( ActivePlanMessageException( what.str() ) );
		}

		mb->release();
		return( 0 );
	}

	void PlanExecutionEngine::consolidateMq()
	{
		// Make sure only one thread tries to release the inactive message
		// queues. It would not cause a problem of more than one did but
		// it is a waste of resources having many threads taking a write
		// lock out on the message queue list.
		ACE_Guard<ACE_Mutex> guard(m_consolidateMqLock);
		{
			std::list<SortablePlanId> inactiveMqList;
			{
				ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
				{
					ConstPlanMqMapIterator it;
					for( it = m_mqMap.begin(); it != m_mqMap.end(); ++it )
                    {
                        TA_Base_Core::EActivePlanState state = (TA_Base_Core::EActivePlanState) 0xFFFFFFFF;

                        try
                        {
                            state = ActivePlanBulletinBoard::instance()->getState( (*it).first.get() );
                        }
                        catch ( ... )
                        {
                            //do nothing as plan state was not found, let the processing be default
                            LOG_GENERIC( SOURCE_INFO, WARN_LOG_LEVEL, "[P:%lu,I:%lu] ActivePlan State doesn't exist", it->first.getPlan(), it->first.getInstance() );
                        }

						switch( state )
						{
							case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
							// don't break

							case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
							// don't break

                            case TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE:
                            // don't break

							case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
							// don't break

							case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:								
							break;

                            //This is only used to take care of updates during Monitor Mode
                            case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
                            // don't break

                            case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
                            // don't break

                            case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
                            break;

							case TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE:
							// if the bulletin board doesn't know about it
							// but there are still some unstarted plans
							// in the activation queue we should keep the
							// messsage queue
							if( m_activationQueue.method_count() == 0 )
							{
								LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: message queue scheduled for deletion.", (*it).first.getPlan(), (*it).first.getInstance() );
								inactiveMqList.push_back( (*it).first.get() );
							}
							break;

							default:
								LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: message queue scheduled for deletion.", (*it).first.getPlan(), (*it).first.getInstance() );
								inactiveMqList.push_back( (*it).first.get() );
								break;
						}
					}
				}
			}

			std::list<SortablePlanId>::const_iterator inactiveIt;

			for( inactiveIt = inactiveMqList.begin(); inactiveIt != inactiveMqList.end(); ++inactiveIt )
			{
				ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
				{
					PlanMqMapIterator it = m_mqMap.find( (*inactiveIt).get() );

					if ( it != m_mqMap.end() )
                    {
                        // Use share pointer instead
//                         delete (*it).second;
//                         (*it).second = NULL;
                        it->second->abandon();
                        m_mqMap.erase( it );
					}
				}
			}
		}
	}

    void PlanExecutionEngine::clearAllMessageQueue()
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
        for( ConstPlanMqMapIterator itLoop = m_mqMap.begin(); itLoop != m_mqMap.end(); ++itLoop )
        {
            itLoop->second->abandon();
        }

        m_mqMap.clear();
    }

    void PlanExecutionEngine::signalAllPlansIntoControl()
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
        for( ConstPlanMqMapIterator itLoop = m_mqMap.begin(); itLoop != m_mqMap.end(); ++itLoop )
        {
            ACE_Message_Block::Message_Flags flgEventType = ACE_EVENT_SET_CONTROL;
            ACE_Message_Block* pMsgBlock = new ACE_Message_Block();
            pMsgBlock->msg_type( ACE_Message_Block::MB_PCEVENT );
            pMsgBlock->set_flags( flgEventType );
            itLoop->second->enqueue( pMsgBlock );
        }
    }

    void PlanExecutionEngine::terminateAndWait()
    {
        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Perf-: Start to terminate" );

        ActivePlanBulletinBoard::instance()->setHangupMessageReceived();

        ACE_Time_Value tmSleepInterval( 0, 200000 );
        while ( !ActivePlanBulletinBoard::instance()->isAllPlanStopped() )
        {
            ACE_OS::sleep( tmSleepInterval );
        }

        if (( m_activationQueue.queue() != NULL ) && 
            ( m_activationQueue.queue()->state() == ACE_Message_Queue_Base::ACTIVATED ))
        {
            m_activationQueue.queue()->close();
            LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Perf-: Close m_activationQueue successfully" );
        }

        wait();

        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Perf-: Terminated successfully" );
    }
} // TA_Base_App

