/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/DataPointWriteQueue.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  *
  * The transport mechanism for write requests to be passed
  * from the DataPoint to the Agent
  *
  *
  */

#ifndef DATAPOINTWRITEQUEUE_H
#define DATAPOINTWRITEQUEUE_H

#include "bus/scada/common_library/src/BlockingSingletonQueue.h"
#include "bus/scada/common_library/src/BlockingSingletonMap.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

namespace TA_Base_Bus
{
	typedef BlockingSingletonQueue<DataPointWriteRequest> DataPointWriteQueue;
	typedef BlockingSingletonMap<unsigned long, DataPointWriteRequest> DataPointWriteMap;
}

#endif
