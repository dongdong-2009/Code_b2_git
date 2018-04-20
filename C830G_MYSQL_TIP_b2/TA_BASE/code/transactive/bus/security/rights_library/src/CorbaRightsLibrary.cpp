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
  *  Implementation of CorbaRightsLibrary.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "RightsLibraryInternal.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{
    const unsigned long INVALID_KEY( 0xFFFFFFFF );
}

namespace TA_Base_Bus
{
	
	CorbaRightsLibrary::CorbaRightsLibrary() :
		m_rightsAgent(NULL),
		m_cache(m_sessionUpdates),
		m_subscriber(m_cache),
        m_superProfileKey(INVALID_KEY)
    {
		FUNCTION_ENTRY("CorbaRightsLibrary");

		TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "Location Key not set" );

		m_cache.setRightsLibrary(this);
		// Get the entity name based on the locationKey
		unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );

		std::string agentType = TA_Base_Core::RightsAgentEntityData::getStaticType();

		TA_Base_Core::CorbaNameList rightsAgent;

		try
		{
			rightsAgent = 
				TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( agentType, locationKey,true);
		}
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch(...)
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );

			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		std::ostringstream msg;
		msg << "Failed to find agent of type " << agentType << " at location " << locationKey;
		if( 0 >= rightsAgent.size() )
		{
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(msg.str()) );
		}

		m_rightsAgent = new RightsAgentType( rightsAgent[0] );

        FUNCTION_EXIT;
	}

	CorbaRightsLibrary::~CorbaRightsLibrary()
	{
		FUNCTION_ENTRY("~CorbaRightsLibrary");
		delete m_rightsAgent;

        FUNCTION_EXIT;
	}

	bool CorbaRightsLibrary::isActionPermittedOnResource(
			const std::string& sessionId, unsigned long resourceKey,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule )
	{
		FUNCTION_ENTRY("isActionPermittedOnResource");

		LOG(SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s,R:%lu,A:%lu", sessionId.c_str(),
			resourceKey, actionKey);

		// initialise the client's return values
		reasonString = "";
		bool response  = false;
		decisionModule = UNDEFINED_DM;

		// initialise our return values
		CORBA::String_var corbaReason;
		IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
		IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;

		try
		{
			{
				TA_Base_Core::ThreadGuard guard(m_cacheLock);
				// check the cache
				if( m_cache.lookup(sessionId, resourceKey, actionKey, response, reasonString, decisionModule) )
				{
					FUNCTION_EXIT;
					return( response );
				}
			}

            // remove dependencies between rights lib and authentication lib.
            // check super profile in rights agent instead of rights lib.
            // so that we don't need to initialize authen lib in every app which is costly
            // TD11013: Now check to see if the request requires any of the 'super' behaviour.
            if ( /*isSuperProfile( sessionId ) ||*/ isSuperSession( sessionId ) )
            {
                // Create some fake query results.
                reasonString = "Privileged session, result is always true.";
                response = true;
                decisionModule = RIGHTS_DM;

                // Add it to the cache and then return true.
                {
			        TA_Base_Core::ThreadGuard guard(m_cacheLock);
			        m_cache.add( sessionId, resourceKey, actionKey, response, reasonString, decisionModule );
                }

                FUNCTION_EXIT;
                return response;
            }

            
			// make the call!
			int tryCount(0);
			bool tryAgain( true );
			
			const int MAX_RETRIES=2;
			// Begin the retry-loop
			while ( tryAgain )
			{
				try
				{
					// Increment the counter
					tryCount++;

					if ( tryCount > MAX_RETRIES )
					{
						// The tryCount will only ever reach the third time if we have a MonitorModeCalled exception!
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Monitor Mode exception caught twice!" );

						// Fill out the error message
						corbaReason = CORBA::string_dup("Attempted call on interface in Monitor mode");

						// We don't want to retry
						tryAgain = false;
					}
					else
					{

						CORBA_CALL_RETURN( corbaRsp,
						                   (*m_rightsAgent),
						                   isActionPermittedOnResource,
						                   ( sessionId.c_str(), resourceKey, actionKey, corbaReason.out(), corbaDm ) );
						// We don't want to retry
						tryAgain = false;
					}

				}
				catch ( const IRightsAgentCorbaDef::MonitorModeCall& )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "OperationModeException:", "Agent reporting monitor mode! Invalidating NamedObject and Trying again" );
					
					m_rightsAgent->invalidateObject( m_rightsAgent->getAgentName() );
				}
			}
        }
        catch( const TA_Base_Core::ObjectResolutionException& ex )
        {
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", ex.what() );
			FUNCTION_EXIT;
            TA_THROW( TA_Base_Core::RightsException(ex.what(), TA_Base_Core::RightsException::UNCONTACTABLE) );
        }
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		// assign the string
		reasonString = corbaReason.in();

		// convert the response
		switch (corbaRsp)
		{
			case IRightsAgentCorbaDef::Permit:
				response = true;
				break;

			case IRightsAgentCorbaDef::Deny:
				response = false;
				break;
			
			case IRightsAgentCorbaDef::Indeterminate:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
				break;
			
			case IRightsAgentCorbaDef::NotApplicable:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
				break;

			default:
				break;
		}

		// convert the decision module
		switch (corbaDm)
		{
			case IRightsAgentCorbaDef::Rights:
				decisionModule = RIGHTS_DM;
				break;

			case IRightsAgentCorbaDef::Mutex:
				decisionModule = MUTEX_DM;
				break;
		
			default:
				break;
		}

		// remember it
		{
			TA_Base_Core::ThreadGuard guard(m_cacheLock);
			m_cache.add( sessionId, resourceKey, actionKey, response, reasonString, decisionModule );
		}
		
		FUNCTION_EXIT;
		return( response );
	}
	void CorbaRightsLibrary::isActionPermittedOnResources(
			const std::string& sessionId, ResourceList& resourceKeys,ResourceList& returnResourceList,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule )
	{
		FUNCTION_ENTRY("isActionPermittedOnResources");
		
		for(ResourceList::iterator it=resourceKeys.begin();it==resourceKeys.end();it++)
		{
		LOG(SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s,R:%lu,A:%lu", sessionId.c_str(),
			*it, actionKey);
		}
		// initialise the client's return values
		reasonString = "";
		bool response  = false;
		decisionModule = UNDEFINED_DM;
		TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq result;

		// initialise our return values
		CORBA::String_var corbaReason;
		IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
		IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;
		TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq_var resultResourceSeqVar;

		try
		{
			/*
			{
				TA_Base_Core::ThreadGuard guard(m_cacheLock);
				// check the cache
				if( m_cache.lookup(sessionId, resourceKey, actionKey, response, reasonString, decisionModule) )
				{
					FUNCTION_EXIT;
					return( response );
				}
			}
			*/

            // remove dependencies between rights lib and authentication lib.
            // check super profile in rights agent instead of rights lib.
            // so that we don't need to initialize authen lib in every app which is costly
            // TD11013: Now check to see if the request requires any of the 'super' behaviour.
            if ( /*isSuperProfile( sessionId ) ||*/ isSuperSession( sessionId ) )
            {
                // Create some fake query results.
                reasonString = "Privileged session, result is always true.";
                response = true;
                decisionModule = RIGHTS_DM;

                // Add it to the cache and then return true.
                {
			        TA_Base_Core::ThreadGuard guard(m_cacheLock);
					for(ResourceList::iterator it=resourceKeys.begin();it==resourceKeys.end();it++)
					{
						m_cache.add( sessionId, *it, actionKey, response, reasonString, decisionModule );
						returnResourceList.push_back(*it);
					}
				}

                FUNCTION_EXIT;
                return ;
            }

            
			// make the call!
			int tryCount(0);
			bool tryAgain( true );
			
			const int MAX_RETRIES=2;
			// Begin the retry-loop
			while ( tryAgain )
			{
				try
				{
					// Increment the counter
					tryCount++;

					if ( tryCount > MAX_RETRIES )
					{
						// The tryCount will only ever reach the third time if we have a MonitorModeCalled exception!
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Monitor Mode exception caught twice!" );

						// Fill out the error message
						corbaReason = CORBA::string_dup("Attempted call on interface in Monitor mode");

						// We don't want to retry
						tryAgain = false;
					}
					else
					{
						//cast resource list to seq
						IRightsAgentCorbaDef::ResourceSeq resourceSeq;
						{
							int i=0;
							resourceSeq.length(resourceKeys.size());
							for(ResourceList::iterator itr = resourceKeys.begin(); itr != resourceKeys.end(); ++ itr )
							{
								resourceSeq[i]=*itr;
								i++;
							}
						}
						IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;

						CORBA_CALL_RETURN( resultResourceSeqVar,
						                   (*m_rightsAgent),
						                   isActionPermittedOnResources,
						                   ( sessionId.c_str(), resourceSeq, actionKey, corbaReason.out(), corbaDm ) );
						// We don't want to retry
						tryAgain = false;
						//cast seq back to list
						for(CORBA::ULong i=0;i<resultResourceSeqVar->length();i++){
							returnResourceList.push_back(resultResourceSeqVar[i]);
						}
					}

				}
				catch ( const IRightsAgentCorbaDef::MonitorModeCall& )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "OperationModeException:", "Agent reporting monitor mode! Invalidating NamedObject and Trying again" );
					
					m_rightsAgent->invalidateObject( m_rightsAgent->getAgentName() );
				}
			}
        }
        catch( const TA_Base_Core::ObjectResolutionException& ex )
        {
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", ex.what() );
			FUNCTION_EXIT;
            TA_THROW( TA_Base_Core::RightsException(ex.what(), TA_Base_Core::RightsException::UNCONTACTABLE) );
        }
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		// assign the string
		reasonString = corbaReason.in();
		/*
		// convert the response
		switch (corbaRsp)
		{
			case IRightsAgentCorbaDef::Permit:
				response = true;
				break;

			case IRightsAgentCorbaDef::Deny:
				response = false;
				break;
			
			case IRightsAgentCorbaDef::Indeterminate:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
				break;
			
			case IRightsAgentCorbaDef::NotApplicable:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
				break;

			default:
				break;
		}
		*/

		// convert the decision module
		switch (corbaDm)
		{
			case IRightsAgentCorbaDef::Rights:
				decisionModule = RIGHTS_DM;
				break;

			case IRightsAgentCorbaDef::Mutex:
				decisionModule = MUTEX_DM;
				break;
		
			default:
				break;
		}

		// remember it
		{
			TA_Base_Core::ThreadGuard guard(m_cacheLock);

			for(ResourceList::iterator it=returnResourceList.begin();it==returnResourceList.end();it++){
				m_cache.add( sessionId, *it, actionKey, response, reasonString, decisionModule );
			}
		}
		
		FUNCTION_EXIT;
		return;
	}

	bool CorbaRightsLibrary::isActionPermittedOnResourceAndLocSub(
			const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule )
	{
		FUNCTION_ENTRY("isActionPermittedOnResource");

		LOG(SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s,R:%lu,A:%lu", sessionId.c_str(),
			resourceKey, actionKey);

		// initialise the client's return values
		reasonString = "";
		bool response  = false;
		decisionModule = UNDEFINED_DM;

		// initialise our return values
		CORBA::String_var corbaReason;
		IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
		IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;

		try
		{
			{
				TA_Base_Core::ThreadGuard guard(m_cacheLock);
				// check the cache
				if( m_cache.lookup(sessionId, locationKey, subsystemKey, actionKey, response, reasonString, decisionModule) )
				{
					FUNCTION_EXIT;
					return( response );
				}
			}

            // remove dependencies between rights lib and authentication lib.
            // check super profile in rights agent instead of rights lib.
            // so that we don't need to initialize authen lib in every app which is costy
            // TD11013: Now check to see if the request requires any of the 'super' behaviour.
            if ( /*isSuperProfile( sessionId ) ||*/ isSuperSession( sessionId ) )
            {
                // Create some fake query results.
                reasonString = "Privileged session, result is always true.";
                response = true;
                decisionModule = RIGHTS_DM;

                // Add it to the cache and then return true.
                {
			        TA_Base_Core::ThreadGuard guard(m_cacheLock);
			        m_cache.add( sessionId, locationKey, subsystemKey, actionKey, response, reasonString, decisionModule );
                }

                FUNCTION_EXIT;
                return response;
            }

            
			// make the call!
			int tryCount(0);
			bool tryAgain( true );
			
			const int MAX_RETRIES=2;
			// Begin the retry-loop
			while ( tryAgain )
			{
				try
				{
					// Increment the counter
					tryCount++;

					if ( tryCount > MAX_RETRIES )
					{
						// The tryCount will only ever reach the third time if we have a MonitorModeCalled exception!
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Monitor Mode exception caught twice!" );

						// Fill out the error message
						corbaReason = CORBA::string_dup("Attempted call on interface in Monitor mode");

						// We don't want to retry
						tryAgain = false;
					}
					else
					{

						CORBA_CALL_RETURN( corbaRsp,
						                   (*m_rightsAgent),
						                   isActionPermittedOnResource,
						                   ( sessionId.c_str(), resourceKey, actionKey, corbaReason.out(), corbaDm ) );
						// We don't want to retry
						tryAgain = false;
					}

				}
				catch ( const IRightsAgentCorbaDef::MonitorModeCall& )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "OperationModeException:", "Agent reporting monitor mode! Invalidating NamedObject and Trying again" );
					
					m_rightsAgent->invalidateObject( m_rightsAgent->getAgentName() );
				}
			}
        }
        catch( const TA_Base_Core::ObjectResolutionException& ex )
        {
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", ex.what() );
			FUNCTION_EXIT;
            TA_THROW( TA_Base_Core::RightsException(ex.what(), TA_Base_Core::RightsException::UNCONTACTABLE) );
        }
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		// assign the string
		reasonString = corbaReason.in();

		// convert the response
		switch (corbaRsp)
		{
			case IRightsAgentCorbaDef::Permit:
				response = true;
				break;

			case IRightsAgentCorbaDef::Deny:
				response = false;
				break;
			
			case IRightsAgentCorbaDef::Indeterminate:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
				break;
			
			case IRightsAgentCorbaDef::NotApplicable:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
				break;

			default:
				break;
		}

		// convert the decision module
		switch (corbaDm)
		{
			case IRightsAgentCorbaDef::Rights:
				decisionModule = RIGHTS_DM;
				break;

			case IRightsAgentCorbaDef::Mutex:
				decisionModule = MUTEX_DM;
				break;
		
			default:
				break;
		}

		// remember it
		{
			TA_Base_Core::ThreadGuard guard(m_cacheLock);
			m_cache.add( sessionId, locationKey, subsystemKey, actionKey, response, reasonString, decisionModule );
		}
		
		FUNCTION_EXIT;
		return( response );
	}
	void CorbaRightsLibrary::areActionsPermittedOnSubsystemAndLocSub(
		const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
		ActionList& actionKeys, ActionList& returnActionList, std::string& reasonString,
		EDecisionModule& decisionModule )
	{

		FUNCTION_ENTRY("areActionsPermittedOnSubsystemAndLocSub");

		for( ActionList::iterator itr = actionKeys.begin(), end = actionKeys.end(); itr != end; ++ itr )
		{
			LOG(SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "session:%s,subsystem:%lu,actions:%lu", sessionId.c_str(),
				subsystemKey, *itr);
		}
		
		

		// initialise the client's return values
		reasonString = "";
		bool response  = false;
		decisionModule = UNDEFINED_DM;
		TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq result;//=new TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq();

		// initialise our return values
		CORBA::String_var corbaReason;
		IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
		IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;
		TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq_var resultActionSeqVar;

		try
		{
			{
				TA_Base_Core::ThreadGuard guard(m_cacheLock);
				// check the cache
				
				for( ActionList::iterator itr = actionKeys.begin(), end = actionKeys.end(); itr != end; ++ itr )
				{
					if( m_cache.lookup(sessionId, locationKey, subsystemKey, *itr, response, reasonString, decisionModule) )
					{
						FUNCTION_EXIT;
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "*****itr:%lu    .response :%s " ,*itr,(response)?"true":"false");
						if(response) {
							int lengthTemp=result.length();
							result.length(lengthTemp+1);
							result[lengthTemp]=*itr;
						}
						/*if(response) {
							int lengthTemp=(*result).length();
							(*result).length(lengthTemp+1);
							(*result)[lengthTemp]=*itr;
						}*/
							
					}
				}
				if(result.length()==actionKeys.size()){
					returnActionList=actionKeys;
					return;
				}
					
			}

			// remove dependencies between rights lib and authentication lib.
			// check super profile in rights agent instead of rights lib.
			// so that we don't need to initialize authen lib in every app which is costy
			// TD11013: Now check to see if the request requires any of the 'super' behaviour.
			if ( /*isSuperProfile( sessionId ) ||*/ isSuperSession( sessionId ) )
			{
				// Create some fake query results.
				reasonString = "Privileged session, result is always true.";
				response = true;
				decisionModule = RIGHTS_DM;

				// Add it to the cache and then return true.
				{
					TA_Base_Core::ThreadGuard guard(m_cacheLock);
					for( ActionList::iterator itr = actionKeys.begin(), end = actionKeys.end(); itr != end; ++ itr )
					{
						m_cache.add( sessionId, locationKey, subsystemKey, *itr, response, reasonString, decisionModule );
					}
					
				}
				FUNCTION_EXIT;	
				returnActionList=actionKeys;
				return;
			}


			// make the call!
			int tryCount(0);
			bool tryAgain( true );
			const int MAX_RETRIES=2;
			// Begin the retry-loop
			while ( tryAgain )
			{
				try
				{
					// Increment the counter
					tryCount++;

					if ( tryCount > MAX_RETRIES )
					{
						// The tryCount will only ever reach the third time if we have a MonitorModeCalled exception!
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Monitor Mode exception caught twice!" );

						// Fill out the error message
						corbaReason = CORBA::string_dup("Attempted call on interface in Monitor mode");

						// We don't want to retry
						tryAgain = false;
					}
					else
					{
						//cast list to seq
						//IRightsAgentCorbaDef::ActionSeq* actionSeq=new IRightsAgentCorbaDef::ActionSeq();
						IRightsAgentCorbaDef::ActionSeq actionSeq;//=new IRightsAgentCorbaDef::ActionSeq();


						//for(int i=0, ActionList::iterator itr = actionKeys.begin(), end = actionKeys.end(); itr != end; ++ itr,i++ ){
						//	int lengthTemp=(*actionSeq).length();
						//	(*actionSeq).length(lengthTemp+1);
						//	(*actionSeq)[lengthTemp]=*itr;
						//}
						//{
						//	int i=0;
						//	
						//	for(ActionList::iterator itr = actionKeys.begin(); itr != actionKeys.end(); ++ itr )
						//	{
						//		int lengthTemp=actionSeq.length();
						//		actionSeq.length(lengthTemp+1);
						//		actionSeq[lengthTemp]=*itr;
						//		i++;
						//	}
						//}
						{
							int i=0;
							actionSeq.length(actionKeys.size());
							for(ActionList::iterator itr = actionKeys.begin(); itr != actionKeys.end(); ++ itr )
							{
								actionSeq[i]=*itr;
								i++;
							}
						}

						IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;
						
						CORBA_CALL_RETURN( resultActionSeqVar,
							(*m_rightsAgent),
							areActionsPermittedOnResource,
							( sessionId.c_str(), resourceKey, actionSeq, corbaReason.out(), corbaDm ) );
					/*	CORBA_CALL_RETURN( &result,
							(*m_rightsAgent),
							areActionsPermittedOnResource,
							( sessionId.c_str(), resourceKey, actionSeq, corbaReason.out(), corbaDm ) );
						*/// We don't want to retry
						tryAgain = false;
						//cast seq back to list
						for(CORBA::ULong i=0;i<resultActionSeqVar->length();i++){
							//String_var name = resultActionSeqVar[ i ]->getName();
							//unsigned long temp = new TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq( , name.in() );
							//returnActionList.push_back( resultActionSeqVar[i].out );
							returnActionList.push_back( resultActionSeqVar[i]);
							//resultActionSeqVar.
							
							
						}
					}

				}
				catch ( const IRightsAgentCorbaDef::MonitorModeCall& )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "OperationModeException:", "Agent reporting monitor mode! Invalidating NamedObject and Trying again" );

					m_rightsAgent->invalidateObject( m_rightsAgent->getAgentName() );
				}
			}
		}
		catch( const TA_Base_Core::ObjectResolutionException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what(), TA_Base_Core::RightsException::UNCONTACTABLE) );
		}
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		// assign the string
		reasonString = corbaReason.in();

		// convert the response
		/*switch (corbaRsp)
		{
		case IRightsAgentCorbaDef::Permit:
			response = true;
			break;

		case IRightsAgentCorbaDef::Deny:
			response = false;
			break;

		case IRightsAgentCorbaDef::Indeterminate:
			TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
			break;

		case IRightsAgentCorbaDef::NotApplicable:
			TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
			break;

		default:
			break;
		}*/

		// convert the decision module
		switch (corbaDm)
		{
		case IRightsAgentCorbaDef::Rights:
			decisionModule = RIGHTS_DM;
			break;

		case IRightsAgentCorbaDef::Mutex:
			decisionModule = MUTEX_DM;
			break;

		default:
			break;
		}

		// remember it
		try
		{
			TA_Base_Core::ThreadGuard guard(m_cacheLock);
			for(CORBA::ULong idx=0; idx<resultActionSeqVar->length(); idx++ ){
				m_cache.add( sessionId, locationKey, subsystemKey, resultActionSeqVar[idx], response, reasonString, decisionModule );
			}
			//m_cache.add( sessionId, locationKey, subsystemKey, actionKey, response, reasonString, decisionModule );
		}
		
		catch(...){
			LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL,"************catch exception before function_exit");
		}
		FUNCTION_EXIT;
		return;
	}


	void CorbaRightsLibrary::resetCache()
	{
		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		m_cache.reset();
	}

	
	void CorbaRightsLibrary::notifyObservers(std::string& sessionId, ERightsChangeType changeType, DutyChanges& changes)
	{
		TA_Base_Core::ThreadGuard guard(m_observersLock);
		for(observeriterator it = m_observers.begin(); it != m_observers.end(); it++)
                   (*it)->OnRightsChange(sessionId,changeType,changes);
	}
	void CorbaRightsLibrary::registerObserver(IRightsChangeObserver& observer)
	{
		TA_Base_Core::ThreadGuard guard(m_observersLock);
		m_observers.push_back(&observer);
	}
	
	void CorbaRightsLibrary::unregisterObserver(IRightsChangeObserver& observer)
	{
		TA_Base_Core::ThreadGuard guard(m_observersLock);
		m_observers.remove(&observer);
	}

    bool CorbaRightsLibrary::isSuperSession( const std::string& sessionId )
    {
        return ( sessionId == TA_Base_Core::SessionAccessFactory::getSuperSessionId() );
    }
}
