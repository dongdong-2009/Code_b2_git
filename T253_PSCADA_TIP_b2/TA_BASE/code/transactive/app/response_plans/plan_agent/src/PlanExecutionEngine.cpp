/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanExecutionEngine.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #10 $
  *
  *  Last modification: $DateTime: 2014/11/10 18:14:32 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanExecutionEngine.
  */

#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageException.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ThirdPartyLibException.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/SortablePlanId.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageQueue.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageBlock.h"
#include "app/response_plans/plan_agent/src/RunPlanMethodObject.h"
#include "app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"

namespace TA_Base_App
{

    const ACE_Message_Block::Message_Flags ACE_EVENT_SET_CONTROL = ACE_Message_Block::USER_FLAGS + 0x0010;
    const ACE_Message_Block::Message_Flags ACE_EVENT_SET_MONITOR = ACE_Message_Block::USER_FLAGS + 0x0020;

	PlanExecutionEngine::PlanExecutionEngine() :
    m_activationQueue(),
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
        // PlanHouseKeeping enqueue
        m_activationQueue.enqueue( new PlanHouseKeepingMethodObject( *this ));

		return activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1 );
	}

	int PlanExecutionEngine::close( u_long flags /*=0*/ )
	{
		return 0;
	}

	int PlanExecutionEngine::svc( void )
	{
        std::auto_ptr<ACE_Method_Request> ptrMoRequest(NULL);

		try
		{
            // Blocking dequeue
            ptrMoRequest.reset(m_activationQueue.dequeue()); // jian.huang
        }
        catch( const std::exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
        }
        catch( const CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
        }

		// No method to execute.
		if( NULL == ptrMoRequest.get() )
		{
            // jian.huang
            // If m_activationQueue is deactivated, ACE_OS::last_error() should return ESHUTDOWN
            // so all threads which are blocked at m_activationQueue.dequeue() should break out of the loop
            // and this will instead of original getq method to notify idling threads to exit the loop
            if ( ESHUTDOWN == ACE_OS::last_error() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Idle thread receive hang up signal" );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "ACE_Activation_Queue::dequeue failed with errno %d", ACE_OS::last_error() );
            }
        }
		else // Method to execute
		{
			// If we get to here we've got an object on the activation queue
            try
            {
                if( -1 == ptrMoRequest->call() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Method call error." );
                }
            }
            catch (...) 
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled when runPlanExecutionEngine::svc" );
            }
		}

		return 0;
	}


    void PlanExecutionEngine::run( 
        const std::string& session, TA_Base_Core::ActivePlanId& plan, const unsigned long entity,
        const bool bPending, const bool manExecution, const bool bStartFromMonitorMode )
	{
		// First we need to reserve an instance because we have to return an instance to 
		// the caller synchronously so that they can refer to this plan in subsequent messages.

        if (thr_count() >= PlanAgentUser::getInstance()->getActivePlanThreadPoolSize())
        {
            std::ostringstream what;
            what << "The number of currently active instances has reached the serviceable limit of "
                << PlanAgentUser::getInstance()->getActivePlanThreadPoolSize() << ". No new instances can be created.";

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.str().c_str());
            TA_THROW( TA_Base_Core::PlanAgentException( what.str(), TA_Base_Core::PAE_NO_RESOURCE) );
        }

		ActivePlanBulletinBoard::instance()->reserveInstance( session, plan, entity, bPending, manExecution);

		// Turn the plan into an "Active" plan. The plan is newed here and is
		// passed around as an auto_ptr so ownership is constantly passed on
		// until the final method/object that holds ownership is destroyed.

        try
        {
// 		    std::auto_ptr<ActivePlan> activePlan( new ActivePlan( plan, initialState ) );

		    // Make a message queue for this plan. Cannot be created in ActivePlan thread since it's needed in Monitor Mode.
		    makeMq( plan );

		    // Put it in the list of plans to be run.
// 		    m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
            m_activationQueue.enqueue( new RunPlanMethodObject( *this, session, plan, bPending, manExecution, bStartFromMonitorMode ));

            // Create new thread
            activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 1 );
        }
        catch ( const TA_Base_Core::PlanAgentException& ex)
        {
            ActivePlanBulletinBoard::instance()->undoReserveInstance( plan );
			TA_THROW(ex);
        }
    }

    void PlanExecutionEngine::publishPlanStart( const std::string& session, 
        const TA_Base_Core::ActivePlanId& plan, const bool bPending, const bool manExecution )
    {
        // No update should be send before RunPlan or CustomisePlan is update in Monitor Agent
//        ActivePlanBulletinBoard::instance()->sendUpdateMessage( plan );

        //Send State Update message for Taking control
        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

        TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
        psu->session = session.c_str();
        psu->plan = plan;
        psu->manuExec = manExecution;

        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( plan ).detail.path.in();
        const unsigned long ulInstanceNum = plan.instance;

        if ( bPending )
        {
            AAMessageSender::instance()->submitAuditPlanPending( session, strPlanName, ulInstanceNum );
        }
        else
        {
            pasUpdateData->update.runPlanUpdate(psu);
            if (ActivePlanBulletinBoard::instance()->isGRCPlan(plan))
            {
                if (ActivePlanBulletinBoard::instance()->isManualPlan(plan))
                {
                    AAMessageSender::instance()->submitAuditManualGrcStarted( session, strPlanName, ulInstanceNum );	
                }
                else//automatic GRC
                {
                    AAMessageSender::instance()->submitAuditAutoGrcStarted( session, strPlanName, ulInstanceNum );
                }
            }
            else//DSS plans
            {
                AAMessageSender::instance()->submitAuditPlanExecution( session, strPlanName, ulInstanceNum );
            }
        }

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);

        // Send update to Plan Manager although useless update will send to Monitor Agent too
        ActivePlanBulletinBoard::instance()->sendUpdateMessage( plan );
    }


    void PlanExecutionEngine::spawn( const std::string& session, const TA_Base_Core::ActivePlanId& parentPlan, 
        TA_Base_Core::ActivePlanId& childPlan, const bool bPending, const bool bStartFromMonitorMode )
	{
		// First we need to reserve an instance because we have to return an instance to 
		// the caller synchronously so that they can refer to this plan in subsequent messages.

        if (thr_count() >= PlanAgentUser::getInstance()->getActivePlanThreadPoolSize())
        {
            std::ostringstream what;
            what << "The number of currently active instances has reached the serviceable limit of "
                << PlanAgentUser::getInstance()->getActivePlanThreadPoolSize() << ". No new instances can be created.";

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.str().c_str());
            TA_THROW( TA_Base_Core::PlanAgentException( what.str(), TA_Base_Core::PAE_NO_RESOURCE) );
        }

		ActivePlanBulletinBoard::instance()->reserveInstance( session, parentPlan, childPlan, bPending );
		// Turn the plan into an "Active" plan. The plan is newed here and is
		// passed around as an auto_ptr so ownership is constantly passed on
		// until the final method/object that holds ownership is destroyed.

        try
        {
// 		    std::auto_ptr<ActivePlan> activePlan( new ActivePlan( childPlan, initialState ) );

		    // Make a message queue for this plan. Cannot be created in ActivePlan thread since it's needed in Monitor Mode.
		    makeMq( childPlan );

		    // Put it in the list of plans to be run.
// 		    m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
            // Spawn by a parent plan, not a manual executed plan by default
            m_activationQueue.enqueue( new RunPlanMethodObject( *this, session, childPlan, bPending, false, bStartFromMonitorMode ));

            // Create new thread
            activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 1 );
        }
        catch ( const TA_Base_Core::PlanAgentException& ex)
        {
            ActivePlanBulletinBoard::instance()->undoReserveInstance( childPlan );
			TA_THROW(ex);
        }
    }

	void PlanExecutionEngine::runImpl( std::auto_ptr<ActivePlan> activePlan )
	{
        // User smart pointer to control instance life cycle
        ActivePlanMessageQueuePtr mq = getMq( activePlan->id() );

		while( activePlan->getIsActive() )
		{
			if( NULL == mq.get() )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[P:%lu,I:%lu]: NULL message queue!", activePlan->id().plan, activePlan->id().instance );
                try
                {
                    activePlan->setIsActive( false );
				    ActivePlanBulletinBoard::instance()->updatePlanState( activePlan->id(), TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, RemarkRunPlanFailed );
                }
                catch (...)
                {
                    // Do nothing
                }
				break;
			}

            //if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
            //{
            //    try
            //    {
            //        activePlan->setIsActive( false );
            //        ActivePlanBulletinBoard::instance()->updatePlanState( activePlan->id(), TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, RemarkRunPlanFailed );
            //    }
            //    catch ( const std::exception& expWhat )
            //    {
            //        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            //    }
            //    catch (...)
            //    {
            //        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when executing plan" );
            //    }
            //    break;
            //}

            try
            {
                processMessages( *activePlan, mq );
            }
            catch ( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                break;
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught exception when process messages." );
                break;
            }
		}

        dropMq( activePlan->id() );
	}

	void PlanExecutionEngine::processMessages( ActivePlan& activePlan, boost::shared_ptr<ActivePlanMessageQueue> mq )
    {
        ACE_Message_Block* aceMsg = NULL;
        ActivePlanMessageBlock* msg = NULL;
        IActivePlanMessage* controlMsg = NULL;

        static ACE_Time_Value DequeueTimeout( 0, 100000 ); // 100 milliseconds
        if ( -1 == mq->dequeue(aceMsg) ) // Blocking call
		{
			if ( EWOULDBLOCK != ACE_OS::last_error() )
			{
				std::ostringstream what;
				what << "[P:" << activePlan.id().plan << ",I:" << activePlan.id().instance << "]:"
					 << " ACE_Message_Queue::dequeue failed with errno: " << ACE_OS::last_error() << ".";
				TA_THROW( ThirdPartyLibException( what.str() ) );
			}
		}
		else // we have a message
        {
            if ( NULL == aceMsg )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Dequeued a NULL ACE_Message_Block" );
                return;
            }

            if ( ACE_Message_Block::MB_PCEVENT == aceMsg->msg_type() )
            {
                ACE_Message_Block::Message_Flags flgCurFlag = aceMsg->flags();

                if ( ACE_EVENT_SET_CONTROL == ( flgCurFlag & ACE_EVENT_SET_CONTROL ))
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: Received turn to control message for Plan [%d, %d]", 
                        activePlan.id().plan, activePlan.id().instance );
                    activePlan.setStateSyncDone( true );
                    aceMsg->release();
                    return;
                }
                else if ( ACE_EVENT_SET_MONITOR == ( flgCurFlag & ACE_EVENT_SET_MONITOR ))
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: Received turn to monitor message for Plan [%d, %d]", 
                        activePlan.id().plan, activePlan.id().instance );
                    activePlan.setIsActive( false );
                    aceMsg->release();
                    return;
                }
            }

            msg = dynamic_cast<ActivePlanMessageBlock*>(aceMsg);

            if ( NULL != msg )
            {
                controlMsg = msg->getControlMessage();
            }

            if ( NULL != controlMsg )
            {
                controlMsg->execute( activePlan );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL control message for ActivePlanMessageBlock" );
            }

            aceMsg->release();
		}
	}

	boost::shared_ptr<ActivePlanMessageQueue> PlanExecutionEngine::getMq( const TA_Base_Core::ActivePlanId& plan ) const
	{
        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);

			ConstPlanMqMapIterator it = m_mqMap.find(plan);

			if ( it != m_mqMap.end() )
			{
				return it->second;
			}
		}

        ActivePlanMessageQueuePtr pNullPtr;
		return pNullPtr;
	}

    void PlanExecutionEngine::makeMq( const TA_Base_Core::ActivePlanId& plan )
	{
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
		PlanMqMapIterator it = m_mqMap.find(plan);

		if ( it != m_mqMap.end() )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Removing duplicate message queue (%p) for [P:%lu,I:%lu]", it->second.get(), plan.plan, plan.instance );
            it->second->abandon();

			m_mqMap.erase( it );
		}

        ActivePlanMessageQueuePtr ptrNewMQ( new ActivePlanMessageQueue() );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Create message queue (%p) for [P:%lu,I:%lu]", ptrNewMQ.get(), plan.plan, plan.instance );
		m_mqMap.insert( PlanMqMap::value_type(plan, ptrNewMQ) );
    }

    void PlanExecutionEngine::dropMq( const TA_Base_Core::ActivePlanId& plan )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);

        PlanMqMapIterator it = m_mqMap.find( plan );

        if ( it != m_mqMap.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removing exist message queue (%p) for [P:%lu,I:%lu]", it->second.get(), plan.plan, plan.instance );
            it->second->abandon();

            m_mqMap.erase( it );
        }
    }

    int PlanExecutionEngine::put( ACE_Message_Block *mb, ACE_Time_Value *tv )
    {
        int nResult = 0;

        if ( NULL == mb )
        {
            nResult = - 1;
        }
        else if( ACE_Message_Block::MB_DATA == mb->msg_type() )
        {
            ActivePlanMessageBlock* apMb = dynamic_cast<ActivePlanMessageBlock*>( mb );

            if ( NULL == apMb )
            {
                mb->release();

                TA_THROW( ActivePlanMessageException( "MB_DATA message type with invalid contents discarded" ));
            }

            ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
            {
                ConstPlanMqMapIterator it = m_mqMap.find( apMb->getPlan() );

                if ( it != m_mqMap.end() )
                {
                    try
                    {
                        nResult = it->second->enqueue( mb, tv );
                    }
                    catch ( ActivePlanMessageException& )
                    {
                        mb->release();
                        nResult = -1;
                    }
                    catch (...)
                    {
                        mb->release();

                        // When consolidateMq erase the mq from m_mqMap, here may catch an unknow exception in Windows and crash in Solaris
                        std::ostringstream what;
                        what << "mq dequeue error";
                        TA_THROW( ThirdPartyLibException( what.str() ) );
                    }
                }
                else
                {
                    const unsigned long ulActiveId = apMb->getPlan().plan;
                    const unsigned long ulActiveInstance = apMb->getPlan().instance;
                    apMb->release();

                    std::ostringstream what;
                    what << "[P:" << ulActiveId << ",I:" << ulActiveInstance << "]:" << " Message for unknown plan discarded.";
                    TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_MESSAGE_FOR_UNKNOWN_PLAN_DISCARDED, ulActiveId, ulActiveInstance) );
                }
            }
        }
        else
        {
            std::ostringstream what;
            what << " Invalid message type (" << mb->msg_type() << ") discarded.";
            mb->release();
            TA_THROW( ActivePlanMessageException( what.str() ) );
        }

        return nResult;
    }

    void PlanExecutionEngine::signalAgentModeChanged( const bool bControl )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);

        for( ConstPlanMqMapIterator itLoop = m_mqMap.begin(); itLoop != m_mqMap.end(); ++itLoop )
        {
            ACE_Message_Block::Message_Flags flgEventType = bControl ? ACE_EVENT_SET_CONTROL : ACE_EVENT_SET_MONITOR;
            ACE_Message_Block* pMsgBlock = new ACE_Message_Block();
            pMsgBlock->msg_type( ACE_Message_Block::MB_PCEVENT );
            pMsgBlock->set_flags( flgEventType );
            itLoop->second->enqueue( pMsgBlock );
        }
    }

    void PlanExecutionEngine::signalHangUpEvent()
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);

        for( ConstPlanMqMapIterator itLoop = m_mqMap.begin(); itLoop != m_mqMap.end(); ++itLoop )
        {
            ACE_Message_Block* pMsgBlock = new ACE_Message_Block();
            pMsgBlock->msg_type( ACE_Message_Block::MB_HANGUP );
            // No need to handle this message but unblock message queue
            itLoop->second->enqueue( pMsgBlock );
        }
    }

    void PlanExecutionEngine::terminateAndWait()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Perf-: Start to terminate" );

        ActivePlanBulletinBoard::instance()->setHangupMessageReceived();

        signalHangUpEvent();

        static const ACE_Time_Value tmSleepInterval( 0, 200000 ); // 200 millisecond
        while ( !ActivePlanBulletinBoard::instance()->isAllPlanStopped() )
        {
            ACE_OS::sleep( tmSleepInterval );
        }

        wait();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Perf-: Terminated successfully" );
    }

} // TA_Base_App

