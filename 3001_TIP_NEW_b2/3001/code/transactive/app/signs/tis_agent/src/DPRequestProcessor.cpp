// DPRequestProcessor.cpp: implementation of the DPRequestProcessor class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif // _MSC_VER

#include "app/signs/tis_agent/src/DPRequestProcessor.h"
#include "app/signs/tis_agent/src/STISManager.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	DPRequestProcessor::DPRequestProcessor()
	{
		m_writeQ = TA_Base_Bus::DataPointWriteQueue::getInstance();
	}
	
	DPRequestProcessor::~DPRequestProcessor()
	{
		
	}
	
	
	void DPRequestProcessor::processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest )
	{
		   
        TA_Base_Bus::DataPoint * dp = newDataPointWriteRequest->getDataPoint();     
		try
		{
			std::string str = dp->getDataPointName();
			STISManager::getInstance()->submitPIDControlRequest(str.substr(0, str.find_last_of('.')).c_str(), 
				(0 == newDataPointWriteRequest->getValue().getValueAsString().compare("ON")) ? TA_Base_Core::TURN_ON : TA_Base_Core::TURN_OFF,
				newDataPointWriteRequest->getSessionID().c_str() );
		}
		catch(...)
		{
			dp->updateControlState(TA_Base_Bus::comms_failed);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error occur where when change the PIDS");
			return;
		}
		dp->updateControlState(TA_Base_Bus::success);
	}
	
	void DPRequestProcessor::clearWriteQueue()
	{
		if ( NULL != m_writeQ )
        {
            // only proceed if there are output commands waiting for processing
            // for each of the requests in the queue
            while ( m_writeQ->getSize() > 0 )
            {
                // remove it from the queue
                TA_Base_Bus::DataPointWriteRequest * writeReq = m_writeQ->blockForItem();
                
                if ( NULL != writeReq )
                {
                    TA_Base_Bus::DataPoint * point = writeReq->getDataPoint();
                    
                    // update the data point control status to failure
                    point->updateControlState( TA_Base_Bus::comms_failed ); 
                    
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Unable to establish RTU comms...Have removed output command from data point %s",
                        point->getDataPointName().c_str());
                }
                
                delete writeReq;
                writeReq = NULL;
            }
        }
	}
}