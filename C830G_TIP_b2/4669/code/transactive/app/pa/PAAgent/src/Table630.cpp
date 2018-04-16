/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table630.cpp $
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

#include <malloc.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/Table630.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"

#include <ace/ACE.h>

using namespace TA_Base_Core;

namespace TA_IRS_App
{

// ExceptionChecked
Table630::Table630( TA_IRS_Bus::Scheduler& socketScheduler,
                    TA_IRS_Bus::Scheduler& processScheduler,
                    IPasEventSource& eventSource )
    : AbstractWriteablePASTable(630, 6)
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
   , m_pasEventSource( eventSource )
{
    FUNCTION_ENTRY("Constructor");

    FUNCTION_EXIT;
}

// ExceptionChecked
Table630::~Table630()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void Table630::setTableDataAndWrite( unsigned int messageKey, CORBA::Octet sourceId ) 
    //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException )
{
    ACE_Future<int> future;
    WriteTable630* event = new WriteTable630( *this,
                                              future,
                                              messageKey,
                                              sourceId,
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


WriteTable630::WriteTable630( Table630& table,
                              ACE_Future<int>& future,
                              unsigned int messageKey,
                              CORBA::Octet sourceId,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASWriteEvent( table, future, socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_messageKey( messageKey )
    , m_sourceId( sourceId )
{
}

WriteTable630::WriteTable630( ACE_Time_Value timeToExpire,
                              Table630& table,
                              ACE_Future<int>& future,
                              unsigned int messageKey,
                              CORBA::Octet sourceId,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASWriteEvent( timeToExpire, table, future, socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_messageKey( messageKey )
    , m_sourceId( sourceId )
{
}

WriteTable630::~WriteTable630()
{
}

void WriteTable630::writeTable()
{
    int adhocOffset = PasHelpers::getInstance()->getAdhocOffset(m_messageKey);
    
    TA_ASSERT(adhocOffset < TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC,
        "adhocOffset out of bounds");
    
    ThreadGuard guard( this->m_table.m_lock );
    
    PasHelpers::getInstance()->set8bit(this->m_table.m_buffer, adhocOffset, m_sourceId);
    
    this->m_future.set( PASConnection::getInstance()->writeTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE) );
}

PASWriteEvent* WriteTable630::recreateEvent(ACE_Time_Value timeToExpire)
{
    return new WriteTable630(timeToExpire,
        this->m_table,
        this->m_future,
        this->m_messageKey,
        this->m_sourceId,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource);
}

} // namespace TA_IRS_App

