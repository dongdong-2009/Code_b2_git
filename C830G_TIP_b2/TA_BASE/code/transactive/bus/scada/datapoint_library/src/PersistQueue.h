/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/PersistQueue.h $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  *	The transport mechanism used by DataPoints to pass
  * persist requests to the PersistProcessor
  *
  *
  */

#ifndef PERSISTQUEUE_H
#define PERSISTQUEUE_H

#include "bus/scada/common_library/src/BlockingSingletonQueue.h"
#include "bus/scada/datapoint_library/src/PersistRequest.h"

namespace TA_Base_Bus
{
    typedef BlockingSingletonQueue<TA_Base_Bus::PersistRequest> PersistQueue;
}

#endif
