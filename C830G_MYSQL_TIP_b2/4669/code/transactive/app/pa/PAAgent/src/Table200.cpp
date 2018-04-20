/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table200.cpp $
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
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/Table200.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/Table202.h"
#include "app/pa/PAAgent/src/Table302.h"
#include "app/pa/PAAgent/src/Table303.h"



using namespace TA_Base_Core;
using namespace TA_IRS_Bus;

const unsigned long TABLE_590_SOURCE_ID_SUB_OFFSET = 0;
const unsigned long TABLE_590_STATUS_AND_PRIORITY_SUB_OFFSET = 1;

namespace TA_IRS_App
{

// ExceptionChecked
Table200::Table200 (Table202* table202, Table302* table302, Table303* table303)
: AbstractPASTable(200, 64),
  m_cachedMapsInstance(NULL),
  m_pasHelpersInstance(NULL),
  m_readWait(),
  m_initialThread(true)// TD17013
{
    FUNCTION_ENTRY("Constructor");

    m_cachedMapsInstance = CachedMaps::getInstance();
    m_pasHelpersInstance = PasHelpers::getInstance();

    m_zoneStatuses.resize(TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID);

    m_table202 = table202;
    m_table302 = table302;
    m_table303 = table303;

	// jeffrey++ Bug457
	m_readIndex = 0;
	// ++jeffrey Bug457
    FUNCTION_EXIT;
}

// ExceptionChecked
Table200::~Table200()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}


// TD 17013
void Table200::setInitalThreadValue(bool initialThreadValue)
{
	m_initialThread = initialThreadValue;
}
// TD 17013


