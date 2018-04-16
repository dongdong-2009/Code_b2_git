/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
