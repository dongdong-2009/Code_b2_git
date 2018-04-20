/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table100.cpp $
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
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "app/pa/PAAgent/src/Table100.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PASConnection.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

Table100::Table100 ()
: AbstractPASTable(100, 51200),
  m_paAgentCommsSender(NULL)
{
    FUNCTION_ENTRY("Constructor");

    if (!m_paAgentCommsSender)
    {
        m_paAgentCommsSender = 
            MessagePublicationManager::getInstance().getCommsMessageSender( PAAgentComms::Context );
    }

    // Initialise StationPriorities sequence length
    m_priorityScheme.length(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSTNID);

    for (int station=0; station<TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSTNID; ++station)
    {
        // Initialise ZonePriorities sequence length
        m_priorityScheme[station].length(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID);
        for (int zone=0; zone<TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID; ++zone)
        {
            // Initialise ModePriorities sequence length
            m_priorityScheme[station][zone].length(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMODEID);
        }
    }
    
    FUNCTION_EXIT;
}

// ExceptionChecked
Table100::~Table100()
{
    FUNCTION_ENTRY("Destructor");

    if (m_paAgentCommsSender)
    {
        delete m_paAgentCommsSender;
        m_paAgentCommsSender = NULL;
    }

    FUNCTION_EXIT;
}

void Table100::processRead()
{
    ThreadGuard guard( m_lock );

    unsigned long offset=0;

    for (int station=0; station<TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSTNID; ++station)
    {
        for (int zone=0; zone<TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID; ++zone)
        {
            for (int mode=0; mode<TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMODEID; ++mode)
            {
                m_priorityScheme[station][zone][mode] = PasHelpers::getInstance()->getInstance()->get8bit(m_buffer, offset++);
            }
        }
    }
}

void Table100::notifyIscs()
{
    // Alert operators to the new scheme, only if updated via table 560
    // TD 15128
	/*PasHelpers::getInstance()->raisePASPrioritySchemeUpdateNonUniqueAlarm();*/
	PasHelpers::getInstance()->submitPaPrioritySchemeUpdateAuditMessage();
	// TD 15128

    try
    {

        // Update any connected PA Managers
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Sending PrioritySchemeUpdate message to subscribers");

        CORBA::Any data;

        data <<= getPrioritySchemeCopy();

        m_paAgentCommsSender->sendCommsMessage(
            PAAgentComms::PrioritySchemeUpdate,                     // Context::Type
            CachedConfig::getInstance()->getAgentKey(),             // EntityKey
            data,                                                   // Data
            CachedConfig::getInstance()->getAgentSubsystemKey(),    // Subsystem
            CachedConfig::getInstance()->getAgentLocationKey(),     // LocationKey
            NULL);
    }
    catch(...)
    {
        // Not expecting any exceptions

        // Note that TA_Base_Core::PasConnectionException will never occur as at this point
        // we have already read the table
    }
}


TA_Base_Bus::IPAAgentCorbaDef::PriorityScheme* Table100::getPrioritySchemeCopy()
    //throw ( TA_Base_Core::PasConnectionException )
{
    TA_Base_Bus::IPAAgentCorbaDef::PriorityScheme* newPriorityScheme = new TA_Base_Bus::IPAAgentCorbaDef::PriorityScheme;

    ThreadGuard guard( m_lock );

    *newPriorityScheme = m_priorityScheme;

    return newPriorityScheme;
}

ReadTable100::ReadTable100( Table100& table,
                            Table560& table560,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent(socketScheduler, processScheduler, eventSource)
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable100::~ReadTable100()
{
}

void ReadTable100::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
                                             this->m_table.m_buffer,
                                             this->m_table.BUFFER_SIZE);
    this->m_processScheduler.post( new ProcessTable100( this->m_table ) );
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_100_FLAG_A_BIT_OFFSET,
                                 Table560::TABLE_560_TABLE_100_FLAG_B_BIT_OFFSET );
}

ProcessTable100::ProcessTable100( Table100& table )
    : IEvent()
    ,  m_table( table )
{
}

ProcessTable100::~ProcessTable100()
{
}

void ProcessTable100::consume()
{
    this->m_table.processRead();
    this->m_table.notifyIscs();
    delete this;
}

void ProcessTable100::cancel()
{
    delete this;
}

} // namespace TA_IRS_App

