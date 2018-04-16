/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/scada/PMSAgent/src/RTUOutputWorker.h $
  * @author  Raymond Pau
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  * The RTUOutputWorker thread is responsible
  * for sending DataPointWriteRequest to the correct StationSystemOutputWriter
  *
  */

#ifndef RTUOUTPUTWORKER_H
#define RTUOUTPUTWORKER_H

#include <vector>

#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

namespace TA_Base_Core
{
    class SocketOptions;
}

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{
    // Forward declaration
	class RTU;

    class RTUOutputWorker : public EventProcessor< TA_Base_Bus::DataPointWriteRequest >
	{

	public:
		RTUOutputWorker();
		virtual ~RTUOutputWorker();

        void addRTU(RTU& rtu);
		
		void clearWriteQueue();

	private:

		/**
		 * processEvent()
		 *
		 * Generates an Event based on the request, and logs it
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
        virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest );

    private:
        bool m_inService;
		std::vector<RTU *> m_rtus;
		TA_Base_Bus::DataPointWriteQueue * m_writeQ;
	};

}

#endif  // RTUOUTPUTWORKER_H
