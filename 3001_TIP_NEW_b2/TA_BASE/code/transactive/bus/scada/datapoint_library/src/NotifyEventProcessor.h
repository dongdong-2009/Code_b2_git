/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/NotifyEventProcessor.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
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
