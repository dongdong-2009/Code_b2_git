/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef _IENTITYUPDATEEVENTPROCESSOR_H
#define _IENTITYUPDATEEVENTPROCESSOR_H

#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
	class IEntityUpdateEventProcessor
	{
		public:
			virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType) = 0;
	};
}

#endif
