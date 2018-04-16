/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\PlannedTelegramStart.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/10 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  */

#include "bus/signalling/SignallingProtocolLibrary/src/PlannedTelegramStart.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	const ta_uint8 PlannedTelegramStart::SUB_ID_POSITION = 4;

	PlannedTelegramStart::PlannedTelegramStart( const Telegram& incomingData )
		: CommonTelegram(incomingData)
	{
		FUNCTION_ENTRY("PlannedTelegramStart");
		FUNCTION_EXIT;
	}

	PlannedTelegramStart::~PlannedTelegramStart()
	{
		FUNCTION_ENTRY("~PlannedTelegramStart");
		FUNCTION_EXIT;
	}

	ta_uint16 PlannedTelegramStart::getSubID() const
	{
		FUNCTION_ENTRY("getSubID");
		FUNCTION_EXIT;
		return getWord(SUB_ID_POSITION);
	}
}