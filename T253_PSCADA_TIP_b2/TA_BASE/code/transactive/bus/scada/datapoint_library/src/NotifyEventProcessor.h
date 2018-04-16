/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/datapoint_library/src/NotifyEventProcessor.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2017/10/31 16:21:54 $
  * Last modified by : $Author: CM $
  *
  *
  * The NotifyEventProcessor thread is responsible
  * for generating events and submitting
  * them.
  *
  */

#ifndef NOTIFYEVENTPROCESSOR_H
#define NOTIFYEVENTPROCESSOR_H

#include <string>

#include "core/threads/src/Thread.h"
#include "bus/scada/common_library/src/EventProcessor.h"
#include "bus/scada/datapoint_library/src/NotifyEventRequest.h"

namespace TA_Base_Bus
{

    class NotifyEventProcessor : public EventProcessor< NotifyEventRequest >
	{

	public:

		NotifyEventProcessor();
		~NotifyEventProcessor();

	private:

		/**
		 * NotifyEventProcessor::processEvent()
		 *
		 * Generates an Event based on the request, and logs it
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
		virtual void processEvent( NotifyEventRequest * newEvent );

	};

}

#endif
