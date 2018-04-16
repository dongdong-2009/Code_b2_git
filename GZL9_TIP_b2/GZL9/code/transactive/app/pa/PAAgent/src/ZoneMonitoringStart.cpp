/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/FZL1_TIP/FZL1/transactive/app/pa/PAAgent/src/ZoneMonitoringStart.cpp $
  * @author  LiXiaoxia
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:59:10 $
  * Last modified by : $Author: CM $
  *
  *
  */

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/ZoneMonitoringStart.h"

namespace TA_IRS_App
{

	const int MODBUS_START_MASTER_MONITORING_START_ADDRESS = 1;
	const int MODBUS_START_MASTER_MONITORING_END_ADDRESS = 92;

	const int MODBUS_START_STATION_MONITORING_START_ADDRESS = 1;
	const int MODBUS_START_STATION_MONITORING_END_ADDRESS = 92;

	ZoneMonitoringStart::ZoneMonitoringStart( const std::string& strPacketId ) : 
	ModbusPacket( strPacketId),
		m_bIsMasterBroadcast( CachedConfig::getInstance()->getIsMaster() )
	{
		if ( m_bIsMasterBroadcast )
		{
			m_dbwModbusPacket.setRange( MODBUS_START_MASTER_MONITORING_START_ADDRESS, MODBUS_START_MASTER_MONITORING_END_ADDRESS );
		}
		else
		{
			m_dbwModbusPacket.setRange( MODBUS_START_STATION_MONITORING_START_ADDRESS, MODBUS_START_STATION_MONITORING_END_ADDRESS );
		}

		setBroadcastSource( ZONE_MONITORING_START_SOURCE );

	}

	ZoneMonitoringStart::~ZoneMonitoringStart()
	{
	}
}
