/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table350.cpp $
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
#include "app/pa/PAAgent/src/Table350.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"

#include <ace/ACE.h>

using namespace TA_Base_Core;


const unsigned long TABLE_350_MESSAGE_SEQUENCE_ID_OFFSET = 0;
const unsigned long TABLE_350_CHIME_OFFSET               = 1;
const unsigned long TABLE_350_DVA_MESSAGE_1_OFFSET       = 2;
const unsigned long TABLE_350_DVA_MESSAGE_2_OFFSET       = 4;
const unsigned long TABLE_350_DVA_MESSAGE_3_OFFSET       = 6;
const unsigned long TABLE_350_DVA_MESSAGE_4_OFFSET       = 8;
const unsigned long TABLE_350_DWELL_TIME_OFFSET          = 10;

namespace TA_IRS_App
{

// ExceptionChecked
Table350::Table350( TA_IRS_Bus::Scheduler& socketScheduler,
                    TA_IRS_Bus::Scheduler& processScheduler,
                    IPasEventSource& eventSource )
    : AbstractWriteablePASTable(350,12)
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_pasEventSource( eventSource )
{
    FUNCTION_ENTRY("Constructor");

    FUNCTION_EXIT;
}

// ExceptionChecked
Table350::~Table350()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void Table350::setTableDataAndWrite( CORBA::Octet messageSeqId, 
                                     bool hasChime, 
                                     TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                                     unsigned short dwellTimeInSecs ) 
    //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException )
{
    ACE_Future<int> future;
    WriteTable350* event = new WriteTable350( *this,
                                              future,
                                              messageSeqId,
                                              hasChime,
                                              messages,
                                              dwellTimeInSecs,
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

WriteTable350::WriteTable350( Table350& table,
                              ACE_Future<int>& future,
                              CORBA::Octet messageSeqId, 
                              bool hasChime, 
                              TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                              unsigned short dwellTimeInSecs,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource ) 
    : PASWriteEvent( table, future, socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_messageSeqId( messageSeqId )
    , m_hasChime( hasChime )
    , m_messages( messages )
    , m_dwellTimeInSecs( dwellTimeInSecs )
{
}

WriteTable350::WriteTable350( ACE_Time_Value timeToExpire,
                              Table350& table,
                              ACE_Future<int>& future,
                              CORBA::Octet messageSeqId, 
                              bool hasChime, 
                              TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                              unsigned short dwellTimeInSecs,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource ) 
    : PASWriteEvent( timeToExpire, table, future, socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_messageSeqId( messageSeqId )
    , m_hasChime( hasChime )
    , m_messages( messages )
    , m_dwellTimeInSecs( dwellTimeInSecs )
{
}

WriteTable350::~WriteTable350()
{
}

void WriteTable350::writeTable()
{
    TA_ASSERT( m_messages.length() > 0 && m_messages.length() <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ,
        "messages contains 0 or more than PA_MAXMSGSEQ dva messages");
    TA_ASSERT( TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ == 4,
        "Assumption TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ == 4 is false");
    
    unsigned short messageIds[4];
    
    for (int i=0; i< 4; ++i)
    {
        if (i < m_messages.length())
        {
            messageIds[i] = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageIdFromKey(m_messages[i]);
        }
        else
        {
            // No more messages - set to 0
            messageIds[i] = 0;
        }
    }
    
    // ReEntrant as this will also be guarded in the write() call.
    ThreadGuard guard( this->m_table.m_lock ); 
    
    PasHelpers::getInstance()->set8bit (this->m_table.m_buffer, TABLE_350_MESSAGE_SEQUENCE_ID_OFFSET, m_messageSeqId);
    PasHelpers::getInstance()->set8bit (this->m_table.m_buffer, TABLE_350_CHIME_OFFSET,               m_hasChime?1:0);
    PasHelpers::getInstance()->set16bit(this->m_table.m_buffer, TABLE_350_DVA_MESSAGE_1_OFFSET,       messageIds[0]);
    PasHelpers::getInstance()->set16bit(this->m_table.m_buffer, TABLE_350_DVA_MESSAGE_2_OFFSET,       messageIds[1]);
    PasHelpers::getInstance()->set16bit(this->m_table.m_buffer, TABLE_350_DVA_MESSAGE_3_OFFSET,       messageIds[2]);
    PasHelpers::getInstance()->set16bit(this->m_table.m_buffer, TABLE_350_DVA_MESSAGE_4_OFFSET,       messageIds[3]);
    PasHelpers::getInstance()->set16bit(this->m_table.m_buffer, TABLE_350_DWELL_TIME_OFFSET,          m_dwellTimeInSecs);
    
    m_future.set( PASConnection::getInstance()->writeTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE) );
}

PASWriteEvent* WriteTable350::recreateEvent(ACE_Time_Value timeToExpire)
{
    return new WriteTable350(timeToExpire,
        this->m_table,
        this->m_future,
        this->m_messageSeqId,
        this->m_hasChime,
        this->m_messages,
        this->m_dwellTimeInSecs,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource);
}

} // namespace TA_IRS_App

