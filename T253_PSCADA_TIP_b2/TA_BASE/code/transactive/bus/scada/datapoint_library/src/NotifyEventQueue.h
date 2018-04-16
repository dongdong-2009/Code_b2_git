/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/datapoint_library/src/NotifyEventQueue.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2017/10/31 16:21:54 $
  * Last modified by : $Author: CM $
  *
  *
  *	The transport mechanism used by DataPoints to pass
  * event requests to the NotifyEventProcessor
  *
  *
  */

#ifndef NOTIFYEVENTQUEUE_H
#define NOTIFYEVENTQUEUE_H

#include "bus/scada/common_library/src/BlockingSingletonQueue.h"
#include "bus/scada/datapoint_library/src/NotifyEventRequest.h"


namespace TA_Base_Bus
{
	typedef BlockingSingletonQueue<NotifyEventRequest> NotifyEventQueue;
}

#endif
