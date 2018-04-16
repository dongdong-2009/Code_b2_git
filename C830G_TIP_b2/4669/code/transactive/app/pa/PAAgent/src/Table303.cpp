/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table303.cpp $
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
#include "app/pa/PAAgent/src/Table303.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"


using namespace TA_Base_Core;

const unsigned long TABLE_303_RECORD_SIZE = 26;

const unsigned long TABLE_303_CHIME_SUB_OFFSET = 0;
const unsigned long TABLE_303_DVA_MESSAGE_1_SUB_OFFSET = 1;
const unsigned long TABLE_303_DVA_MESSAGE_2_SUB_OFFSET = 3;
const unsigned long TABLE_303_DVA_MESSAGE_3_SUB_OFFSET = 5;
const unsigned long TABLE_303_DVA_MESSAGE_4_SUB_OFFSET = 7;
const unsigned long TABLE_303_DWELL_SUB_OFFSET = 9;
const unsigned long TABLE_303_PERIOD_SUB_OFFSET = 11;
const unsigned long TABLE_303_START_TIME_SUB_OFFSET = 13;
const unsigned long TABLE_303_STOP_TIME_SUB_OFFSET = 17;
const unsigned long TABLE_303_EVENT_TRIGGERED_SUB_OFFSET = 21;
const unsigned long TABLE_303_LOCAL_COVERAGE_SUB_OFFSET = 22;

const unsigned long PAS_NO_MESSAGE_ID = 0;


namespace TA_IRS_App
{

// ExceptionChecked
Table303::Table303( unsigned long locationKey )
: AbstractPASTable(303, 104),
  m_locationKey( locationKey )
{
    FUNCTION_ENTRY("Constructor");

    m_messageSequenceDescriptions.resize(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ);

    FUNCTION_EXIT;
}

// ExceptionChecked
Table303::~Table303()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

void Table303::processRead()
{
    ThreadGuard guard( m_lock );

    unsigned long offset = 0;
    unsigned long messageId = 0;
    unsigned long messageKey = 0;

    for (int sequenceIdMinus1=0; sequenceIdMinus1 < TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ; ++sequenceIdMinus1)
    {
        m_messageSequenceDescriptions[sequenceIdMinus1].m_hasChime = 
            PasHelpers::getInstance()->get8bit(m_buffer, TABLE_303_CHIME_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1)?true:false;

        m_messageSequenceDescriptions[sequenceIdMinus1].m_dwellInSecs = 
            PasHelpers::getInstance()->get16bit(m_buffer, TABLE_303_DWELL_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_periodInSecs = 
            PasHelpers::getInstance()->get16bit(m_buffer, TABLE_303_PERIOD_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_startTime = 
            PasHelpers::getInstance()->get32bit(m_buffer, TABLE_303_START_TIME_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_stopTime = 
            PasHelpers::getInstance()->get32bit(m_buffer, TABLE_303_STOP_TIME_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_isEventTriggered = 
            PasHelpers::getInstance()->get8bit(m_buffer, TABLE_303_EVENT_TRIGGERED_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1)?true:false;

        // Convert local message Ids to message keys
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages.length(4);
        
        
        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_303_DVA_MESSAGE_1_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[0] = getMessageKeyAndValidate(messageId);

        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_303_DVA_MESSAGE_2_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[1] = getMessageKeyAndValidate(messageId);

        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_303_DVA_MESSAGE_3_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[2] = getMessageKeyAndValidate(messageId);

        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_303_DVA_MESSAGE_4_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[3] =getMessageKeyAndValidate(messageId);


        m_messageSequenceDescriptions[sequenceIdMinus1].m_localCoverageBitmap = 
            PasHelpers::getInstance()->get32bit(m_buffer, TABLE_303_LOCAL_COVERAGE_SUB_OFFSET + TABLE_303_RECORD_SIZE*sequenceIdMinus1);

        // Convert local coverage bitmap to PA Zone Keys
        
        PasHelpers::getInstance()->localCoverageBitmapToCoverage( m_messageSequenceDescriptions[sequenceIdMinus1].m_localCoverageBitmap, 
                                                   m_messageSequenceDescriptions[sequenceIdMinus1].m_zones, 
                                                   m_locationKey);
    }

}

unsigned long Table303::getMessageKeyAndValidate(unsigned long messageId)
{
    if (messageId != PAS_NO_MESSAGE_ID)
    {
        try
        {
            return TA_IRS_Bus::CachedMaps::getInstance()->getKeyFromStationDvaMessageIdAndLocation(messageId, m_locationKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException& e)
        {
            // Received invalid data
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Invalid message Id in table 303 returned from PAS" );
        }
    }

    // Not set
    return -1;
}

bool Table303::hasStartTimeElapsed(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    return 
    ( m_messageSequenceDescriptions[sequenceId-1].m_startTime == 0 ||
      time(NULL) >= m_messageSequenceDescriptions[sequenceId-1].m_startTime);
}

TA_Base_Core::DateTime Table303::getStartTime(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    return m_messageSequenceDescriptions[sequenceId-1].m_startTime;
}

TA_Base_Core::DateTime Table303::getStopTime(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    return m_messageSequenceDescriptions[sequenceId-1].m_stopTime;
}

bool Table303::isMessageSequenceCyclic(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    if ( m_messageSequenceDescriptions[sequenceId - 1].m_startTime != 0 ||
        m_messageSequenceDescriptions[sequenceId - 1].m_stopTime  != 0 )
    {
        return true;
    }

    return false;
}


ReadTable303::ReadTable303( Table303& table, 
                            Table560& table560,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable303::~ReadTable303()
{
}

void ReadTable303::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    this->m_processScheduler.post( new ProcessTable303( this->m_table ) );
    
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_303_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_303_FLAG_B_BIT_OFFSET );
}

ProcessTable303::ProcessTable303( Table303& table )
    : m_table( table )
{
}

ProcessTable303::~ProcessTable303()
{
}

void ProcessTable303::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable303::cancel()
{
    delete this;
}



} // namespace TA_IRS_App

