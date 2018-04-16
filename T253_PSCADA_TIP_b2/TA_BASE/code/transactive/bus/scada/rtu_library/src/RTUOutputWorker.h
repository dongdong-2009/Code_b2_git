/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source: $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/rtu_library/src/RTUOutputWorker.h $
  * @author  Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2017/10/31 16:21:54 $
  * Last modified by:	$Author: CM $
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

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
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

namespace TA_Base_Bus
{
    // Forward declaration
	class RTU;

    class RTUOutputWorker : public EventProcessor< TA_Base_Bus::DataPointWriteRequest >
	{

	public:
		
		virtual ~RTUOutputWorker();

        void addRTU(RTU& rtu);
		
		void clearWriteQueue();

        void setToControl();
        void setToMonitor();

		static RTUOutputWorker* getInstance();
		static void             removeInstance();

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
		RTUOutputWorker();
        // bool m_inService;
		volatile bool m_isInControlMode;

		std::vector<RTU *> m_rtus;
		TA_Base_Bus::DataPointWriteQueue * m_writeQ;

		static RTUOutputWorker *m_singleton;
		static unsigned int     m_refCounter;
		static TA_Base_Core::ReEntrantThreadLockable    m_singletonLock;
	};

}

#endif  // RTUOUTPUTWORKER_H
