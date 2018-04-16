/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/message/src/EventChannelUser.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/EventChannelUser.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	IChannelObserver::IChannelObserver( const std::string& channelName, unsigned long locationKey )
		: m_channelName( channelName ), m_locationKey( locationKey )
	{
		TA_ASSERT( !channelName.empty() , "The channel was empty");

        // Clients sometimes use locationKey == 0 to indicate an uninitialised value.
        TA_Assert(m_locationKey != 0);
	}


	IChannelObserver::~IChannelObserver()
	{
	}

}; // TA_Base_Core

