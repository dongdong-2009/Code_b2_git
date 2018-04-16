/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/datapoint_library/src/PersistQueue.h $
  * @author  Raymond Pau
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2017/10/31 16:21:54 $
  * Last modified by : $Author: CM $
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
