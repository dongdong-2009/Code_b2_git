/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
