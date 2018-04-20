// DPRequestProcessor.h: interface for the DPRequestProcessor class.
// process the datapoint request from the scada
// this file is add for fixing the TD16505
//////////////////////////////////////////////////////////////////////
#ifndef TIS_AGENT_DPREQUESTPROCESSOR_H_INCLUDED
#define TIS_AGENT_DPREQUESTPROCESSOR_H_INCLUDED


#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

namespace TA_IRS_App
{
	class DPRequestProcessor : public EventProcessor< TA_Base_Bus::DataPointWriteRequest >
	{
	public:
		DPRequestProcessor();
		virtual ~DPRequestProcessor();
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

        bool m_inService;
		TA_Base_Bus::DataPointWriteQueue * m_writeQ;
	};
}
#endif // !defined(TIS_AGENT_DPREQUESTPROCESSOR_H_INCLUDED)
