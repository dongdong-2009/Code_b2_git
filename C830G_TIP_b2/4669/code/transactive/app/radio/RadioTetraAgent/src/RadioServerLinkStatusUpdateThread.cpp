// RadioServerLinkStatusUpdateThread.cpp: implementation of the RadioServerLinkStatusUpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#include "RadioServerLinkStatusUpdateThread.h"
#include "RadioAgentConstants.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/types/public_types/Constants.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioServerLinkStatusUpdateThread::RadioServerLinkStatusUpdateThread(): 
m_dataPoint( NULL ),
m_terminate(false),
m_DpServerStatus(6)
{
	FUNCTION_ENTRY( "RadioServerLinkStatusUpdateThread" );

	FUNCTION_EXIT;
}

RadioServerLinkStatusUpdateThread::~RadioServerLinkStatusUpdateThread()
{
	FUNCTION_ENTRY( "~RadioServerLinkStatusUpdateThread" );

	FUNCTION_EXIT;
}

bool RadioServerLinkStatusUpdateThread::setDataPoint( TA_Base_Bus::DataPoint* dataPoint )
{
    FUNCTION_ENTRY( "setDataPoint" );

    if ( NULL == m_dataPoint )
    {
        std::string datapointName = dataPoint->getDataPointName();

        if ( std::string::npos != datapointName.rfind( TA_IRS_App::RadioAgentConstants::RADIO_LINK_DP_SUFFIX ) )
        {
            m_dataPoint = dataPoint;

            // set initial status
            m_semaphore.post();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Radio Server Link Status Datapoint set to %s",
                         datapointName.c_str() );

            FUNCTION_EXIT;
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}

void RadioServerLinkStatusUpdateThread::run()
{
	FUNCTION_ENTRY( "run" );

	while(!m_terminate)
	{
		m_semaphore.wait();

        if ( NULL != m_dataPoint )
		{
			try
			{
				TA_THREADGUARD(m_setValueLock)

                TA_Base_Bus::DpValue dpValue( m_dataPoint->getDataPointDataType(),
                                              m_dataPoint->getEnumLabelsMap(),
                                              m_dataPoint->getBooleanLabels() );

				dpValue.setFloat(m_DpServerStatus.to_ulong());
                
                timeb newTimestamp = TA_Base_Core::defaultTime;
			    newTimestamp.time = time( NULL );

                TA_Base_Bus::DataPointState newState( dpValue,
                                                      newTimestamp,
                                                      TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

                m_dataPoint->updateFieldState( newState );

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "RadioServerLinkStatusUpdateThread - Datapoint status value set to %d", m_DpServerStatus.to_ulong());

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
	                     "RadioServerLinkStatusUpdateThread - Datapoint not initialised" );
	    }
	}
}

void RadioServerLinkStatusUpdateThread::terminate()
{
	m_terminate = true;
	m_semaphore.post();
}

void RadioServerLinkStatusUpdateThread::setLinkStatus(DpServerStatus statusCode, bool value)
{
	TA_THREADGUARD(m_setValueLock)

	m_DpServerStatus[statusCode] = value;
	m_semaphore.post();
}

void RadioServerLinkStatusUpdateThread::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
{
	return;
}
