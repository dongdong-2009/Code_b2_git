// HeartBeatWork.h: interface for the HeartBeatWork class.
//
//////////////////////////////////////////////////////////////////////
#ifndef HEARTBEATWORK_H
#define HEARTBEATWORK_H

#include <string>
#include <vector>
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "core/threads/src/Thread.h"

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
	class HeartBeatWorker : public TA_Base_Core::Thread
	{

	public:

		HeartBeatWorker();

		virtual ~HeartBeatWorker ();

		virtual void run();

		virtual void terminate();

		void addRTU(RTU& rtu);

        void setToControl();//limin, failover issue

        void setToMonitor();//limin, failover issue

	protected:

		virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newHeartBeat );

		std::vector<RTU *> m_rtus;

		bool m_keepRunning;

        unsigned long m_operationMode; //limin, failover issue
	};
}
#endif