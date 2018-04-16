/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File$
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * Manages the messaging and threading for the peers.
 * 
 * For C830 there will most likely be a single thread per peer. However for other
 * projects that have multiple regions per location, this could be increased.
 */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	const ACE_Time_Value DutyPeerTask::m_mqTimeout(0,   1000); // short
	const ACE_Time_Value DutyPeerTask::m_aqTimeout(0, 100000); // long
	const unsigned long DutyPeerTask::THREAD_COUNT = 5;

	DutyPeerTask::DutyPeerTask(std::auto_ptr<IDutyPeer> dutyPeer,bool local, int threadCount) :
		m_dutyPeer(dutyPeer),
		m_bLocalPeer(local),
		m_threadCount(threadCount)
	{
		TA_ASSERT( NULL != m_dutyPeer.get(), "NULL IDutyPeer!" );
		if(m_threadCount ==0)
		{
			m_threadCount = THREAD_COUNT;
		}
	}


	DutyPeerTask::~DutyPeerTask()
	{
		ACE_Message_Block* aceMsg = NULL;
		getq(aceMsg);

		aceMsg->release();
	}


	int DutyPeerTask::open(void*)
	{
		return activate(THR_JOINABLE, THREAD_COUNT);
	}


	int DutyPeerTask::close(u_long flags /*=0*/)
	{
		return(0);
	}


	int DutyPeerTask::svc(void )
	{
		int rval = 0;

		try
		{
			while( true )
			{
				// check the activation queue
				ACE_Time_Value aqAbsTime( ACE_OS::gettimeofday() + m_aqTimeout );

				std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue(&aqAbsTime));
				
				// No method to execute.
				if( 0 == mo.get() )
				{
					// Check the activation queue is OK.
					if ( EWOULDBLOCK != ACE_OS::last_error() )
					{
						std::ostringstream what;
						what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
						TA_THROW( TA_Base_Core::DutyAgentException( what.str() ) );
					}
					
					// Check the Message Queue
					ACE_Message_Block* aceMsg = NULL;
				
					ACE_Time_Value mqAbsTime( ACE_OS::gettimeofday() + m_mqTimeout );
					
					if( DASConst::ACE_ERROR == getq(aceMsg, &mqAbsTime) )
					{
						if ( EWOULDBLOCK != ACE_OS::last_error() )
						{
							std::ostringstream what;
							what << "ACE_Task::getq failed with errno " << ACE_OS::last_error();
							TA_THROW( TA_Base_Core::DutyAgentException( what.str() ) );
						}
					}
					else
					{
						if( ACE_Message_Block::MB_HANGUP == aceMsg->msg_type() )
						{
							LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received MB_HANGUP message." );
							putq( aceMsg );
							break;
						}
					}

				}
				else // Method to execute
				{
					if(m_bLocalPeer)
					{
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn, "DutyPeerTask for LocalDutyPeer should not go here");
					}
					// If we get to here we've got an object on the activation queue
					if( DASConst::ACE_ERROR == mo->call() )
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Method call error." );
						break;
					}
				}
			}
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );
			rval = -1;
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
			rval = -1;
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			rval = -1;
		}

		return(rval);
	}


	ACE_Future<bool> DutyPeerTask::sessionBegin( DutyState& state, const SessionInfo& session )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try{

				this->sessionBeginImpl(state,session);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionBeginMO( *this, future, state, session );			
			m_activationQueue.enqueue( newMO );
		}
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::sessionEnd( DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions )
	{
		ACE_Future<bool> future;
		
		if(m_bLocalPeer)
		{
			try
			{
				this->sessionEndImpl(state,session,regions);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
			
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionEndMO( *this, future, state, session, regions );
			
			m_activationQueue.enqueue( newMO );
		}
		

		return( future );
	}


	ACE_Future<bool> DutyPeerTask::sessionOverrideBegin( DutyState& state, const SessionInfo& session )
	{
		ACE_Future<bool> future;
		
		if(m_bLocalPeer)
		{
			try
			{
				this->sessionOverrideBeginImpl(state,session);
				future.set(true);
			}
			catch (...)
			{				
			    future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionOvrBeginMO( *this, future, state, session );
			
			m_activationQueue.enqueue( newMO );
		}
		

		return( future );
	}


	ACE_Future<bool> DutyPeerTask::sessionOverrideEnd( DutyState& state, const SessionInfo& session )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				sessionOverrideEndImpl(state,session);
				future.set(true);
			}
			catch (...)
			{				
			    future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionOvrEndMO( *this, future, state, session );
			
			m_activationQueue.enqueue( newMO );
		}
		
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const DASType::RegionSet& regions )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				transferImpl(state,session,regions);
				future.set(true);
			}
			catch (...)
			{
			    future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new TransferRegionsMO( *this, future, state, session, regions );
			
			m_activationQueue.enqueue( newMO );
		}
		
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const  DASType::SubsystemMap& subsystems )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				this->transferImpl(state,session, subsystems);
				future.set(true);
			}
			catch (...)
			{
			    future.set(false);
			}
		}
		else
		{		
			DutyPeerTaskMO* newMO = new TransferSubsystemsMO( *this, future, state, session, subsystems );

			m_activationQueue.enqueue( newMO );
		}

		return( future );
	}


	ACE_Future<bool> DutyPeerTask::degrade( DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try{
				this->degradeImpl(state,subsystems);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
			
		}
		else
		{
			DutyPeerTaskMO* newMO = new DegradeMO( *this, future, state, subsystems );

			m_activationQueue.enqueue( newMO );

		}
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::executeQuery( DutyState& state, IDutyQuery& query )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				executeQueryImpl(state,query);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
			
			
		}
		else
		{
			DutyPeerTaskMO* newMO = new ExecuteQueryMO( *this, future, state, query );			
			m_activationQueue.enqueue( newMO );
		}
		
		return( future );
	}

	void DutyPeerTask::sessionBeginImpl(DutyState& state, const SessionInfo& session)
	{
		m_dutyPeer->sessionBegin( state, session );
	}

	void DutyPeerTask::sessionEndImpl(DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions)
	{
		m_dutyPeer->sessionEnd( state, session, regions );
	}

	void DutyPeerTask::sessionOverrideBeginImpl(DutyState& state, const SessionInfo& session)
	{
		m_dutyPeer->sessionOverrideBegin( state, session );
	}

	void DutyPeerTask::sessionOverrideEndImpl(DutyState& state, const SessionInfo& session)
	{
		m_dutyPeer->sessionOverrideEnd( state, session );
	}

	void DutyPeerTask::transferImpl( DutyState& state, const TA_Base_Bus::SessionInfo& session, 
		const DASType::RegionSet& regions )
	{
		m_dutyPeer->transfer( state, session, regions );
	}

	void DutyPeerTask::transferImpl( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const  DASType::SubsystemMap& subsystems )
	{
		m_dutyPeer->transfer( state, session, subsystems );
	}

	void DutyPeerTask::degradeImpl( DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		m_dutyPeer->degrade( state, subsystems );
	}

	void DutyPeerTask::executeQueryImpl( DutyState& state, IDutyQuery& query )
	{
		m_dutyPeer->executeQuery( state, query );
	}

}


