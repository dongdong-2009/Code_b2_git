// HeartBeatWork.cpp: implementation of the HeartBeatWork class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>
#include "app/scada/PMSAgent/src/HeartBeatWorker.h"
#include "app/scada/PMSAgent/src/RTU.h"
#include "app/scada/PMSAgent/src/StationSystemOutputWriter.h"

#include "bus/scada/common_library/src/BlockingSingletonMap.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/sockets/src/SocketOptions.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

using TA_Base_Core::DebugUtil;

namespace
{
    bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
    {
        bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
                     TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
                     TA_Base_Bus::NotLoaded == sessionID );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
        return ret;
    }
}


namespace TA_IRS_App
{

	HeartBeatWorker::HeartBeatWorker() 
        : m_keepRunning(true),
          m_operationMode( TA_Base_Core::NotApplicable ) //limin, fail over issue
	{
	}

	HeartBeatWorker::~HeartBeatWorker()
	{
		m_rtus.clear();
		terminateAndWait();
	}

	void HeartBeatWorker::run()
	{
		TA_Base_Bus::DataPointWriteRequest * newHeartBeat = NULL;
		
		m_keepRunning = true; 
		
		while ( m_keepRunning )
		{
			newHeartBeat = NULL;
			
			try
			{
				newHeartBeat = TA_Base_Bus::DataPointWriteMap::getInstance()->blockForItem();
				
                if ( NULL != newHeartBeat )
                {
                    if ( TA_Base_Core::Control == m_operationMode ) //limin, failover issue
                    {
                        processEvent( newHeartBeat );
                    }
                }
			}
			// Catch all exceptions to prevent thread from dying prematurely
			catch( const TA_Base_Core::TransactiveException & te )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", te.what() );
			}
			catch( const std::exception & e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
			}
			catch( ... )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
			}
			
			if (NULL != newHeartBeat)
			{
				delete newHeartBeat;
				newHeartBeat = NULL;
			}

			TA_Base_Core::Thread::sleep( 0 );
		}
	};


	void HeartBeatWorker::terminate()
	{
		m_keepRunning = false;
        TA_Base_Bus::DataPointWriteMap::getInstance()->unBlockQueue();
	};


	void HeartBeatWorker::processEvent( TA_Base_Bus::DataPointWriteRequest * newHeartBeat )
	{
        FUNCTION_ENTRY("processEvent");
		
        bool requestProcessed = false;
		
		for ( std::vector<RTU *>::iterator itr = m_rtus.begin(); itr != m_rtus.end() && requestProcessed == false; ++itr )
		{
			if((* itr)->processWriteRequest( *newHeartBeat))
			{
				requestProcessed = true;
			}
		}
		
        if ( false == requestProcessed )
        {
            TA_Base_Bus::DataPoint * dp = newHeartBeat->getDataPoint();            
			
			if ( dp )
			{
				dp->updateControlState( TA_Base_Bus::comms_failed, isSystemSessionID( newHeartBeat->getSessionID() ) );//limin++, SystemSession should not access db
			}			
        }
        
        FUNCTION_EXIT;		
	}


	void HeartBeatWorker::addRTU(RTU& rtu)
	{
		m_rtus.push_back(&rtu);
	}


    //limin++, failover issue
    void HeartBeatWorker::setToControl()
    {
        m_operationMode = TA_Base_Core::Control;
    }

    
    void HeartBeatWorker::setToMonitor()
    {
        m_operationMode = TA_Base_Core::Monitor;
    }
    //++limin, failover issue

}