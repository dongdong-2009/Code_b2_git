/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/RpcAgency.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/02/06 18:35:32 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#include "ace/OS.h"
#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/common/src/MessageSender.h"
#include "bus/response_plans/common/src/MessageReceiver.h"
#include "bus/response_plans/common/src/IReqHandler.h"
#include "bus/response_plans/common/src/RpcAgency.h"

namespace TA_Base_Bus
{

RpcAgency* RpcAgency::s_instance = NULL;
ACE_Thread_Mutex RpcAgency::s_singletonLock;

/////////////////////////////////////////////////////////////////////////////
// RpcAgency

RpcAgency::RpcAgency() :
m_ulHandler( 0u ),
m_ulLocation( 0u ),
m_unReqId( 0u ),
m_pSender( NULL ),
m_pReceiver( NULL ),
m_pAckNotify( NULL )
{
    FUNCTION_ENTRY( "RpcAgency" );

    initSender();
    initReceiver( true );

    FUNCTION_EXIT;
}

RpcAgency::RpcAgency( const unsigned int ulLocation, const unsigned int ulHandler ) :
m_ulHandler( ulHandler ),
m_ulLocation( ulLocation ),
m_unReqId( 0u ),
m_pSender( NULL ),
m_pReceiver( NULL ),
m_pAckNotify( NULL )
{
    FUNCTION_ENTRY( "RpcAgency" );

    initSender();
    initReceiver( m_ulHandler == 0u );

    FUNCTION_EXIT;
}

RpcAgency::~RpcAgency()
{
    FUNCTION_ENTRY( "~RpcAgency" );

    uninitSender();
    uninitReceiver();

    FUNCTION_EXIT;
}

RpcAgency& RpcAgency::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if ( NULL == s_instance )
    {
        ACE_Guard<ACE_Thread_Mutex> guard( s_singletonLock );

        if ( NULL == s_instance )
        {
            s_instance = new RpcAgency();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}

void RpcAgency::setInstance( RpcAgency& refAgency )
{
    s_instance = &refAgency;
}

void RpcAgency::createInstance( const unsigned int ulLocation, const unsigned int ulHandler )
{
    FUNCTION_ENTRY( "createInstance" );

    if ( NULL == s_instance )
    {
        ACE_Guard<ACE_Thread_Mutex> guard( s_singletonLock );

        if ( NULL == s_instance )
        {
            s_instance = new RpcAgency( ulLocation, ulHandler );
        }
    }

    FUNCTION_EXIT;
}

void RpcAgency::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    ACE_Guard<ACE_Thread_Mutex> guard( s_singletonLock );

    if ( NULL != s_instance )
    {
        delete s_instance;

        s_instance = NULL;
    }

    FUNCTION_EXIT;
}

void RpcAgency::initSender()
{
    if ( NULL == m_pSender )
    {
        m_pSender = new MessageSender();
    }
}

void RpcAgency::initReceiver( const bool bRequest )
{
    if ( NULL == m_pReceiver )
    {
        m_pReceiver = new MessageReceiver( *this );

        if ( bRequest )
        {
            if ( NULL == m_pAckNotify )
            {
                ACE_Thread_Mutex* pMutex = new ACE_Thread_Mutex();
                m_pAckNotify = new ACE_Condition_Thread_Mutex( *pMutex );
            }

            m_pReceiver->subscribeAck();
        }
    }
}

void RpcAgency::uninitSender()
{
    if ( NULL != m_pSender )
    {
        delete m_pSender;
        m_pSender = NULL;
    }
}

void RpcAgency::uninitReceiver()
{
    if ( NULL != m_pReceiver )
    {
        m_pReceiver->unsubscribe();

        if ( NULL != m_pAckNotify )
        {
            ACE_Thread_Mutex* pMutex = &(m_pAckNotify->mutex());

            delete m_pAckNotify;
            m_pAckNotify = NULL;

            delete pMutex;
            pMutex = NULL;
        }

        delete m_pReceiver;
        m_pReceiver = NULL;
    }
}

void RpcAgency::asyncSendAck( const unsigned int ulToLocation, const unsigned int unId )
{
    if ( NULL == m_pSender )
    {
        return;
    }

    m_pSender->sendAck( unId, ulToLocation );

}

void RpcAgency::asyncSendReq( const unsigned int ulToLocation, const int eType, const unsigned int ulPlanLocation, 
    const unsigned int ulPlanId, const unsigned int ulInstanceNum, const unsigned int ulStepNum, 
    const unsigned int ulHandler, const std::vector<std::string>& vecParameters )
{
    if ( NULL == m_pSender )
    {
        return;
    }

    const unsigned int unCurReqId = getNewReqId();

    m_pSender->sendReq( unCurReqId, ulToLocation, eType, ulPlanLocation, ulPlanId, ulInstanceNum, ulStepNum, ulHandler, vecParameters );
}

bool RpcAgency::syncSendReq( const unsigned int ulToLocation, const int eType, const unsigned int ulPlanLocation, 
    const unsigned int ulPlanId, const unsigned int ulInstanceNum, const unsigned int ulStepNum, const unsigned int ulHandler, 
    const std::vector<std::string>& vecParameters, const unsigned int unMsTimeout )
{
    if (( NULL == m_pSender ) || ( NULL == m_pAckNotify ))
    {
        return false;
    }

    const unsigned int unCurReqId = getNewReqId();
    m_pSender->sendReq( unCurReqId, ulToLocation, eType, ulPlanLocation, ulPlanId, ulInstanceNum, ulStepNum, ulHandler, vecParameters );

    m_pAckNotify->mutex().acquire();

    ACE_Time_Value tmTimeout( 0, unMsTimeout * 1000 );

    tmTimeout += ACE_OS::gettimeofday();

    bool bAcked = false;

    while ( !bAcked )
    {
        if ( -1 == m_pAckNotify->wait( &tmTimeout ))
        {
            // timeout or other error
            break;
        }

        // At least check once, would break when check elapsed time.

        std::set<unsigned int>::const_iterator itFound = m_setAckedReqIds.find( unCurReqId );

        bAcked = (m_setAckedReqIds.end() != itFound);

        if ( bAcked )
        {
            m_setAckedReqIds.erase( itFound );
        }
    }

    m_pAckNotify->mutex().release();

    return bAcked;
}

void RpcAgency::handleAck( const unsigned int ulToLocation, const unsigned int unId )
{
    if ( ulToLocation != m_ulLocation )
    {
        return;
    }

    if ( NULL != m_pAckNotify )
    {
        m_pAckNotify->mutex().acquire();

        m_setAckedReqIds.insert( unId );

        m_pAckNotify->broadcast();

        m_pAckNotify->mutex().release();
    }
}

void RpcAgency::handleReq( const unsigned int ulToLocation, const unsigned int unId, const int eType, 
    const unsigned int ulPlanLocation, const unsigned int ulPlanId, const unsigned int ulInstanceNum, 
    const unsigned int ulStepNum, const unsigned int ulHandler, const std::vector<std::string>& vecParameters )
{
    if ( ulToLocation != m_ulLocation )
    {
        return;
    }

    if ( ANY_HANDLER != ulHandler )
    {
        if ( m_ulHandler == ulHandler )
        {
            asyncSendAck( ulPlanLocation, unId );
        }
        else
        {
            // Not in scope, no handling
            return;
        }
    }

    for ( std::set<IReqHandler*>::iterator itLoop = m_setReqCallbacks.begin(); m_setReqCallbacks.end() != itLoop; ++itLoop )
    {
        IReqHandler* pCallback = *itLoop;

        if ( NULL != pCallback )
        {
            pCallback->handle( unId, eType, ulPlanLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters );
        }
    }
}

unsigned int RpcAgency::getNewReqId()
{
    return (++m_unReqId);
}

void RpcAgency::subscribe( IReqHandler& refCallback )
{
    if ( m_setReqCallbacks.empty() )
    {
        if ( NULL != m_pReceiver )
        {
            m_pReceiver->subscribeReq();
        }
    }

    if ( m_setReqCallbacks.find( &refCallback ) == m_setReqCallbacks.end() )
    {
        m_setReqCallbacks.insert( &refCallback );
    }
}

void RpcAgency::unsubscribe( IReqHandler& refCallback )
{
    std::set<IReqHandler*>::iterator itCallback = m_setReqCallbacks.find( &refCallback );

    if ( m_setReqCallbacks.end() != itCallback )
    {
        m_setReqCallbacks.erase( itCallback );

        if ( m_setReqCallbacks.empty() )
        {
            if ( NULL != m_pReceiver )
            {
                m_pReceiver->unsubscribe();
            }
        }
    }
}

}
