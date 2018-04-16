/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  Huang Qi
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  */
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/LiveBroadcastStop.h"

namespace TA_IRS_App
{

const int MODBUS_STOP_MASTER_LIVE_START_ADDRESS = 1;
const int MODBUS_STOP_MASTER_LIVE_END_ADDRESS = 63;

const int MODBUS_STOP_STATION_LIVE_START_ADDRESS = 1;
const int MODBUS_STOP_STATION_LIVE_END_ADDRESS = 63;

LiveBroadcastStop::LiveBroadcastStop( const std::string& strPacketId ) : 
ModbusPacket( strPacketId ),
m_bIsMasterBroadcast( CachedConfig::getInstance()->getIsMaster() )
{
    if ( m_bIsMasterBroadcast )
    {
        m_dbwModbusPacket.setRange( MODBUS_STOP_MASTER_LIVE_START_ADDRESS, MODBUS_STOP_MASTER_LIVE_END_ADDRESS );
    }
    else
    {
        m_dbwModbusPacket.setRange( MODBUS_STOP_STATION_LIVE_START_ADDRESS, MODBUS_STOP_STATION_LIVE_END_ADDRESS );
    }

    setBroadcastSource( LIVE_BROADCAST_STOP_SOURCE );

    //setIsStop( true );
}

LiveBroadcastStop::~LiveBroadcastStop()
{
}

}
