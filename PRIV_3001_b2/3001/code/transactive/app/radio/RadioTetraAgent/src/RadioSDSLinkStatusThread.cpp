/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: \\depot\3001_TIP_NEW\3001\transactive\app\radio\RadioTetraAgent\src\RadioSDSLinkStatusThread.cpp $
 * @author:  Lucky Cuizon
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/11/03 15:18:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This class handles the Datapoint updates for SDS Server Link status
 */

#include "app/radio/RadioTetraAgent/src/RadioSDSLinkStatusThread.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/types/public_types/Constants.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	RadioSDSLinkStatusThread::RadioSDSLinkStatusThread()
		: m_dataPoint(NULL),
		  m_terminate(false),
		  m_status(false),
		  m_semaphore(0)
	{
	}

	RadioSDSLinkStatusThread::~RadioSDSLinkStatusThread()
	{
		terminateAndWait();
	}

	void RadioSDSLinkStatusThread::setDataPointConfig( DataPointList& dpList, RadioSDSSummary* sdsSummary )
	{
		std::string dpName = sdsSummary->getSDTSLinkDpName();
		DataPointList::iterator iter = dpList.begin();
		for ( iter; iter != dpList.end(); ++iter )
		{
			if ( (*iter).second->getDataPointName().find(dpName) != std::string::npos )
			{
				setDataPoint((*iter).second);
				start();
				break;
			}
		}
	}


	void RadioSDSLinkStatusThread::setDataPoint( TA_Base_Bus::DataPoint* dataPoint )
	{
		std::string datapointName = dataPoint->getDataPointName();

		m_dataPoint = dataPoint;

		// set initial status
		m_semaphore.post();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"RadioSDSLinkStatusThread::setDataPoint set to %s",
			datapointName.c_str() );
	}

	void RadioSDSLinkStatusThread::setLinkStatus( bool status )
	{
		TA_THREADGUARD(m_statusLock);

		if ( NULL != m_dataPoint )
		{
			m_status = status;

			m_semaphore.post();

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"SetLinkStatus : %d",status );
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Cannot set SDS Link Status, Datapoint is not exist.");
		}
		
	}

	void RadioSDSLinkStatusThread::run()
	{
		while( false == m_terminate )
		{
			m_semaphore.wait();

			if ( NULL != m_dataPoint )
			{
				try
				{
					TA_THREADGUARD(m_statusLock);

					TA_Base_Bus::DpValue dpValue( m_dataPoint->getDataPointDataType(),
						m_dataPoint->getEnumLabelsMap(),
						m_dataPoint->getBooleanLabels() );

					dpValue.setBoolean( m_status );

					timeb newTimestamp = TA_Base_Core::defaultTime;
					newTimestamp.time = time( NULL );

					TA_Base_Bus::DataPointState newState( dpValue,
						newTimestamp,TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

					m_dataPoint->updateFieldState( newState );

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"RadioSDSLinkStatusThread - Datapoint status value set to %d",
						m_status );
				}
				catch( TA_Base_Core::TransactiveException& e )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
				}
				catch( ... )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "...", "While setting server status datapoint" );
				}
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
					"RadioSDSLinkStatusThread - Datapoint not initialised" );
			}
		}
	}

	void RadioSDSLinkStatusThread::terminate()
	{
		m_terminate = true;
		m_semaphore.post();
	}
}