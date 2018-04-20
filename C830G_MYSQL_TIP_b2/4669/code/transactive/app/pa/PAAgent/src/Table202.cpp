/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table202.cpp $
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
#include "app/pa/PAAgent/src/Table202.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"

using namespace TA_Base_Core;


namespace TA_IRS_App
{

// ExceptionChecked
Table202::Table202 ()
: AbstractPASTable(202, 32)
{
    FUNCTION_ENTRY("Constructor");

	FUNCTION_EXIT;
}

// ExceptionChecked
Table202::~Table202()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void Table202::processRead()
{
    // No processing required
}

CORBA::Octet Table202::getAnnounceId(unsigned int sourceId)
    //throw ( TA_Base_Core::PasConnectionException )
{
    TA_ASSERT(sourceId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSRCID,
              "sourceId out of bounds");

    ThreadGuard guard( m_lock );

    return PasHelpers::getInstance()->get8bit(m_buffer, (sourceId-1));
}


unsigned int Table202::getSourceId( CORBA::Octet announceId )
{
    ThreadGuard guard( m_lock );

    for (unsigned int sourceId=1; sourceId<= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSRCID; ++sourceId)
    {
        if (PasHelpers::getInstance()->get8bit(m_buffer, (sourceId-1)) == announceId)
        {
            return sourceId;
        }
    }

    return 0;
}

ReadTable202::ReadTable202( Table202& table,
                            Table560& table560,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable202::~ReadTable202()
{
}

void ReadTable202::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_202_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_202_FLAG_B_BIT_OFFSET );
}

} // namespace TA_IRS_App

