/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\app\ats\AtsAgent\SignallingProcessors\src\HelperLogs.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/10/31 02:37:50 $
  * Last modified by:  $Author: builder $
  *
  * This is a helper class which used to print the logs.
  */

#include "bus/signalling/SignallingProtocolLibrary/src/HelperLogs.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>
#include <iomanip>

namespace TA_IRS_Bus
{
	void HelperLogs::printRawData(const std::vector<unsigned char>& rawdata, const std::string& sourceMsg)
	{
		if (TA_Base_Core::DebugUtil::DebugDebug <= TA_Base_Core::DebugUtil::getInstance().getLevel())
		{
			std::ostringstream packetData;
			packetData << "Source:" << sourceMsg <<", Size=" << rawdata.size() <<", Hex Raw Data= ";

			std::vector<unsigned char>::const_iterator iter = rawdata.begin();
			for(iter; iter != rawdata.end(); iter++)
			{
				packetData << std::setw(2) << std::setfill('0')<< std::hex << std::uppercase << static_cast<int>(*iter) <<" ";
			}

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,packetData.str().c_str() );
		}	
	}
}