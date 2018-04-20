/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table560.cpp $
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
#include "app/pa/PAAgent/src/Table560.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/Table100.h"
#include "app/pa/PAAgent/src/Table202.h"
#include "app/pa/PAAgent/src/Table204.h"
#include "app/pa/PAAgent/src/Table1204.h"
#include "app/pa/PAAgent/src/Table303.h"
#include "app/pa/PAAgent/src/Table1303.h"
#include "app/pa/PAAgent/src/Table550.h"
#include "app/pa/PAAgent/src/Table570.h"
#include "app/pa/PAAgent/src/Table590.h"
#include "app/pa/PAAgent/src/Table603.h"
#include "app/pa/PAAgent/src/Table604.h"
#include "app/pa/PAAgent/src/Table610.h"
#include "app/pa/PAAgent/src/Table1610.h"
#include "app/pa/PAAgent/src/CachedConfig.h"


using namespace TA_Base_Core;

namespace TA_IRS_App
{

const unsigned int Table560::TABLE_560_TABLE_100_FLAG_A_BIT_OFFSET  = 0;
const unsigned int Table560::TABLE_560_TABLE_100_FLAG_B_BIT_OFFSET  = 1;
const unsigned int Table560::TABLE_560_TABLE_202_FLAG_A_BIT_OFFSET  = 2;
const unsigned int Table560::TABLE_560_TABLE_202_FLAG_B_BIT_OFFSET  = 3;
const unsigned int Table560::TABLE_560_TABLE_204_FLAG_A_BIT_OFFSET  = 4;
const unsigned int Table560::TABLE_560_TABLE_204_FLAG_B_BIT_OFFSET  = 5;
const unsigned int Table560::TABLE_560_TABLE_1204_FLAG_A_BIT_OFFSET = 6;
const unsigned int Table560::TABLE_560_TABLE_1204_FLAG_B_BIT_OFFSET = 7;
const unsigned int Table560::TABLE_560_TABLE_303_FLAG_A_BIT_OFFSET  = 8;
const unsigned int Table560::TABLE_560_TABLE_303_FLAG_B_BIT_OFFSET  = 9;
const unsigned int Table560::TABLE_560_TABLE_1303_FLAG_A_BIT_OFFSET = 10;
const unsigned int Table560::TABLE_560_TABLE_1303_FLAG_B_BIT_OFFSET = 11;
const unsigned int Table560::TABLE_560_TABLE_550_FLAG_A_BIT_OFFSET  = 12;
const unsigned int Table560::TABLE_560_TABLE_550_FLAG_B_BIT_OFFSET  = 13;
const unsigned int Table560::TABLE_560_TABLE_570_FLAG_A_BIT_OFFSET  = 14;
const unsigned int Table560::TABLE_560_TABLE_570_FLAG_B_BIT_OFFSET  = 15;
const unsigned int Table560::TABLE_560_TABLE_590_FLAG_A_BIT_OFFSET  = 16;
const unsigned int Table560::TABLE_560_TABLE_590_FLAG_B_BIT_OFFSET  = 17;
const unsigned int Table560::TABLE_560_TABLE_603_FLAG_A_BIT_OFFSET  = 18;
const unsigned int Table560::TABLE_560_TABLE_603_FLAG_B_BIT_OFFSET  = 19;
const unsigned int Table560::TABLE_560_TABLE_604_FLAG_A_BIT_OFFSET  = 20;
const unsigned int Table560::TABLE_560_TABLE_604_FLAG_B_BIT_OFFSET  = 21;
const unsigned int Table560::TABLE_560_TABLE_610_FLAG_A_BIT_OFFSET  = 22;
const unsigned int Table560::TABLE_560_TABLE_610_FLAG_B_BIT_OFFSET  = 23;
const unsigned int Table560::TABLE_560_TABLE_1610_FLAG_A_BIT_OFFSET = 24;
const unsigned int Table560::TABLE_560_TABLE_1610_FLAG_B_BIT_OFFSET = 25;

// ExceptionChecked
Table560::Table560( TA_IRS_Bus::Scheduler& socketScheduler,
                    TA_IRS_Bus::Scheduler& processScheduler,
                    AbstractPASTable* table100,
                    AbstractPASTable* table202,
                    AbstractPASTable* table204,
                    AbstractPASTable* table1204,
                    AbstractPASTable* table303,
                    AbstractPASTable* table1303,
                    AbstractPASTable* table550,
                    AbstractPASTable* table570,
                    AbstractPASTable* table590,
                    AbstractPASTable* table603,
                    AbstractPASTable* table604,
                    AbstractPASTable* table610,
                    AbstractPASTable* table1610,
                    IPasEventSource& eventSource )
    : AbstractWriteablePASTable(560, 8)
    , m_pasHelpersInstance(NULL)
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_table100( table100 )
    , m_table202( table202 )
    , m_table204( table204 )
    , m_table1204( table1204 )
    , m_table303( table303 )
    , m_table1303( table1303 )
    , m_table550( table550 )
    , m_table570( table570 )
    , m_table590( table590 )
    , m_table603( table603 )
    , m_table604( table604 )
    , m_table610( table610 )
    , m_table1610( table1610 )
    , m_writeTable560( false )
    , m_pasEventSource( eventSource )
{
    FUNCTION_ENTRY("Constructor");

    m_pasHelpersInstance = PasHelpers::getInstance();

    FUNCTION_EXIT;
}

// ExceptionChecked
Table560::~Table560()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

bool Table560::isAnyFlagAset()
{
    ThreadGuard guard( m_lock );

    bool ret = false;

    if (m_table100) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_100_FLAG_A_BIT_OFFSET);
    if (m_table202) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_202_FLAG_A_BIT_OFFSET);
    if (m_table204) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_204_FLAG_A_BIT_OFFSET);
    if (m_table1204) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_1204_FLAG_A_BIT_OFFSET);
    if (m_table303) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_303_FLAG_A_BIT_OFFSET);
    if (m_table1303) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_1303_FLAG_A_BIT_OFFSET);
    if (m_table550) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_550_FLAG_A_BIT_OFFSET);
    if (m_table570) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_570_FLAG_A_BIT_OFFSET);
    if (m_table590) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_590_FLAG_A_BIT_OFFSET);
    if (m_table603) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_603_FLAG_A_BIT_OFFSET);
    if (m_table604) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_604_FLAG_A_BIT_OFFSET);
    if (m_table610) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_610_FLAG_A_BIT_OFFSET);
    if (m_table1610) ret |= m_pasHelpersInstance->getBit(m_buffer, TABLE_560_TABLE_1610_FLAG_A_BIT_OFFSET);
    
    return ret;
}

