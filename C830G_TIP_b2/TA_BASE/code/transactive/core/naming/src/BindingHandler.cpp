/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Cameron Rochester
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This is a locally created object that wrappers the sending
  * and receiving of BindingRequestMessages
  * The executing thread will block querying this object.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/src/NamingPortAccessFactory.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/message/src/BindingRequestMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/NameRebindStateUpdate_MessageTypes.h"
#include "core/message/types/NamingService_MessageTypes.h"
#include "core/naming/IDL/src/NamedObjectRepositoryCorbaDef.h"
#include "core/naming/src/BindingHandler.h"
#include "core/naming/src/Naming.h"
#include "core/naming/src/NamingThreadPoolSingleton.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/RunParams.h"


#define DEFAULT_TTL 300

namespace TA_Base_Core
{
    ObjectRepositoryInfo::ObjectRepositoryInfo( const std::string& agentName )
        : m_agentName( agentName ),
          m_timeToLive( DEFAULT_TTL ),
          m_firstResolution( true ),
          m_bindingHandler(NULL),
          m_namingThreadPoolSingleton( NamingThreadPoolSingleton::getInstance() ),
          /*CL-21230*/m_primaryNSString(""),
          /*CL-21230*/m_secondaryNSString("")
          //, /*CL-21230*/m_nsStringCount(0)
    {
        FUNCTION_ENTRY( "ObjectRepositoryInfo" );

        // Set up the inital expiry time
        resetExpiry();
        m_resolvedRepository = NamedObjectRepositoryCorbaDef::_nil();
        // Get the binding handler to handle this request
        //m_bindingHandler = new BindingHandler(); // gongzong++ CL-21311
        // Make sure that a listener exists
        //TA_ASSERT(NULL != m_bindingHandler, "A BindingHandler could not be created"); // gongzong++ CL-21311

        // yanrong++: CL-21230
        // constructor CORBA naming service locator string
        std::string primaryNSHostname(""), secondaryNSHostname("");
        unsigned long primaryNSPort = 0, secondaryNSPort = 0;

		try
		{
			primaryNSHostname= NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getPrimaryHostname();
			primaryNSPort = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getPrimaryPort();
			secondaryNSHostname = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getSecondaryHostname();
			secondaryNSPort = NamingPortAccessFactory::getInstance().getNamingPort(m_agentName).getSecondaryPort();
		}
		catch( const TA_Base_Core::DataException& ex )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}

        if ( !primaryNSHostname.empty() && primaryNSPort != 0 )
        {
            std::ostringstream oss_pstring;
            oss_pstring << "corbaloc::"
                << primaryNSHostname
                << ":"
                << primaryNSPort
                << "/"
                << Naming::getInstance().getServantKey(m_agentName);

            m_primaryNSString = oss_pstring.str().c_str();
            // ++m_nsStringCount;
        }

        if ( !secondaryNSHostname.empty() && secondaryNSPort != 0 )
        {
            std::ostringstream oss_sstring;
            oss_sstring << "corbaloc::"
                << secondaryNSHostname
                << ":"
                << secondaryNSPort
                << "/"
                << Naming::getInstance().getServantKey(m_agentName);

            m_secondaryNSString = oss_sstring.str().c_str();
            // ++m_nsStringCount;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ObjectRepositoryInfo Ctor, agentName=%s, primaryNS=%s, secondaryNS=%s",
            agentName.c_str(), m_primaryNSString.c_str(), m_secondaryNSString.c_str());

        // ++yanrong

