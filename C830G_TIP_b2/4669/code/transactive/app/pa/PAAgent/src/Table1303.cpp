/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table1303.cpp $
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
#include "app/pa/PAAgent/src/Table1303.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

using namespace TA_Base_Core;

const unsigned long TABLE_1303_RECORD_SIZE = 222;

const unsigned long TABLE_1303_CHIME_SUB_OFFSET = 0;
const unsigned long TABLE_1303_DVA_MESSAGE_1_SUB_OFFSET = 1;
const unsigned long TABLE_1303_DVA_MESSAGE_2_SUB_OFFSET = 3;
const unsigned long TABLE_1303_DVA_MESSAGE_3_SUB_OFFSET = 5;
const unsigned long TABLE_1303_DVA_MESSAGE_4_SUB_OFFSET = 7;
const unsigned long TABLE_1303_DWELL_SUB_OFFSET = 9;
const unsigned long TABLE_1303_PERIOD_SUB_OFFSET = 11;
const unsigned long TABLE_1303_START_TIME_SUB_OFFSET = 13;
const unsigned long TABLE_1303_STOP_TIME_SUB_OFFSET = 17;
const unsigned long TABLE_1303_EVENT_TRIGGERED_SUB_OFFSET = 21;
const unsigned long TABLE_1303_GLOBAL_COVERAGE_SUB_OFFSET = 22;

const unsigned long PAS_NO_MESSAGE_ID = 0;

namespace TA_IRS_App
{

// ExceptionChecked
Table1303::Table1303( unsigned long locationKey )
: AbstractPASTable(1303, 888),
    m_locationKey( locationKey )
{
    FUNCTION_ENTRY("Constructor");
    TA_ASSERT(CachedConfig::getInstance()->getIsMaster(), "Master only table created for station");

    m_messageSequenceDescriptions.resize(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ);

    FUNCTION_EXIT;
}

// ExceptionChecked
Table1303::~Table1303()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

void Table1303::processRead()
{
    ThreadGuard guard( m_lock );

    unsigned long offset = 0;
    unsigned long messageId = 0;
    ta_uint32 localCoverageBitmap = 0;

    for (int sequenceIdMinus1=0; sequenceIdMinus1 < TA_Base_Bus::IPAAgentCorbaDef::PA_MAXMSGSEQ; ++sequenceIdMinus1)
    {
        m_messageSequenceDescriptions[sequenceIdMinus1].m_hasChime = 
            PasHelpers::getInstance()->get8bit(m_buffer, TABLE_1303_CHIME_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1)?true:false;

        m_messageSequenceDescriptions[sequenceIdMinus1].m_dwellInSecs = 
            PasHelpers::getInstance()->get16bit(m_buffer, TABLE_1303_DWELL_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_periodInSecs = 
            PasHelpers::getInstance()->get16bit(m_buffer, TABLE_1303_PERIOD_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_startTime = 
            PasHelpers::getInstance()->get32bit(m_buffer, TABLE_1303_START_TIME_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_stopTime = 
            PasHelpers::getInstance()->get32bit(m_buffer, TABLE_1303_STOP_TIME_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);

        m_messageSequenceDescriptions[sequenceIdMinus1].m_isEventTriggered = 
            PasHelpers::getInstance()->get8bit(m_buffer, TABLE_1303_EVENT_TRIGGERED_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1)?true:false;

        // Convert local message Ids to message keys
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages.length(4);
        
        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_1303_DVA_MESSAGE_1_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[0] = getMessageKeyAndValidate(messageId);

        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_1303_DVA_MESSAGE_2_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[1] = getMessageKeyAndValidate(messageId);
    
        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_1303_DVA_MESSAGE_3_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[2] = getMessageKeyAndValidate(messageId);

        messageId = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_1303_DVA_MESSAGE_4_SUB_OFFSET + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);
        m_messageSequenceDescriptions[sequenceIdMinus1].m_messages[3] = getMessageKeyAndValidate(messageId);

        // Copy the global coverage bitmap and also Convert to PA Zone Keys

        unsigned long absoluteZoneId = 1;
        unsigned long offset = TABLE_1303_GLOBAL_COVERAGE_SUB_OFFSET;
        for (int stationId=1; stationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSTNID; ++stationId)
        {
            localCoverageBitmap = PasHelpers::getInstance()->get32bit(m_buffer, offset + TABLE_1303_RECORD_SIZE*sequenceIdMinus1);
            m_messageSequenceDescriptions[sequenceIdMinus1].m_globalCoverageBitmaps[stationId-1] = localCoverageBitmap;
            offset+=4;
        }
    
    
        PasHelpers::getInstance()->globalCoverageBitmapsToCoverage( 
            m_messageSequenceDescriptions[sequenceIdMinus1].m_globalCoverageBitmaps, 
            m_messageSequenceDescriptions[sequenceIdMinus1].m_zones);
    }
}

unsigned long Table1303::getMessageKeyAndValidate(unsigned long messageId)
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
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Invalid message Id in table 1303 returned from PAS" );
        }
    }

    // Not set
    return -1;

}

bool Table1303::hasStartTimeElapsed(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    return 
    ( m_messageSequenceDescriptions[sequenceId-1].m_startTime == 0 ||
      time(NULL) >= m_messageSequenceDescriptions[sequenceId-1].m_startTime);
}

TA_Base_Core::DateTime Table1303::getStartTime(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    return m_messageSequenceDescriptions[sequenceId-1].m_startTime;
}

TA_Base_Core::DateTime Table1303::getStopTime(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    return m_messageSequenceDescriptions[sequenceId-1].m_stopTime;
}

bool Table1303::isMessageSequenceCyclic(unsigned long sequenceId)
{
    ThreadGuard guard( m_lock );

    if ( m_messageSequenceDescriptions[sequenceId - 1].m_startTime != 0 ||
        m_messageSequenceDescriptions[sequenceId - 1].m_stopTime  != 0 )
    {
        return true;
    }

    return false;
}


ReadTable1303::ReadTable1303( Table1303& table,
                              Table560& table560,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable1303::~ReadTable1303()
{
}

void ReadTable1303::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable(this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    this->m_processScheduler.post( new ProcessTable1303( this->m_table ) );
    
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_1303_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_1303_FLAG_B_BIT_OFFSET );
}

ProcessTable1303::ProcessTable1303( Table1303& table )
    : m_table( table )
{
}

ProcessTable1303::~ProcessTable1303()
{
}

void ProcessTable1303::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable1303::cancel()
{
    delete this;
}



} // namespace TA_IRS_App