// TD 16749
void Table200::processRead()
{
    ThreadGuard guard( m_lock );
	
    for (int zone=0; zone < TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID; ++zone)
    {
        // Zone data is expected to be in the following format
        //
        // Bit: 15 14 13 12 11 10 09 08 07  06 05 04 03 02 01 00
        //      |_____________________|  |  |__________________|
        //                |              |            |
        //           Source Id        Status       Priority
        //
        ta_uint16 zoneData = m_pasHelpersInstance->get16bit(m_buffer, zone*2);
		
        CORBA::Octet sourceId = static_cast<CORBA::Octet>(zoneData>>8);
        bool status = ( zoneData & (0x0080) ) != 0;
        CORBA::Octet priority = static_cast<CORBA::Octet>(zoneData & (0x007f));
		
        // The datapoints will only update if the DataPointState has changed
        // (with the exception of just a timestamp change).  Therefore to
        // simplify the code, we update the datapoint regardless.
		
		
        // Update broadcast icon datapoints
		if(m_zoneStatuses[zone].m_sourceId != sourceId || m_initialThread)// TD17013
		{
			m_zoneStatuses[zone].m_sourceId = sourceId;
			
			TA_Base_Bus::DataPoint* liveDp   = m_cachedMapsInstance->getLiveDataPointFromZoneId(zone+1);
			TA_Base_Bus::DataPoint* cyclicDp = m_cachedMapsInstance->getCyclicDataPointFromZoneId(zone+1);
			TA_Base_Bus::DataPoint* musicDp  = m_cachedMapsInstance->getMusicDataPointFromZoneId(zone+1);
			
			// toan++
			// TD-10381
			// 
			// Fixed 23/06/2005: Set good quality to all broadcast datapoints
			//                   Except the fix mentioned above, there's no change 
			//                   in the logic. The code was only made shorter by
			//                   moving out the duplicated parts to the end.
			//
			//                   Note: if source id is an unexpected value -> no update. 
			//                   Don't know if this is correct. It is the original 
			//                   logic.
			
			// determine the state of each broadcast
			bool bLive = false;
			bool bMusic = false;
			bool bCyclic = false;
			bool bRecognisedSourceId = true;    // false if value of source id is unexpected
			
			if ( sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_STATION_SO ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_DEPOT_DCO ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_DEPOT_DPSCO ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_CS ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_TCO_1 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_TCO_2 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_TCO_3 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_SCO_1 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_SCO_2 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_MCO_1 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_MCO_2 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_MFT_OCR_PSCO ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_SPARE_1 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_SPARE_2 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_PCP_STATION_1 ||
				sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_PAGING_CONSOLE_PCP_STATION_2 )
			{
				bLive = true;
			}
			else
			{
				if (sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_OCR_MUSIC ||
					sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_STATION_MUSIC ||
					sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DEPOT_MUSIC )
				{
					bMusic = true;
				}
				else
				{
					// jeffrey++ Bug457
					if (sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA1_SET_A ||
						sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA1_SET_B_D ||
						sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA2_SET_A ||
						sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA2_SET_B_D )
						//if (sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA_SET_A ||
						//    sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA_SET_B ||
						//    sourceId == TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DVA_SET_D )
						// ++jeffrey Bug457
					{
						if (cyclicDp)
						{
							// Cross reference tables to determine whether the
							// zone has a cyclic message sequence being played on it
							CORBA::Octet announcementId = m_table202->getAnnounceId(sourceId);
							unsigned int sequenceId = m_table302->getSequenceId(announcementId);
							bCyclic = m_table303->isMessageSequenceCyclic(sequenceId);
						}
					}
					else
					{
						if ( sourceId == 0 )
						{
							// The zone is free. As in the original code, we assign 
							// "false" to bLive, bCyclic, bMusic. However, they 
							// were initialized with "false" already.
						}
						else
						{
							// unexpected value -> no update. Don't know if this is 
							// correct. It is the logic of original version.
							//
							// As in the old code, we don't update the datapoints
							// of live, music and cyclic broadcasts if the value 
							// of source id is an unexpected one
							// 
							bRecognisedSourceId = false;
						}
					}
				}
			}
			
			if ( bRecognisedSourceId )
			{
				// update the music, live and cyclic broadcast datapoints
				if (cyclicDp)
				{
					TA_Base_Bus::DpValue dpValue(cyclicDp->getDataPointDataType(),
						cyclicDp->getEnumLabelsMap(),
						cyclicDp->getBooleanLabels());
					dpValue.setBoolean ( bCyclic );
					TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
					cyclicDp->updateFieldState( newState );
				}
				if (liveDp)
				{
					TA_Base_Bus::DpValue dpValue(liveDp->getDataPointDataType(),
						liveDp->getEnumLabelsMap(),
						liveDp->getBooleanLabels());
					dpValue.setBoolean ( bLive );
					TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
					liveDp->updateFieldState( newState );
				}
				if (musicDp)
				{
					TA_Base_Bus::DpValue dpValue(musicDp->getDataPointDataType(),
						musicDp->getEnumLabelsMap(),
						musicDp->getBooleanLabels());
					dpValue.setBoolean ( bMusic );
					TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
					musicDp->updateFieldState( newState );
				}
			}
			// ++toan
			// TD-10381
		}
		
        // update status datapoint
		if(m_zoneStatuses[zone].m_status != status || m_initialThread)// TD17013
		{
			m_zoneStatuses[zone].m_status = status;
			TA_Base_Bus::DataPoint* statusDp = m_cachedMapsInstance->getStatusDataPointFromZoneId(zone+1);
			if (statusDp)
			{
				// PA Zone datapoint exists, so we can update it
				TA_Base_Bus::DpValue dpValue(statusDp->getDataPointDataType(),
					statusDp->getEnumLabelsMap(),
					statusDp->getBooleanLabels());
				
				dpValue.setBoolean ( status );
				
				// set up DataPointState with the default timestamp and quality
				TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				
				statusDp->updateFieldState( newState );
			}
		}
		
        // update priority datapoint
		if(m_zoneStatuses[zone].m_priority != priority || m_initialThread)// TD17013
		{
			m_zoneStatuses[zone].m_priority = priority;
			TA_Base_Bus::DataPoint* priorityDp = m_cachedMapsInstance->getPriorityDataPointFromZoneId(zone+1);
			if (priorityDp)
			{
				// PA Zone datapoint exists, so we can update it
				TA_Base_Bus::DpValue dpValue(priorityDp->getDataPointDataType(),
					priorityDp->getEnumLabelsMap(),
					priorityDp->getBooleanLabels());
				
				dpValue.setFloat ( priority, priorityDp->getPrecision() );
				
				// set up DataPointState with the default timestamp and quality
				TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				
				priorityDp->updateFieldState( newState );
			}
        }
		
    }
	
	// jeffrey++ Bug457
	m_readIndex++;
	// ++jeffrey Bug457
    this->m_readWait.broadcast();

	m_initialThread = false;// TD17013

}
// TD 16749

