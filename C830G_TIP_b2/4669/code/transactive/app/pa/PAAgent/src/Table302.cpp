/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table302.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/Table302.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/PasTableManager.h"

using namespace TA_Base_Core;


namespace TA_IRS_App
{

// ExceptionChecked
Table302::Table302()
    : AbstractPASTable(302, 12)
    , m_pasHelpersInstance(NULL)
    , m_readWait()
    , m_processWait()
{
    FUNCTION_ENTRY("Constructor");

    m_pasHelpersInstance = PasHelpers::getInstance();

    m_messageSequenceStatuses.resize(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ);


    FUNCTION_EXIT;
}

// ExceptionChecked
Table302::~Table302()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void Table302::processRead()
{
    ThreadGuard guard( m_lock );

    unsigned long offset = 0;
    for (int sequenceId=1; sequenceId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ; ++sequenceId)
    {
        m_messageSequenceStatuses[sequenceId-1].m_announceId = m_pasHelpersInstance->get8bit(m_buffer, offset++);
        // jeffrey++ TES712
		//m_messageSequenceStatuses[sequenceId-1].m_priority = m_pasHelpersInstance->get8bit(m_buffer, offset++);
        m_messageSequenceStatuses[sequenceId-1].m_sourceid = m_pasHelpersInstance->get8bit(m_buffer, offset++);
		// ++jeffrey TES712
        m_messageSequenceStatuses[sequenceId-1].m_status = static_cast<MessageSequenceStatus::EState>(m_pasHelpersInstance->get8bit(m_buffer, offset++));
    }

    this->m_processWait.broadcast();
}

unsigned int Table302::getSequenceId( CORBA::Octet announceId )
{
    ThreadGuard guard( m_lock );

    for (int sequenceId=1; sequenceId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ; ++sequenceId)
    {
        if (m_messageSequenceStatuses[sequenceId-1].m_announceId == announceId)
        {
            return sequenceId;
        }
    }
    return 0;
}

MessageSequenceStatus Table302::getMessageSequenceStatus(int messageSequenceId)
    //throw ( TA_Base_Core::PasConnectionException )
{
    TA_ASSERT( messageSequenceId > 0 && messageSequenceId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ, 
               "messageSequenceId is out of range" );

    this->m_readWait.timedWait( CachedConfig::getInstance()->getFastPollRateInMSecs() );
    this->m_processWait.timedWait( CachedConfig::getInstance()->getFastPollRateInMSecs() );

    ThreadGuard guard( m_lock );

    return m_messageSequenceStatuses[messageSequenceId-1];
}


int Table302::getNextAvailableMessageSequenceId() 
    //throw (TA_Base_Core::NoFreeMessageSequenceIdException, TA_Base_Core::PasConnectionException )
{
    static int lastSeqId = 0;

    ThreadGuard guard( m_lock );

    for (int count=0; count < TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ; ++ count)
    {
        if (lastSeqId == TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ)
        {
            // loop around
            lastSeqId = 1 ;
        }
        else
        {
            ++lastSeqId;
        }
    
        if(m_messageSequenceStatuses[lastSeqId-1].m_announceId == 0)
        {
            // Free to use
            return lastSeqId;
        }
        else
        {
            // Check to see whether we can reset this one
            if ( !AbstractBroadcastCircularList::getInstance()->isStationAnnounceIdUsed(
                    m_messageSequenceStatuses[lastSeqId-1].m_announceId))
            {
                // PAS thinks this messageId is used but we have no knowledge of it.  
                return lastSeqId;
            }
        }
    } 


    // All of the occupied messageSequenceIds are taken.  

    TA_THROW(NoFreeMessageSequenceIdException());

    return 0;
}


bool Table302::isRunning(int messageSequenceId)
{
    TA_ASSERT( messageSequenceId > 0 && messageSequenceId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ, 
               "messageSequenceId is out of range" );

    ThreadGuard guard( m_lock );

    return (m_messageSequenceStatuses[messageSequenceId-1].m_status != MessageSequenceStatus::BroadcastFinishedAbnormally && 
            m_messageSequenceStatuses[messageSequenceId-1].m_status != MessageSequenceStatus::BroadcastFinishedNormally );
}


ReadTable302::ReadTable302( Table302& table,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

ReadTable302::ReadTable302( ACE_Time_Value expiryTime,
                            Table302& table,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( expiryTime, socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

ReadTable302::~ReadTable302()
{
}

void ReadTable302::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    this->m_table.m_readWait.broadcast();
    
    ACE_Time_Value nextReadTime(this->m_timeToExpire + ACE_Time_Value( 0, CachedConfig::getInstance()->getFastPollRateInMSecs() * 1000 ));
    if (nextReadTime < ACE_OS::gettimeofday())
    {
        nextReadTime = ACE_OS::gettimeofday();
    }

    this->m_socketScheduler.post( new ReadTable302( nextReadTime,
        this->m_table,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource ) );
    
    this->m_processScheduler.post( new ProcessTable302( this->m_table ) );
}

ProcessTable302::ProcessTable302( Table302& table )
    : m_table( table )
{
}

ProcessTable302::~ProcessTable302()
{
}

void ProcessTable302::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable302::cancel()
{
    delete this;
}

} // namespace TA_IRS_App

