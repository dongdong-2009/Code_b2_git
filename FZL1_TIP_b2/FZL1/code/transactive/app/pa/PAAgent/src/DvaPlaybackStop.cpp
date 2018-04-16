// DVABroadcast.cpp: implementation of the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/DvaPlaybackStop.h"

namespace TA_IRS_App
{

const int MODBUS_STOP_MASTER_DVA_PLAYBACK_START_ADDRESS = 1;
const int MODBUS_STOP_MASTER_DVA_PLAYBACK_END_ADDRESS = 63;

const int MODBUS_STOP_STATION_DVA_PLAYBACK_START_ADDRESS = 1;
const int MODBUS_STOP_STATION_DVA_PLAYBACK_END_ADDRESS = 63;

DvaPlaybackStop::DvaPlaybackStop( const std::string& strPacketId ) : 
ModbusPacket( strPacketId ),
m_bIsMasterBroadcast( CachedConfig::getInstance()->getIsMaster() )
{
    if ( m_bIsMasterBroadcast )
    {
        m_dbwModbusPacket.setRange( MODBUS_STOP_MASTER_DVA_PLAYBACK_START_ADDRESS, MODBUS_STOP_MASTER_DVA_PLAYBACK_END_ADDRESS );
    }
    else
    {
        m_dbwModbusPacket.setRange( MODBUS_STOP_STATION_DVA_PLAYBACK_START_ADDRESS, MODBUS_STOP_STATION_DVA_PLAYBACK_END_ADDRESS );
    }

    //setBroadcastSource( DVA_PLAYBACK_SOURCE );

    //setIsStop( true );
}

DvaPlaybackStop::~DvaPlaybackStop()
{
}

}