// ++ Bug456
void Table200::readTimedWait()
{
	this->m_readWait.timedWait( CachedConfig::getInstance()->getFastPollRateInMSecs() );
}
// ++ Bug456

ZoneStatus Table200::getZoneStatus(unsigned int zoneId)
    //throw ( TA_Base_Core::PasConnectionException )
{
    TA_ASSERT( zoneId > 0 && zoneId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID, "zoneId is out of range");

	// ++ Bug456
    //this->m_readWait.timedWait( CachedConfig::getInstance()->getFastPollRateInMSecs() );
	// ++ Bug456

    ThreadGuard guard( m_lock );

    return m_zoneStatuses[zoneId-1];
}



void invalidateDataPoint(TA_Base_Bus::DataPoint& datapoint)
{
    TA_Base_Bus::DpValue dpValue=datapoint.getCurrentValue();
    TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );
    datapoint.updateFieldState( newState );
}

void Table200::invalidateAllDataPoints()
{
    for (int zone=0; zone < TA_Base_Bus::IPAAgentCorbaDef::PA_MAXZONEID; ++zone)
    {

        try
        {
            TA_Base_Bus::DataPoint* liveDp   = m_cachedMapsInstance->getLiveDataPointFromZoneId(zone+1);
            if (liveDp)
            {
                invalidateDataPoint(*liveDp);
            }

            TA_Base_Bus::DataPoint* musicDp  = m_cachedMapsInstance->getMusicDataPointFromZoneId(zone+1);
            if (musicDp)
            {
                invalidateDataPoint(*musicDp);
            }

            TA_Base_Bus::DataPoint* cyclicDp = m_cachedMapsInstance->getCyclicDataPointFromZoneId(zone+1);
            if (cyclicDp)
            {
                invalidateDataPoint(*cyclicDp);
            }

            TA_Base_Bus::DataPoint* statusDp = m_cachedMapsInstance->getStatusDataPointFromZoneId(zone+1);
            if (statusDp)
            {
                invalidateDataPoint(*statusDp);
            }

            TA_Base_Bus::DataPoint* priorityDp = m_cachedMapsInstance->getPriorityDataPointFromZoneId(zone+1);
            if (priorityDp)
            {
                invalidateDataPoint(*priorityDp);
            }
        }
        catch(...)
        {
            // Carry on
        }
    }
}

ReadTable200::ReadTable200( Table200& table,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

ReadTable200::ReadTable200( ACE_Time_Value expiryTime,
                            Table200& table,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( expiryTime, socketScheduler, processScheduler, eventSource )
    , m_table( table )
{
}

ReadTable200::~ReadTable200()
{
}

void ReadTable200::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    ACE_Time_Value nextReadTime(this->m_timeToExpire + ACE_Time_Value( 0, CachedConfig::getInstance()->getFastPollRateInMSecs() * 1000 ));
    if (nextReadTime < ACE_OS::gettimeofday())
    {
        nextReadTime = ACE_OS::gettimeofday();
    }

    this->m_socketScheduler.post( new ReadTable200( nextReadTime,
        this->m_table,
        this->m_socketScheduler,
        this->m_processScheduler,
        this->m_pasEventSource ) );
    
    this->m_processScheduler.post( new ProcessTable200( this->m_table ) );
}

ProcessTable200::ProcessTable200( Table200& table )
    : m_table( table )
{
}

ProcessTable200::~ProcessTable200()
{
}

void ProcessTable200::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable200::cancel()
{
    delete this;
}


InvalidateTable200::InvalidateTable200( Table200& table )
    : m_table( table )
{
}

InvalidateTable200::~InvalidateTable200()
{
}

void InvalidateTable200::consume()
{
    this->m_table.invalidateAllDataPoints();

    delete this;
}

void InvalidateTable200::cancel()
{
    delete this;
}


} // namespace TA_IRS_App

