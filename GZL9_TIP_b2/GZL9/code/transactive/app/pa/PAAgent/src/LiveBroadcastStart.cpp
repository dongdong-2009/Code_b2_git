// DVABroadcast.cpp: implementation of the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/LiveBroadcastStart.h"

namespace TA_IRS_App
{

const int MODBUS_START_MASTER_LIVE_START_ADDRESS = 1;
const int MODBUS_START_MASTER_LIVE_END_ADDRESS = 92;

const int MODBUS_START_STATION_LIVE_START_ADDRESS = 1;
const int MODBUS_START_STATION_LIVE_END_ADDRESS = 92;

LiveBroadcastStart::LiveBroadcastStart( const std::string& strPacketId ) : 
ModbusPacket( strPacketId ),
m_bIsMasterBroadcast( CachedConfig::getInstance()->getIsMaster() )
{
    if ( m_bIsMasterBroadcast )
    {
        m_dbwModbusPacket.setRange( MODBUS_START_MASTER_LIVE_START_ADDRESS, MODBUS_START_MASTER_LIVE_END_ADDRESS );
    }
    else
    {
        m_dbwModbusPacket.setRange( MODBUS_START_STATION_LIVE_START_ADDRESS, MODBUS_START_STATION_LIVE_END_ADDRESS );
    }

    setBroadcastSource( LIVE_BROADCAST_START_SOURCE );

    //setIsStop( false );
}

LiveBroadcastStart::~LiveBroadcastStart()
{
}

}
