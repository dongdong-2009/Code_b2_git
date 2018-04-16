/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/io_manager/src/ProtocolDevice.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "ProtocolDevice.h"

namespace TA_Base_Bus
{
    ProtocolDevice::ProtocolDevice(TA_Base_Core::IEntityData* pData)
    {
		if (pData)
		{
			m_entityKey			= pData->getKey();
			m_entityName		= pData->getName();
			m_entityAddress		= pData->getAddress();
			m_entityType		= pData->getType();
			m_entityDescription = pData->getDescription();
			m_entityParentName	= pData->getParentName();
			m_entityAgentName	= pData->getAgentName();
			m_entityLocationName = pData->getLocationName();
			m_entitySubsystemName = pData->getSubsystemName();
		}
    }

	ProtocolDevice::~ProtocolDevice()
	{
	}

}
