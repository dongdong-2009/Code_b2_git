/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ScheduleDvaIdSupplier.cpp $
  * @author:  HuangQi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/ScheduledDvaDownload.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/ScheduleDvaIdSupplier.h"

namespace TA_IRS_App
{

ScheduleDvaIdSupplier* ScheduleDvaIdSupplier::s_pInstance = NULL;

ScheduleDvaIdSupplier::ScheduleDvaIdSupplier() : 
m_bEnable( false ),
m_bInitialized( false ),
m_pHistoryDownload( NULL )
{
    m_pHistoryDownload = 
        new ScheduledDvaDownload( CachedConfig::getInstance()->getIsMaster(), this );
    m_pHistoryDownload->setMinEventId( TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_SCHEDULE_EVENT_ID );

    PasTransactionManager::getInstance().bindTransactionWorker( *this, m_pHistoryDownload, true );
}

ScheduleDvaIdSupplier::~ScheduleDvaIdSupplier()
{
    // m_pHistoryDownload is deleted by PasTransactionWorker;
    m_pHistoryDownload = NULL;

    PasTransactionManager::getInstance().unbindTransactionWorker( *this );
}

void ScheduleDvaIdSupplier::createInstance()
{
    if ( NULL != s_pInstance )
    {
        return;
    }

    s_pInstance = new ScheduleDvaIdSupplier();
}

void ScheduleDvaIdSupplier::destoryInstance()
{
    if ( NULL == s_pInstance )
    {
        return;
    }

    delete s_pInstance;
    s_pInstance = NULL;
}

ScheduleDvaIdSupplier& ScheduleDvaIdSupplier::getInstance()
{
    if ( NULL == s_pInstance )
    {
        TA_THROW( TA_Base_Core::TransactiveException("Instance of ScheduleDvaIdSupplier is not created") );
    }

    return *s_pInstance;
}

void ScheduleDvaIdSupplier::enable( const bool bEnable )
{
    if ( bEnable == m_bEnable )
    {
        return;
    }

    m_bEnable = bEnable;

    // If changing to Control and uninitialized, should refresh all available event id
    if ( bEnable && !m_bInitialized && ( NULL != m_pHistoryDownload ))
    {
        m_pHistoryDownload->setRefreshEventIds( TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_SCHEDULE_EVENT_ID, 
            TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_SCHEDULE_EVENT_ID );
        PasTransactionManager::getInstance().signalTransaction( *this );
    }
}

void ScheduleDvaIdSupplier::refreshAvailableEventIds()
{
    if ( NULL == m_pHistoryDownload )
    {
        return;
    }

    std::map<unsigned short, bool> mapAvailableIds = m_pHistoryDownload->getEventIdAvaliableMap();

    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );

        for ( std::map<unsigned short, bool>::iterator itLoop = mapAvailableIds.begin(); 
            mapAvailableIds.end() != itLoop; ++itLoop )
        {
            std::set<unsigned short>::iterator itAvailable = m_setAvailableEventId.find( itLoop->first );

            if ( itLoop->second && (  itAvailable == m_setAvailableEventId.end() ))
            {
                // Event Id is available but not in cache, insert it
                m_setAvailableEventId.insert( itLoop->first );
            }
            else if ( !itLoop->second && ( itAvailable != m_setAvailableEventId.end() ))
            {
                // Event Id is not available but in cache, remove it
                m_setAvailableEventId.erase( itAvailable );
            }
        }
    }

	{
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxReservedEventIdSet );
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock2( m_mtxEventIdSet );
		for(std::set<unsigned short>::iterator it = m_setReservedEventId.begin(); it != m_setReservedEventId.end(); ++it)
		{
			m_setAvailableEventId.erase(*it);
		}
	}

	{
		ACE_Write_Guard<ACE_RW_Mutex> guardCurLock(m_mtxReservedEventIdSet);
		m_setReservedEventId.clear();
	}

    if ( !m_bInitialized )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );
        PasHelpers::getInstance()->sendPAAgentPasScheduleDvaIdUpdate( m_setAvailableEventId );
        m_bInitialized = true;
    }
}

unsigned short ScheduleDvaIdSupplier::applyAvailableEventId()
{
    if ( !m_bInitialized )
    {
        // Not synchronized with PAS yet, return false for retrying
        return INVALID_SCHEDULE_EVENT_ID;
    }

    unsigned short usAvailable = INVALID_SCHEDULE_EVENT_ID;

    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );

        if ( m_setAvailableEventId.empty() )
        {
            return FAILURE_SCHEDULE_EVENT_ID;
        }

        std::set<unsigned short>::iterator itAvailable = m_setAvailableEventId.begin();
        usAvailable = *itAvailable;
        m_setAvailableEventId.erase( itAvailable );
    }

    return usAvailable;
}

void ScheduleDvaIdSupplier::refreshEventIdAvaliableState( const unsigned short usEventId )
{
    if ( INVALID_SCHEDULE_EVENT_ID == usEventId )
    {
        return;
    }

    // Event id reverting should be done by checking with PAS
    if ( NULL == m_pHistoryDownload )
    {
        return;
    }

    m_pHistoryDownload->setRefreshEventId( usEventId );
    PasTransactionManager::getInstance().signalTransaction( *this );
}

void ScheduleDvaIdSupplier::transactionExecuted( int nResult )
{
    if ( TRANSACTION_SUCCESS == nResult )
    {
        // Refresh available Event Id
        refreshAvailableEventIds();
    }
}

bool ScheduleDvaIdSupplier::getAllAvailableEventIds( std::set<unsigned short>& setAllIds )
{
    setAllIds.clear();

    {
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );

        for ( std::set<unsigned short>::iterator itLoop = m_setAvailableEventId.begin(); 
            m_setAvailableEventId.end() != itLoop; ++itLoop )
        {
            setAllIds.insert( *itLoop );
        }
    }

    return m_bInitialized;
}

void ScheduleDvaIdSupplier::setAllAvailableEventIds( const std::set<unsigned short>& setAllIds )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );
        m_setAvailableEventId = setAllIds;
    }

    m_bInitialized = true;
}

void ScheduleDvaIdSupplier::changeEventId( const unsigned short usFrom, const unsigned short usTo )
{
    if ( usFrom == usTo )
    {
        return;
    }

    if (( FAILURE_SCHEDULE_EVENT_ID == usFrom ) || ( FAILURE_SCHEDULE_EVENT_ID == usTo ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Unexpected logic: Schedule DVA ID changed from %d to %d", usFrom, usTo );
        return;
    }

    if ( INVALID_SCHEDULE_EVENT_ID == usFrom )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );

        std::set<unsigned short>::iterator itUsing = m_setAvailableEventId.find( usTo );

        if ( m_setAvailableEventId.end() != itUsing )
        {
            m_setAvailableEventId.erase( itUsing );
        }
    }
    else if ( INVALID_SCHEDULE_EVENT_ID == usTo )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );

        std::set<unsigned short>::iterator itReleased = m_setAvailableEventId.find( usFrom );

        if ( m_setAvailableEventId.end() == itReleased )
        {
            m_setAvailableEventId.insert( usFrom );
        }
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Unexpected logic: Schedule DVA ID changed from %d to %d", usFrom, usTo );
    }
}

void ScheduleDvaIdSupplier::reserveEventId( const unsigned short usEventId )
{
	if(m_bInitialized)
	{
	    ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxEventIdSet );
		m_setAvailableEventId.erase(usEventId);
	}
	else
	{
	    ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxReservedEventIdSet );
		m_setReservedEventId.insert(usEventId);
	}
}

}
