/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/threads/src/WorkItemQueueProcessor.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is a queue processor which processes work items.
  * It provides a means of having a queue which you can put anything in
  * and the items you insert will do their own work sequentially
  *
  */

#include "core/threads/src/WorkItemQueueProcessor.h"

namespace TA_Base_Core
{
    WorkItemQueueProcessor::WorkItemQueueProcessor (unsigned long maxQueueSize)
      : QueueProcessor<IWorkItem>(maxQueueSize)
    {}

    WorkItemQueueProcessor::~WorkItemQueueProcessor()
    {
    }

	void WorkItemQueueProcessor::processEvent( boost::shared_ptr<IWorkItem> newEvent )
    {
        newEvent->executeWorkItem();
    }
};

