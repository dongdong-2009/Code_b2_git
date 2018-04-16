/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  Raymond Pau
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  * The WfOutputWorker thread is responsible
  * for sending DataPointWriteRequest to the actual hardware
  *
  */
#include <sstream>
#include "WFCommandPolicy.h"
#include "app/scada/WFAgent/src/WFUnit.h"
#include "app/scada/WFAgent/src/WFOutputWorker.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

#include "core/sockets/src/SocketOptions.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	WfOutputWorker::WfOutputWorker()
    :
    m_inService( false ),
	m_writeQ( NULL )
    {
        FUNCTION_ENTRY("Constructor");

		m_wfUnits.clear();

		// get instance to the write queue
		m_writeQ = TA_Base_Bus::DataPointWriteQueue::getInstance();
		
        FUNCTION_EXIT("Constructor");
	}


	WfOutputWorker::~WfOutputWorker()
	{
        FUNCTION_ENTRY("Destructor");

		// remove instance of the write queue
		if ( NULL != m_writeQ )
		{
			TA_Base_Bus::DataPointWriteQueue::removeInstance( m_writeQ );
		}
		
		m_wfUnits.clear();

        FUNCTION_EXIT("Destructor");
	}
    

    void WfOutputWorker::addWfUnit(WfUnit& wfUnit)
	{
		m_wfUnits.push_back(&wfUnit);
	}

	void WfOutputWorker::processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest )
	{
        FUNCTION_ENTRY("processEvent");

        bool requestProcessed = false;

		for ( std::vector<WfUnit *>::iterator itr = m_wfUnits.begin(); 
              itr != m_wfUnits.end() && requestProcessed == false; 
              ++itr )
		{
			requestProcessed = (* itr)->processWriteRequest( *newDataPointWriteRequest);
		}

        if ( false == requestProcessed )
        {
            // update control status of the data point to indicate that
            // the output command has failed
            
            TA_Base_Bus::DataPoint * dp = newDataPointWriteRequest->getDataPoint();            
            TA_ASSERT( 0 != dp, "WfOutputWorker::processEvent: newDataPointWriteRequest contains a NULL datapoint." );

            WfCommandPolicy::updateControlState(dp, TA_Base_Bus::comms_failed,"", false);

            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "No output worker found to process write request for datapoint %s value: %s",
                newDataPointWriteRequest->getDataPoint()->getDataPointName().c_str(),
                newDataPointWriteRequest->getValue().getValueAsString().c_str() );
        }

        FUNCTION_EXIT("processEvent");
    }

	void WfOutputWorker::clearWriteQueue()
	{
		FUNCTION_ENTRY("clearWriteQueue");
       // if the queue is initialised
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
                    WfCommandPolicy::updateControlState(point, TA_Base_Bus::comms_failed,"", false); //wenching++ (TD13999)
                    
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                        "Unable to establish RTU comms...Have removed output command from data point %s",
                        point->getDataPointName().c_str());
                }
                
                delete writeReq;
                writeReq = NULL;
            }
        }
		FUNCTION_EXIT("clearWriteQueue");
    }
}
