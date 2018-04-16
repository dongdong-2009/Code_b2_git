/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table1204.cpp $
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
#include "app/pa/PAAgent/src/Table1204.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"


using namespace TA_Base_Core;

const unsigned long TABLE_1204_RECORD_SIZE = 200;

namespace TA_IRS_App
{

// ExceptionChecked
Table1204::Table1204 ()
: AbstractPASTable(1204, 6400)
{
    FUNCTION_ENTRY("Constructor");

	FUNCTION_EXIT;
}

// ExceptionChecked
Table1204::~Table1204()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void Table1204::processRead()
{
    // Nothing to process immediately
}

TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage Table1204::getLocalCoverage(unsigned int sourceId)
    //throw ( TA_Base_Core::PasConnectionException )
{
    TA_ASSERT(sourceId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSRCID,
              "sourceId out of bounds");

    ThreadGuard guard( m_lock );

    unsigned long offset = TABLE_1204_RECORD_SIZE * (sourceId-1);

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage coverage;

    PasHelpers::getInstance()->globalCoverageBitmapsToCoverage( reinterpret_cast<ta_uint32*>(&m_buffer[offset]), coverage);
    
    return coverage;
}

ReadTable1204::ReadTable1204( Table1204& table,
                              Table560& table560,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable1204::~ReadTable1204()
{
}

void ReadTable1204::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_1204_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_1204_FLAG_B_BIT_OFFSET );
}

} // namespace TA_IRS_App


