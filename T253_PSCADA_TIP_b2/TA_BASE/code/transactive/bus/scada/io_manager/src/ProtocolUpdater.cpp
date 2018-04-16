/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/io_manager/src/ProtocolUpdater.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "ProtocolUpdater.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    ProtocolUpdater::ProtocolUpdater(std::string name)
		:
		ProtocolIOBase(name)
    {
        
    }

	ProtocolUpdater::~ProtocolUpdater()
	{
	}

	void ProtocolUpdater::updateDPFieldValue(DataPoint* dp, DataPointState& dpState)
	{
		if(dp)
		{
			this->m_dpProcessor->submitData(dp, dpState);
		}
	}
}
