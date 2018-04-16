/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DpControlState.cpp $
  * @author  
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2016/11/01 16:40:39 $
  * Last modified by : $Author: wang.wang $
  *
  * Class representing the control status of the value of a DataPoint
  * For blinking using
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <sstream>

#include "app/scada/OpcBridgeLib/attributes/DpControlState.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "app/scada/opcbridgelib/src/DataPointAttributes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DpControlState::DpControlState(DataPointAttributes* dpAttribute, const std::string& tagName)
	:
	Attribute( tagName, (bool)false, TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED ),
	m_dpAttribute( dpAttribute )
	{

	}

	DpControlState::~DpControlState()
	{
        
	}

	void DpControlState::processProxyUpdate()
	{
		update( m_dpAttribute->isInControlState( ), m_dpAttribute->getStatus( ) );
	}
}
