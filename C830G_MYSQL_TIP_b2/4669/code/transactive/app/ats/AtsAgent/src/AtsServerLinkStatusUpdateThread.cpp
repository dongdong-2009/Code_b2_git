// AtsServerLinkStatusUpdateThread.cpp: implementation of the AtsServerLinkStatusUpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#include "app/ats/AtsAgent/src/AtsServerLinkStatusUpdateThread.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/types/public_types/Constants.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    // OCC datapoint name is "OCC.SYS.ATS.ATSRCS.diiATS-NormalFallBack"
    const std::string ATS_LINK_DP_SUFFIX = ".diiATS-NormalFallBack";

    AtsServerLinkStatusUpdateThread::AtsServerLinkStatusUpdateThread()
        : m_dataPoint( NULL ),
          m_terminate( false ),
		  m_status( true ), /*to be consistent with initialization of m_atsOk*/
          m_semaphore( 0 )
    {
        FUNCTION_ENTRY( "AtsServerLinkStatusUpdateThread" );

        FUNCTION_EXIT;
    }


    AtsServerLinkStatusUpdateThread::~AtsServerLinkStatusUpdateThread()
    {
        FUNCTION_ENTRY( "~AtsServerLinkStatusUpdateThread" );

	    terminateAndWait();

        FUNCTION_EXIT;
    }


    bool AtsServerLinkStatusUpdateThread::setDataPoint( TA_Base_Bus::DataPoint* dataPoint )
    {
        FUNCTION_ENTRY( "setDataPoint" );

        if ( NULL == m_dataPoint )
        {
            std::string datapointName = dataPoint->getDataPointName();

            if ( std::string::npos != datapointName.rfind( ATS_LINK_DP_SUFFIX ) )
            {
                m_dataPoint = dataPoint;

                // set initial status
                m_semaphore.post();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "AtsServerLinkStatusUpdateThread::setDataPoint set to %s",
                             datapointName.c_str() );

                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    void AtsServerLinkStatusUpdateThread::setLinkStatus( bool status )
    {
        FUNCTION_ENTRY( "setLinkStatus" );

	    m_status = status;
	    m_semaphore.post();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"AtsServerLinkStatusUpdateThread::setLinkStatus set to %d", status);

        FUNCTION_EXIT;
    }

    
    void AtsServerLinkStatusUpdateThread::run()
    {
        FUNCTION_ENTRY( "run" );

	    while( false == m_terminate )
	    {
		    m_semaphore.wait();

            if ( NULL != m_dataPoint )
            {
                try
                {
                    // TODO: only update datapoint if value/quality changes.
                    TA_Base_Bus::DpValue dpValue( m_dataPoint->getDataPointDataType(),
                                                  m_dataPoint->getEnumLabelsMap(),
                                                  m_dataPoint->getBooleanLabels() );


                    dpValue.setBoolean( m_status );
                    
                    timeb newTimestamp = TA_Base_Core::defaultTime;
			        newTimestamp.time = time( NULL );
			        
                    TA_Base_Bus::DataPointState newState( dpValue,
                                                          newTimestamp,
                                                          TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
			        
                    m_dataPoint->updateFieldState( newState );

				    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "AtsServerLinkStatusUpdateThread - Datapoint status value set to %d",
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
                             "AtsServerLinkStatusUpdateThread - Datapoint not initialised" );
            }
        }

        FUNCTION_EXIT;
    }

    
    void AtsServerLinkStatusUpdateThread::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

	    m_terminate = true;
	    m_semaphore.post();

        FUNCTION_EXIT;
    }

}
