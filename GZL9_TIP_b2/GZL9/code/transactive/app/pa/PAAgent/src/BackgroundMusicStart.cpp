/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  LiXiaoxia
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  */

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BackgroundMusicStart.h"

namespace TA_IRS_App
{

const int MODBUS_START_MASTER_BACKGROUND_MUSIC_START_ADDRESS = 1;
const int MODBUS_START_MASTER_BACKGROUND_MUSIC_END_ADDRESS = 92;

const int MODBUS_START_STATION_BACKGROUND_MUSIC_START_ADDRESS = 1;
const int MODBUS_START_STATION_BACKGROUND_MUSIC_END_ADDRESS = 92;

BackgroundMusicStart::BackgroundMusicStart( const std::string& strPacketId ) : 
ModbusPacket( strPacketId ),
m_bIsMasterBroadcast( CachedConfig::getInstance()->getIsMaster() )
{
    if ( m_bIsMasterBroadcast )
    {
        m_dbwModbusPacket.setRange( MODBUS_START_MASTER_BACKGROUND_MUSIC_START_ADDRESS, MODBUS_START_MASTER_BACKGROUND_MUSIC_END_ADDRESS );
    }
    else
    {
        m_dbwModbusPacket.setRange( MODBUS_START_STATION_BACKGROUND_MUSIC_START_ADDRESS, MODBUS_START_STATION_BACKGROUND_MUSIC_END_ADDRESS );
    }

    setBroadcastSource( BACKGROUND_MUSIC_START_SOURCE );

    //setIsStop( false );
}

BackgroundMusicStart::~BackgroundMusicStart()
{
}

}
