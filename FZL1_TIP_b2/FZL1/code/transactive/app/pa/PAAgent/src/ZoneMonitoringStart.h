/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/FZL1_TIP/FZL1/transactive/app/pa/PAAgent/src/ZoneMonitoringStart.h $
  * @author  LiXiaoxia
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:59:10 $
  * Last modified by : $Author: CM $
  *
  *
  */
#if !defined(ZONEMONITORINGSTART__INCLUDED_)
#define ZONEMONITORINGSTART__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

	class ZoneMonitoringStart : public ModbusPacket
	{
	public:
		ZoneMonitoringStart( const std::string& strPacketId );
		virtual ~ZoneMonitoringStart();

	private:
		bool m_bIsMasterBroadcast;
	};

}

#endif 