void Table560::processRead()
{
    ThreadGuard guard( m_lock );

    processTableFlag(TABLE_560_TABLE_100_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_100_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_202_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_202_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_204_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_204_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_1204_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_1204_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_303_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_303_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_1303_FLAG_A_BIT_OFFSET, TABLE_560_TABLE_1303_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_550_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_550_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_570_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_570_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_590_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_590_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_603_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_603_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_604_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_604_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_610_FLAG_A_BIT_OFFSET,  TABLE_560_TABLE_610_FLAG_B_BIT_OFFSET);
    processTableFlag(TABLE_560_TABLE_1610_FLAG_A_BIT_OFFSET, TABLE_560_TABLE_1610_FLAG_B_BIT_OFFSET);
}

void Table560::processTableFlag(unsigned int tableFlagA, unsigned int tableFlagB)
{

    // Guarded in processRead()

    unsigned int byteOffset(0);
    
    bool isFlagASet = m_pasHelpersInstance->getBit(m_buffer, tableFlagA);
    bool isFlagBSet = m_pasHelpersInstance->getBit(m_buffer, tableFlagB);

    if (!isFlagASet && !isFlagBSet)
    {
        // Ignore
        return;
    }
    else if (!isFlagASet && isFlagBSet)
    {
        // PAS is in the process of responding.  We expect both flags to be 
        // reset in the next update
        return;
    }
    else if (isFlagASet && !isFlagBSet)
    {
        // Ingore - this should be impossible.  We'll try to reset the flags anyhow
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
            "Unexpected flag table 560 state - Flag A is true and Flag B is false");

        resetFlags(tableFlagA, tableFlagB);
        return;
    }
    else // (isFlagASet && isFlagBSet)
    {
        TA_IRS_Bus::IEvent* event = NULL;

        switch( tableFlagA )
        {
            case TABLE_560_TABLE_100_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table100 )
                    break;
                event = new ReadTable100( dynamic_cast<Table100&>(*(this->m_table100)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_202_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table202 )
                    break;
                event = new ReadTable202( dynamic_cast<Table202&>(*(this->m_table202)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_204_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table204 )
                    break;
                event = new ReadTable204( dynamic_cast<Table204&>(*(this->m_table204)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_1204_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table1204 )
                    break;
                event = new ReadTable1204( dynamic_cast<Table1204&>(*(this->m_table1204)),
                                           *this,
                                           this->m_socketScheduler,
                                           this->m_processScheduler,
                                           this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_303_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table303 )
                    break;
                event = new ReadTable303( dynamic_cast<Table303&>(*(this->m_table303)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_1303_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table1303 )
                    break;
                event = new ReadTable1303( dynamic_cast<Table1303&>(*(this->m_table1303)),
                                           *this,
                                           this->m_socketScheduler,
                                           this->m_processScheduler,
                                           this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_550_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table550 )
                    break;
                event = new ReadTable550( dynamic_cast<Table550&>(*(this->m_table550)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_570_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table570 )
                    break;
                event = new ReadTable570( dynamic_cast<Table570&>(*(this->m_table570)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_590_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table590 )
                    break;
                event = new ReadTable590( dynamic_cast<Table590&>(*(this->m_table590)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_603_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table603 )
                    break;
                event = new ReadTable603( dynamic_cast<Table603&>(*(this->m_table603)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_604_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table604 )
                    break;
                event = new ReadTable604( dynamic_cast<Table604&>(*(this->m_table604)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_610_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table610 )
                    break;
                event = new ReadTable610( dynamic_cast<Table610&>(*(this->m_table610)),
                                          *this,
                                          this->m_socketScheduler,
                                          this->m_processScheduler,
                                          this->m_pasEventSource );
                break;
            case TABLE_560_TABLE_1610_FLAG_A_BIT_OFFSET:
                if( NULL == this->m_table1610 )
                    break;
                event = new ReadTable1610( dynamic_cast<Table1610&>(*(this->m_table1610)),
                                           *this,
                                           this->m_socketScheduler,
                                           this->m_processScheduler,
                                           this->m_pasEventSource );
                break;
            default:
                break;
        }
        if( event != NULL )
            this->m_socketScheduler.post( event );
    }
}

void Table560::resetFlags( unsigned int tableFlagA, unsigned int tableFlagB )
{
    
    // Scopes to prevent clash with lock in isAnyFlagAset()
    {
        ThreadGuard guardWrite( m_lock );
        m_pasHelpersInstance->setBit(m_buffer, tableFlagA, false);
    }


    if (!isAnyFlagAset())
    {
        // we have processed all flagged tables

        if( !this->m_writeTable560 )
        {
            WriteTable560* event = new WriteTable560( *this,
                                                      this->m_socketScheduler,
                                                      this->m_processScheduler,
                                                      this->m_pasEventSource );
            this->addWriteEvent(*event);
            this->m_socketScheduler.post( event );
            this->m_writeTable560 = true;
        }
    }
}


ReadTable560::ReadTable560( Table560& table,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

ReadTable560::ReadTable560( ACE_Time_Value expiryTime,
                            Table560& table,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( expiryTime, socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

ReadTable560::~ReadTable560()
{
}

void ReadTable560::readTable()
{
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
//    ACE_Time_Value nextReadTime(this->m_timeToExpire + ACE_Time_Value( 0, CachedConfig::getInstance()->getFastPollRateInMSecs() * 1000 ));
//    if (nextReadTime < ACE_OS::gettimeofday())
//    {
//        nextReadTime = ACE_OS::gettimeofday();
//    }

//    this->m_socketScheduler.post( new ReadTable560( nextReadTime,
//        this->m_table,
//        this->m_socketScheduler,
//        this->m_processScheduler,
//        this->m_pasEventSource ) );
    
    this->m_processScheduler.post( new ProcessTable560( this->m_table ) );
}

ProcessTable560::ProcessTable560( Table560& table )
    : m_table( table )
{
}

ProcessTable560::~ProcessTable560()
{
}

void ProcessTable560::consume()
{
    this->m_table.processRead();

    if (!m_table.isAnyFlagAset())
    {           
        ACE_Time_Value nextReadTime(this->m_timeToExpire + ACE_Time_Value( 0, CachedConfig::getInstance()->getFastPollRateInMSecs() * 1000 ));
        if (nextReadTime < ACE_OS::gettimeofday())
        {
            nextReadTime = ACE_OS::gettimeofday();
        }

        // Queue next read
        this->m_table.m_socketScheduler.post( new ReadTable560( nextReadTime,
            this->m_table,
            this->m_table.m_socketScheduler,
            this->m_table.m_processScheduler,
            this->m_table.m_pasEventSource ) );
    }

    delete this;
}

void ProcessTable560::cancel()
{
    delete this;
}

WriteTable560::WriteTable560( Table560& table,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
   : PASWriteEvent( table, socketScheduler, processScheduler, eventSource )
   , m_table( table )
{
}

WriteTable560::WriteTable560( ACE_Time_Value timeToExpire,
                              Table560& table,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
   : PASWriteEvent( timeToExpire, table, socketScheduler, processScheduler, eventSource )
   , m_table( table )
{
}

WriteTable560::~WriteTable560()
{
}

void WriteTable560::writeTable()
{
    // ReEntrant as this will also be guarded in the write() call.
    ThreadGuard guard( this->m_table.m_lock ); 
    
    PASConnection::getInstance()->writeTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    this->m_table.m_writeTable560 = false;  // mission complete

    // Resume reading
    this->m_socketScheduler.post( new ReadTable560( ACE_OS::gettimeofday(),
        this->m_table,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource ) );

}

PASWriteEvent* WriteTable560::recreateEvent(ACE_Time_Value timeToExpire)
{
    return new WriteTable560(timeToExpire,
                             this->m_table,
                             this->m_socketScheduler,
                             this->m_processScheduler,
                             this->m_pasEventSource);
}

} // namespace TA_IRS_App