        FUNCTION_EXIT;
    }


    ObjectRepositoryInfo::~ObjectRepositoryInfo()
    {
        FUNCTION_ENTRY( "~ObjectRepositoryInfo" );

        CORBA::release( m_resolvedRepository );
        // Clean up the handler
        // gongzong++ CL-21311
        //m_bindingHandler->deactivateAndDeleteServant();
        if ( m_bindingHandler )
        {
            m_bindingHandler->deactivateAndDeleteServant();
            m_bindingHandler = NULL;
        }
        // ++gongzong

        TA_Base_Core::NamingThreadPoolSingleton::removeInstance( m_namingThreadPoolSingleton );

        FUNCTION_EXIT;
    }


    void ObjectRepositoryInfo::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );

        TA_THREADGUARD( m_lock );

        // re-resolve asap
        m_expiryTime = 0;

        // Set back to default TTL
        m_timeToLive = DEFAULT_TTL;

        // Free up memory and set the repository to nil
        CORBA::release( m_resolvedRepository );
        m_resolvedRepository = NamedObjectRepositoryCorbaDef::_nil();

        FUNCTION_EXIT;
    }


    CORBA::Object_ptr ObjectRepositoryInfo::getObject( const std::string& objectName )
    {
        FUNCTION_ENTRY( "getObject" );

        //chenlei++ CL-21230
        //guarantee that all name resolving requests are serial
        ThreadGuard repoInfoGuard( m_repoInfoLock );
        //++chenlei CL-21230

        // Resolve the repository if necessary
        resolveObjectRepository();

        TA_THREADGUARD( m_lock );

        // if the repository is NULL we have not been able to resolve the object
        if ( CORBA::is_nil( m_resolvedRepository ) )
        {
            TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::NO_AGENT, m_agentName, objectName ) );
        }

        FUNCTION_EXIT;
        return m_resolvedRepository->getObject( objectName.c_str() );
    }


    void ObjectRepositoryInfo::setObjectRepository( NamedObjectRepositoryCorbaDef_ptr repository, unsigned long timeToLive )
    {
        FUNCTION_ENTRY( "setObjectRepository" );

        TA_THREADGUARD( m_lock );

        // Free memory assigned to old reference
        CORBA::release( m_resolvedRepository );

        //Bohong++ for more infomation when resolve
        {
            //CORBA::ORB orb = CorbaUtil::getInstance().getOrb();
            CORBA::String_var objToString( (CorbaUtil::getInstance().getOrb()).object_to_string(repository) );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ObjectToString: %s", objToString.in() );
        }
        //++Bohong

        // insert new reference
        m_resolvedRepository = NamedObjectRepositoryCorbaDef::_duplicate( repository );
        m_timeToLive = timeToLive;

        // update the expiry time
        resetExpiry();

        FUNCTION_EXIT;
    }


    void ObjectRepositoryInfo::setObjectRepository_r( NamedObjectRepositoryCorbaDef_ptr repository,
                                                      unsigned long timeToLive )
    {
        FUNCTION_ENTRY( "setObjectRepository_r" );

        setObjectRepository( repository, timeToLive );

        FUNCTION_EXIT;
    }


    void ObjectRepositoryInfo::resolveObjectRepository()
    {
        FUNCTION_ENTRY( "resolveObjectRepository" );

        bool needResolution = false;

        if ( m_firstResolution )
        {
            // Resolve the repository
            needResolution = true;
        }

        // Check to make sure the object has not expired
        if ( m_expiryTime <= time( NULL ) )
        {
            TA_THREADGUARD( m_lock );

            // Check to see if the object is valid
            if ( CORBA::is_nil( m_resolvedRepository ) )
            {
                needResolution = true;
            }
            else
            {
                // First we want to call poll on the object to see if it is valid, and in control mode
                // If the call succeeds we don't want to expire it
                try
                {
                    // Call fails, flag the need for re-resolution
                    m_resolvedRepository->poll();
                    // set expiry to specific ttl
                    resetExpiry();
                }
                catch ( const OperationModeException& )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Cached NamedObjectRepository not on [ %s ] in Control Mode. Cache cleared.", m_agentName.c_str() );

                    // set shorter expiry for fast recovery.
                    resetExpiry(true);
                    // Flag the repository for re-resolution
                    needResolution = true;
                }
                catch ( const CORBA::Exception& )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Cached NamedObjectRepository on [ %s ] unavailable. Cache cleared.", m_agentName.c_str() );

                    // set shorter expiry for fast recovery.
                    resetExpiry(true);
                    // Flag the repository for re-resolution
                    needResolution = true;
                }
            }
        }

        if ( needResolution )
        {
            // We need to resolve the repository
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "NamedObjectRepository on [ %s ] not found in cache - attempting resolution.", m_agentName.c_str() );

            // yanrong++: CL-21230
            if ( nsResolve() )
            {
                if ( m_firstResolution )
                {
                    m_firstResolution = false;
                }

                FUNCTION_EXIT;
                return ;
            }
            // ++yanrong

            // Return the object
            // This should never throw an exception, but just in case

            // gongzong++ CL-21311
            if ( !m_bindingHandler )
            {
                m_bindingHandler = new BindingHandler();
            }

            TA_ASSERT(NULL != m_bindingHandler, "A BindingHandler could not be created");
            // ++gongzong

            try
            {
                m_bindingHandler->resolveObjectRepository( this );

                TA_THREADGUARD( m_lock );
				
                if ( m_firstResolution && !CORBA::is_nil( m_resolvedRepository ) )
                {
                    m_firstResolution = false;
                }
            }
            catch ( ... )
            {
                std::string msg( "Exception caught while resolving ObjectRepository on [ " );
                msg += m_agentName;
                msg += " ]. Object not cached.";
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", msg.c_str() );
            }
        }

        FUNCTION_EXIT;
    }


    std::string ObjectRepositoryInfo::getAgentName() const
    {
        FUNCTION_ENTRY( "getAgentName" );
        FUNCTION_EXIT;
        return m_agentName;
    }


    void ObjectRepositoryInfo::resetExpiry(bool isExpired)
    {
        FUNCTION_ENTRY( "resetExpiry" );

        if ( !isExpired )
        {
            // reset the expiry time
            m_expiryTime = m_timeToLive + time( NULL );
        }
        else
        {
            // for fast recovery, if resolve failed, set expiry to be 30 seconds.
            static unsigned long s_failedTimeToLive = 5;
            // set the expiry time to current system time.
            m_expiryTime = time( NULL ) + s_failedTimeToLive;
        }

        FUNCTION_EXIT;
    }


    // gongzong++ CL-21311
    IWorkItemPtr ObjectRepositoryInfo::queueNSResovleWorkItem(const std::string& nsRefString, unsigned long resolveTimeout)
    {
        FUNCTION_ENTRY( "queueNSResovleWorkItem" );

        if ( !nsRefString.empty() )
        {
            IWorkItemPtr item( new NSResovleWorkItem(*this, nsRefString.c_str(), resolveTimeout) );

            m_namingThreadPoolSingleton->queueWorkItem(item);

            FUNCTION_EXIT;
            return item;
        }
        else
        {
            FUNCTION_EXIT;
            return IWorkItemPtr();
        }
    }


    // yanrong++: CL-21230
    bool ObjectRepositoryInfo::nsResolve()
    {
        FUNCTION_ENTRY( "nsResolve" );

        // if we can't find NS URL then we needn't resolve it
        if ( m_primaryNSString.empty() && m_secondaryNSString.empty() )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "nsResolve return false because of no nsstring. agentName=%s", m_agentName.c_str());

            FUNCTION_EXIT;
            return false;
        }

        bool ifOneNSstring = false;

        m_doubleFailedCounter = 2;
        if ( m_primaryNSString.empty() || m_secondaryNSString.empty() )
        {
            ifOneNSstring = true;
            m_doubleFailedCounter = 1;
        }

        bool success = false;

        // gongzong++ CL-21311
        m_bHasSuccess = false;
        static unsigned long s_resolveTimeout = TA_Base_Core::getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, 2000);

        {
            ThreadGuard guard(m_nsResolveCondition.getLinkedThreadLockable());

            IWorkItemPtr primaryWorkItem   = queueNSResovleWorkItem(m_primaryNSString.c_str(), s_resolveTimeout);
            IWorkItemPtr secondaryWorkItem = queueNSResovleWorkItem(m_secondaryNSString.c_str(), s_resolveTimeout);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "nsResolve wait condition, timeout = %lu ms, agentName=%s", s_resolveTimeout, m_agentName.c_str());

            m_nsResolveCondition.timedWait(s_resolveTimeout);

            // if work items still in the queue of thread pool, remove them
            m_namingThreadPoolSingleton->removeWorkItem( primaryWorkItem );
            m_namingThreadPoolSingleton->removeWorkItem( secondaryWorkItem );

            success = m_bHasSuccess;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "nsResolve finished, result = %s, timeout = %lu ms, agentName=%s",
            success? "true":"false", s_resolveTimeout, m_agentName.c_str());
        // ++gongzong
        FUNCTION_EXIT;
        return success;
    }


    void ObjectRepositoryInfo::nsResolveProcess(IWorkItem* item, const std::string& nsRefString)
    {
        FUNCTION_ENTRY( "nsResolveProcess" );

        //if ( !nsRefString.empty() )
        TA_ASSERT(!nsRefString.empty(), "An empty ns string to be used");
        {
            // NS resolve timeout default use 2000 milliseconds
            static unsigned long nsResolveTimeoutMs = getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, 2000);
            try
            {
                // gongzong++ cl-21311 do not change global CORBA timeout
                //LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-21230] Set Corba client timeout = %lu ms", nsResolveTimeoutMs);
                //omniORB::setClientCallTimeout(nsResolveTimeoutMs);

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-21230] corba stringToObject, string = %s", nsRefString.c_str());
                CORBA::Object_var tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(nsRefString.c_str());

                omniORB::setClientCallTimeout(tmpVar,nsResolveTimeoutMs);

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-21230] Narrow CorbaObject to NamedObjectRepositoryCorbaDef Object");
                NamedObjectRepositoryCorbaDef_var objRep = TA_Base_Core::NamedObjectRepositoryCorbaDef::_narrow(tmpVar);

                if ( !CORBA::is_nil(objRep) )
                {
                    try
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-21311] Set poll timeout = %lu ms", nsResolveTimeoutMs);
                        omniORB::setClientCallTimeout(objRep,nsResolveTimeoutMs);
                        objRep->poll();
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "nsResolve successfully, NSString=%s", nsRefString.c_str());

                        ThreadGuard guard(m_nsResolveCondition.getLinkedThreadLockable());
                        setObjectRepository(objRep, m_timeToLive);
                        {
                            //ThreadGuard guard(nslock);
                            m_bHasSuccess = true;
                        }
                        // gongzong++ CL-21311
                        m_nsResolveCondition.signal();
                        // ++gongzong

                        // got here means process successfully.
                        FUNCTION_EXIT;
                        return ;
                    }
                    catch ( const OperationModeException& ex)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Caught OperationModeException: %s, NSString=%s",
                            ex.reason.in(), nsRefString.c_str());
                    }
                    catch ( const CORBA::Exception& ex )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Caught COBAException: %s, m_currentNSString=%s",
                            CorbaUtil::getInstance().exceptionToString(ex).c_str(), nsRefString.c_str());
                    }
                    catch( ... )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "Caught unknown exception, m_currentNSString=%s", nsRefString.c_str());
                    }
                }
            }
            catch( const CORBA::Exception& ex )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-21230] Caught COBAException: %s, m_currentNSString=%s",
                    CorbaUtil::getInstance().exceptionToString(ex).c_str(), nsRefString.c_str());

            }
            catch( ... )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "[CL-21230] Caught unknown exception, m_currentNSString=%s", nsRefString.c_str());
            }
        }

        {
            //ThreadGuard guard(nslock);
            ThreadGuard guard(m_nsResolveCondition.getLinkedThreadLockable());
            if ( (--m_doubleFailedCounter) <=0 )
            {
                m_nsResolveCondition.signal();
            }

        }
        // got here means some unexpected error happen
        // pushFailedNsString(nsRefString);

        FUNCTION_EXIT;
    }


    // void ObjectRepositoryInfo::pushFailedNsString(const std::string& nsstring)
    // {
    //     ThreadGuard guard(m_failedNsLock);
    //     m_failedNsStrings[nsstring] = false;
    //
    //     // if all the NS resolving is failed, the signal the waiting thread
    //     if(m_failedNsStrings.size() == m_nsStringCount)
    //     {
    //         ThreadGuard conditionGuard(m_nsResolveCondition.getLinkedThreadLockable());
    //         m_nsResolveCondition.signal();
    //     }
    // }
    //
    // void ObjectRepositoryInfo::clearFailedNsString()
    // {
    //     ThreadGuard guard(m_failedNsLock);
    //     m_failedNsStrings.clear();
    // }
    //
    // unsigned long ObjectRepositoryInfo::getFailedNsStringCount()
    // {
    //     ThreadGuard guard(m_failedNsLock);
    //     return (unsigned long)m_failedNsStrings.size();
    // }
    // ++yanrong

    /////////////////////////////////////////////////////////////
    // yanrong++: CL-21230
    NSResovleWorkItem::NSResovleWorkItem( ObjectRepositoryInfo& callback, const std::string& nsString, unsigned long resolveTimeout)
        : m_callback(callback),
          m_nsString( nsString.c_str() ), // limin++, add .c_str()
          m_timeout(resolveTimeout)
    {
        FUNCTION_ENTRY( "NSResovleWorkItem" );

        ftime(&m_startTime);

        FUNCTION_EXIT;
    }


    void NSResovleWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        timeb currentTime;
        ftime(&currentTime);

        double currentTimeMs = currentTime.time * 1000 + currentTime.millitm;
        double startTimeMs     = m_startTime.time * 1000 + m_startTime.millitm;
        double elapseTimeMs  = currentTimeMs - startTimeMs;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "[for test] %s : currentTimeMs[%.0f], startTimeMs[%.0f], elapseTimeMs[%.0f], m_timeout[%lu]",
            m_nsString.c_str(), currentTimeMs, startTimeMs, elapseTimeMs, m_timeout);

        if ( elapseTimeMs < m_timeout )
        {
            m_callback.nsResolveProcess(this, m_nsString.c_str());
        }

        FUNCTION_EXIT;
    }


    // ++yanrong

    /////////////////////////////////////////////////////////////

    BindingHandler::BindingHandler()
        : m_reposInfo( NULL ),
          m_bindingRequestSender(NULL)
    {
        FUNCTION_ENTRY( "BindingHandler" );

        activateServant();

        m_bindingRequestSender = std::auto_ptr<BindingRequestMessageSender>(
            TA_Base_Core::MessagePublicationManager::getInstance().getBindingRequestMessageSender(
                TA_Base_Core::NamingService::BindName) );

        TA_ASSERT( NULL != m_bindingRequestSender.get(), "Unable to get the BindingRequestMessageSender" );

        FUNCTION_EXIT;
    }


    void BindingHandler::resolveObjectRepository( ObjectRepositoryInfo* reposInfo )
    {
        FUNCTION_ENTRY( "resolveObjectRepository" );

        TA_ASSERT( reposInfo != NULL, "ObjectRepositoryInfo should not be NULL" );
        const unsigned long DEFAULT_RESOLVE_TIMEOUT_MS = 2000; // 2 seconds
        static unsigned long s_resolveTimeoutMs = getRunParamValue(RPARAM_RESOLVE_TIMEOUT_MS, DEFAULT_RESOLVE_TIMEOUT_MS);

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: agentname=%s, try to gain condition lock", reposInfo->getAgentName().c_str());

        ThreadGuard guard(m_condition.getLinkedThreadLockable());

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: gained condition lock, is sending binding request message for naming, agentname=%s, resolveTime=%d, timewait begin",
            reposInfo->getAgentName().c_str(), s_resolveTimeoutMs );

        // Store the repository for the callback
        m_reposInfo = reposInfo;

        m_bindingRequestSender->sendBindingRequest( TA_Base_Core::NamingService::BindName,
                                                    m_reposInfo->getAgentName(),
                                                    this->_this() );

        // timedWait() unlocks the lock object so callback() can proceed
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: sent binding request message for naming, agentname=%s, resolveTime=%d, timewait begin",
            m_reposInfo->getAgentName().c_str(), s_resolveTimeoutMs );

        m_condition.timedWait( s_resolveTimeoutMs );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingHandler::resolveObjectRepository: sent binding request message for naming, agentname=%s, resolveTime=%d, timewait end",
            m_reposInfo->getAgentName().c_str(), s_resolveTimeoutMs );

        FUNCTION_EXIT;
    }


    void BindingHandler::callback( NamedObjectRepositoryCorbaDef_ptr requestedObject, CORBA::ULong timeToLive )
    {
        FUNCTION_ENTRY( "callback" );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BindingHandler::callback begin" );

        ThreadGuard guard(m_condition.getLinkedThreadLockable());

        TA_Assert(!CORBA::is_nil(requestedObject));

        m_reposInfo->setObjectRepository( requestedObject, timeToLive );

        m_condition.signal();

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BindingHandler::callback return, resolved successful" );

        FUNCTION_EXIT;
    }


}; // TA_Base_Core
