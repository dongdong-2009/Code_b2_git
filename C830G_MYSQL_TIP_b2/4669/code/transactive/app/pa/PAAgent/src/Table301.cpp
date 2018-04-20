/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table301.cpp $
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

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/Table301.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"

#include <ace/ACE.h>

using namespace TA_Base_Core;

const unsigned long TABLE_301_MESSAGE_SEQUENCE_ID_OFFSET = 0;
const unsigned long TABLE_301_ANNOUNCE_ID_OFFSET         = 1;
const unsigned long TABLE_301_PERIOD_OFFSET              = 2;
const unsigned long TABLE_301_START_TIME_OFFSET          = 4;
const unsigned long TABLE_301_STOP_TIME_OFFSET           = 8;
const unsigned long TABLE_301_EVENT_TRIGGERED_OFFSET     = 12;
const unsigned long TABLE_301_LOCAL_COVERAGE_OFFSET      = 13;

namespace TA_IRS_App
{

// ExceptionChecked
Table301::Table301( TA_IRS_Bus::Scheduler& socketScheduler,
                    TA_IRS_Bus::Scheduler& processScheduler,
                    IPasEventSource& eventSource )
    : AbstractWriteablePASTable(301,17)
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_pasEventSource( eventSource )
{
    FUNCTION_ENTRY("Constructor");

	FUNCTION_EXIT;
}

// ExceptionChecked
Table301::~Table301()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}


void Table301::setTableDataAndWrite( CORBA::Octet messageSeqId, 
                                     CORBA::Octet announceId,
                                     unsigned short periodInSecs,
                                     TA_Base_Core::DateTime startTime,
                                     TA_Base_Core::DateTime stopTime,
                                     bool isEventTriggered,
                                     const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones )
    //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException )
{
    ACE_Future<int> future;
    WriteTable301* event = new WriteTable301( *this,
                                              future,
                                              messageSeqId,
                                              announceId,
                                              periodInSecs,
                                              startTime,
                                              stopTime,
                                              isEventTriggered,
                                              zones,
                                              this->m_socketScheduler,
                                              this->m_processScheduler,
                                              this->m_pasEventSource );
    this->addWriteEvent(*event);
    this->m_socketScheduler.post( event );

    int retVal = -1;
    ACE_Time_Value tv( 20, 0 );
    tv += ACE_OS::gettimeofday();
    if( -1 == future.get( retVal, &tv ) )
    {
        char msg[100] = {0};
        sprintf(msg, "Timeout on write operation on table %d", TABLE_NUMBER);
        TA_THROW(TA_Base_Core::PasWriteErrorException(retVal, msg));
    }
    if (retVal)
    {
        char msg[100] = {0};
        sprintf(msg, "Write operation on table %d returned an error state %d", TABLE_NUMBER, retVal);
        TA_THROW(TA_Base_Core::PasWriteErrorException(retVal, msg));
    }
}

WriteTable301::WriteTable301( Table301& table,
                              ACE_Future<int>& future,
                              CORBA::Octet messageSeqId,
                              CORBA::Octet announceId,
                              unsigned short periodInSecs,
                              TA_Base_Core::DateTime startTime,
                              TA_Base_Core::DateTime stopTime,
                              bool isEventTriggered,
                              const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASWriteEvent( table, future, socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_messageSeqId( messageSeqId )
    , m_announceId( announceId )
    , m_periodInSecs( periodInSecs )
    , m_startTime( startTime )
    , m_stopTime( stopTime )
    , m_isEventTriggered( isEventTriggered )
    , m_zones( zones )
{
}

WriteTable301::WriteTable301( ACE_Time_Value timeToExpire,
                              Table301& table,
                              ACE_Future<int>& future,
                              CORBA::Octet messageSeqId,
                              CORBA::Octet announceId,
                              unsigned short periodInSecs,
                              TA_Base_Core::DateTime startTime,
                              TA_Base_Core::DateTime stopTime,
                              bool isEventTriggered,
                              const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASWriteEvent( timeToExpire, table, future, socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_messageSeqId( messageSeqId )
    , m_announceId( announceId )
    , m_periodInSecs( periodInSecs )
    , m_startTime( startTime )
    , m_stopTime( stopTime )
    , m_isEventTriggered( isEventTriggered )
    , m_zones( zones )
{
}

WriteTable301::~WriteTable301()
{
}

void WriteTable301::writeTable()
{
    ta_uint32 localCoverageBitmap;
    PasHelpers::getInstance()->coverageToLocalCoverageBitmap(this->m_zones, localCoverageBitmap);
    
    ThreadGuard guard( this->m_table.m_lock ); 
    
    PasHelpers::getInstance()->set8bit (this->m_table.m_buffer, TABLE_301_MESSAGE_SEQUENCE_ID_OFFSET, this->m_messageSeqId);
    PasHelpers::getInstance()->set8bit (this->m_table.m_buffer, TABLE_301_ANNOUNCE_ID_OFFSET,         this->m_announceId);
    PasHelpers::getInstance()->set16bit(this->m_table.m_buffer, TABLE_301_PERIOD_OFFSET,              this->m_periodInSecs);
    PasHelpers::getInstance()->set32bit(this->m_table.m_buffer, TABLE_301_START_TIME_OFFSET,          this->m_startTime);
    PasHelpers::getInstance()->set32bit(this->m_table.m_buffer, TABLE_301_STOP_TIME_OFFSET,           this->m_stopTime);
    PasHelpers::getInstance()->set8bit (this->m_table.m_buffer, TABLE_301_EVENT_TRIGGERED_OFFSET,     this->m_isEventTriggered?1:0);
    PasHelpers::getInstance()->set32bit(this->m_table.m_buffer, TABLE_301_LOCAL_COVERAGE_OFFSET,      localCoverageBitmap);
    
    m_future.set( PASConnection::getInstance()->writeTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE) );
}

PASWriteEvent* WriteTable301::recreateEvent(ACE_Time_Value timeToExpire)
{
    return new WriteTable301(timeToExpire,
        this->m_table,
        this->m_future,
        this->m_messageSeqId,
        this->m_announceId,
        this->m_periodInSecs,
        this->m_startTime,
        this->m_stopTime,
        this->m_isEventTriggered,
        this->m_zones,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource);
}

}

