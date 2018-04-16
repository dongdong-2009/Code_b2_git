/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
