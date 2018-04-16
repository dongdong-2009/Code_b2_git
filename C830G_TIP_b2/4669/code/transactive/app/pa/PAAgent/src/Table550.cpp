/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table550.cpp $
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
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/pa/PAAgent/src/Table550.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/CachedConfig.h"



using namespace TA_Base_Core;

const unsigned int TABLE_550_WORD_OFFSET = 0;
const unsigned int TABLE_550_SUMMARY_OF_MAJOR_ALARMS_BIT_OFFSET  = 0;
const unsigned int TABLE_550_SUMMARY_OF_MINOR_ALARMS_BIT_OFFSET  = 1;

namespace TA_IRS_App
{

// ExceptionChecked
Table550::Table550 ()
: AbstractPASTable(550, 18)
{
    FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}

// ExceptionChecked
Table550::~Table550()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

void Table550::processRead()
{
    ThreadGuard guard( m_lock ); // ReEntrant

    ta_uint16 wordData = PasHelpers::getInstance()->get16bit(m_buffer, TABLE_550_WORD_OFFSET);

    {
        // Update major summary alarm datapoint
        TA_Base_Bus::DataPoint* dp = CachedConfig::getInstance()->getMajorSummaryAlarmDP();
        if (dp)
        {
            TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(),
										 dp->getEnumLabelsMap(),
										 dp->getBooleanLabels());
            bool alarmState = (wordData & ( 1 << TABLE_550_SUMMARY_OF_MAJOR_ALARMS_BIT_OFFSET )) != 0;
            dpValue.setBoolean ( alarmState );
            // set up DataPointState with the default timestamp and quality
            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

	        dp->updateFieldState( newState );
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError,
                "Major PA equipment alarm detected but datapoint not found" );
        }
    }

    {
        // Update minor summary alarm datapoint
        TA_Base_Bus::DataPoint* dp = CachedConfig::getInstance()->getMinorSummaryAlarmDP();
        if (dp)
        {
            TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(),
										 dp->getEnumLabelsMap(),
										 dp->getBooleanLabels());
            bool alarmState = (wordData & ( 1 << TABLE_550_SUMMARY_OF_MINOR_ALARMS_BIT_OFFSET )) != 0;
            dpValue.setBoolean ( alarmState );
            // set up DataPointState with the default timestamp and quality
            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

	        dp->updateFieldState( newState );
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError,
                "Minor PA equipment alarm detected but datapoint not found" );
        }
    }

}

void invalidateDataPoint( TA_Base_Bus::DataPoint& );

void Table550::invalidateAllDataPoints()
{
    try
    {
        TA_Base_Bus::DataPoint* majorDp = CachedConfig::getInstance()->getMajorSummaryAlarmDP();
        if (majorDp)
        {
            invalidateDataPoint(*majorDp);
        }

        TA_Base_Bus::DataPoint* minorDp = CachedConfig::getInstance()->getMinorSummaryAlarmDP();
        if (minorDp)
        {
            invalidateDataPoint(*minorDp);
        }
    }
    catch(...)
    {
        // Carry on
    }
}


ReadTable550::ReadTable550( Table550& table,
                            Table560& table560,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable550::~ReadTable550()
{
}

void ReadTable550::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    this->m_processScheduler.post( new ProcessTable550( this->m_table ) );
    
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_550_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_550_FLAG_B_BIT_OFFSET );
}

ProcessTable550::ProcessTable550( Table550& table )
    : m_table( table )
{
}

ProcessTable550::~ProcessTable550()
{
}

void ProcessTable550::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable550::cancel()
{
    delete this;
}



InvalidateTable550::InvalidateTable550( Table550& table )
    : m_table( table )
{
}

InvalidateTable550::~InvalidateTable550()
{
}

void InvalidateTable550::consume()
{
    this->m_table.invalidateAllDataPoints();

    delete this;
}

void InvalidateTable550::cancel()
{
    delete this;
}


} // namespace TA_IRS_App

