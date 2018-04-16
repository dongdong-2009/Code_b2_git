/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/FZL1_TIP/FZL1/transactive/app/pa/PAAgent/src/ZoneMonitoringStop.h $
  * @author  LiXiaoxia
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:59:10 $
  * Last modified by : $Author: CM $
  *
  *
  */

#if !defined(ZONEMONITORINGSTOP__INCLUDED_)
#define ZONEMONITORINGSTOP__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{


class ZoneMonitoringStop : public ModbusPacket
{
public:
	ZoneMonitoringStop(const std::string& strPacketId);
	virtual ~ZoneMonitoringStop();
private:
	bool m_bIsMasterBroadcast;
};
}
#endif
