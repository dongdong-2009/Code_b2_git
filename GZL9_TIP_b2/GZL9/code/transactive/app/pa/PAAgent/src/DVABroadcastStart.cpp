// DVABroadcast.cpp: implementation of the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/DVABroadcastStart.h"

namespace TA_IRS_App
{

const int MODBUS_START_MASTER_DVA_START_ADDRESS = 1;
const int MODBUS_START_MASTER_DVA_END_ADDRESS = 92;

const int MODBUS_START_STATION_DVA_START_ADDRESS = 1;
const int MODBUS_START_STATION_DVA_END_ADDRESS = 92;

DVABroadcastStart::DVABroadcastStart( const std::string& strPacketId ) : 
ModbusPacket( strPacketId),
m_bIsMasterBroadcast( CachedConfig::getInstance()->getIsMaster() )
{
    if ( m_bIsMasterBroadcast )
    {
        m_dbwModbusPacket.setRange( MODBUS_START_MASTER_DVA_START_ADDRESS, MODBUS_START_MASTER_DVA_END_ADDRESS );
    }
    else
    {
        m_dbwModbusPacket.setRange( MODBUS_START_STATION_DVA_START_ADDRESS, MODBUS_START_STATION_DVA_END_ADDRESS );
    }

    setBroadcastSource( DVA_BROADCAST_START_SOURCE );

    //setIsStop( false );
}

DVABroadcastStart::~DVABroadcastStart()
{
}

void DVABroadcastStart::updateCylicTimes( unsigned long ulTimes )
{
    setCyclicTimes( ulTimes );
}

void DVABroadcastStart::updateDwellTime( unsigned long ulDwellTime )
{
    setDwellTime( ulDwellTime );
}
}

