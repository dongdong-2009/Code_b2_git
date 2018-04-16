/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table570.cpp $
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
#include "app/pa/PAAgent/src/Table570.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/CachedConfig.h"


using namespace TA_Base_Core;

namespace TA_IRS_App
{

// ExceptionChecked
Table570::Table570( TA_IRS_Bus::Scheduler& socketScheduler, TA_IRS_Bus::Scheduler& processScheduler, IPasEventSource& eventSource )
    : AbstractWriteablePASTable(570, 1)
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_pasEventSource( eventSource )
	// Maochun Sun++
	// bug774 (TD13071)
	, m_writeBuffer(new ta_uint8[BUFFER_SIZE]),
	// ++Maochun Sun
	// bug774 (TD13071)
	m_isOnFireAlarm(false) 	// TD18294
{
    FUNCTION_ENTRY("Constructor");

	memset( m_writeBuffer, 0, BUFFER_SIZE );
	
    FUNCTION_EXIT;
}

// ExceptionChecked
Table570::~Table570()
{
    FUNCTION_ENTRY("Destructor");

    delete [] m_writeBuffer;

    FUNCTION_EXIT;
}

void Table570::processRead()
{
    ThreadGuard guard( m_lock );

    // TD15174
	if (!CachedConfig::getInstance()->getIsDepot() && !CachedConfig::getInstance()->getIsMaster())
    {
		TA_Base_Bus::DataPoint* dp = CachedConfig::getInstance()->getFireCountdownActiveDP();
		if (dp)
		{
			TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(),
				dp->getEnumLabelsMap(),
				dp->getBooleanLabels());
			if ( m_buffer[0] != 0 )
			{
				dpValue.setBoolean ( true );
				// TD18294
				m_isOnFireAlarm = true;
				// TD18294
			}
			else
			{
				dpValue.setBoolean ( false );
				// TD18294
				m_isOnFireAlarm = false;
				// TD18294
			}
			
			// set up DataPointState with the default timestamp and quality
			TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
			
			dp->updateFieldState( newState );
		}
    }
    // TD15174
}

// TD18294
bool Table570::isOnFireAlarmState()
{
	return m_isOnFireAlarm;
}
// TD18294


void Table570::resetFireAlarmStatusAndWrite()
{
    ThreadGuard guard( m_lock );

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
        "Resetting FireAlarmStatus on FEPC");


	// Maochun Sun++
	// bug774 (TD13071)

    //m_buffer[0] = 0;
	m_writeBuffer[0] = 0;

	// ++Maochun Sun
	// bug774 (TD13071)

    WriteTable570* event = new WriteTable570( *this,
                                              this->m_socketScheduler,
                                              this->m_processScheduler,
                                              this->m_pasEventSource );
    this->addWriteEvent( *event );
    this->m_socketScheduler.post( event );
}

void invalidateDataPoint( TA_Base_Bus::DataPoint& );

void Table570::invalidateAllDataPoints()
{
    try
    {
		// TD15174
		if (!CachedConfig::getInstance()->getIsDepot() && !CachedConfig::getInstance()->getIsMaster())
		{
			TA_Base_Bus::DataPoint* dp = CachedConfig::getInstance()->getFireCountdownActiveDP();
			if (dp)
			{
				invalidateDataPoint(*dp);
			}
		}
		// TD15174
    }
    catch(...)
    {
        // Carry on
    }
}


ReadTable570::ReadTable570( Table570& table,
                            Table560& table560,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )

{
}

ReadTable570::~ReadTable570()
{
}

void ReadTable570::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    this->m_processScheduler.post( new ProcessTable570( this->m_table ) );
    
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_570_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_570_FLAG_B_BIT_OFFSET );
}

ProcessTable570::ProcessTable570( Table570& table )
    : m_table( table )
{
}

ProcessTable570::~ProcessTable570()
{
}

void ProcessTable570::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable570::cancel()
{
    delete this;
}

WriteTable570::WriteTable570( Table570& table,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASWriteEvent( table, socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

WriteTable570::WriteTable570( ACE_Time_Value timeToExpire,
                              Table570& table,
                              TA_IRS_Bus::Scheduler& socketScheduler,
                              TA_IRS_Bus::Scheduler& processScheduler,
                              IPasEventSource& eventSource )
    : PASWriteEvent( timeToExpire, table, socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

WriteTable570::~WriteTable570()
{
}

void WriteTable570::writeTable()
{
    ThreadGuard guard( this->m_table.m_lock ); 
    
    PASConnection::getInstance()->writeTable(this->m_table.TABLE_NUMBER,
		// Maochun Sun++
		// bug774 (TD13071)

		//this->m_table.m_buffer,
		this->m_table.m_writeBuffer,

		// ++Maochun Sun
		// bug774 (TD13071)
        this->m_table.BUFFER_SIZE);
}

InvalidateTable570::InvalidateTable570( Table570& table )
    : m_table( table )
{
}

InvalidateTable570::~InvalidateTable570()
{
}

void InvalidateTable570::consume()
{
    this->m_table.invalidateAllDataPoints();

    delete this;
}

void InvalidateTable570::cancel()
{
    delete this;
}

PASWriteEvent* WriteTable570::recreateEvent(ACE_Time_Value timeToExpire)
{
    return new WriteTable570(timeToExpire,
        this->m_table,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource);
}

} // namespace TA_IRS_App

