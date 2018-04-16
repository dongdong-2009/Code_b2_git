
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"

#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

const unsigned long GLOBAL_STATION_ID_FIRST_WORD = 1u;
const unsigned long GLOBAL_STATION_ID_SECOND_WORD = 2u;

const unsigned long GLOBAL_ZONE_ID_BEGIN_WORD = 1u;
const unsigned long GLOBAL_ZONE_ID_END_WORD = 60u;
const unsigned long GLOBAL_ZONE_ID_BITS_COUNT = 8u;
const unsigned long GLOBAL_ZONE_ID_WORD_COUNT = 2u;
const unsigned long GLOBAL_ZONE_ID_COUNT_FOR_ONE_WORD = 16;

const unsigned long GLOBAL_MESSAGE_ID_WORD = 61u;
const unsigned long GLOBAL_MESSAGE_ID_MIN = 1u;
const unsigned long GLOBAL_MESSAGE_ID_MAX = 700u;

const unsigned long GLOBAL_CYCLIC_TIMES_WORD = 63u;
const unsigned long GLOBAL_CYCLIC_TIMES_BIT_IN_WORD = 0u;
const unsigned long GLOBAL_CYCLIC_TIMES_BITS_COUNT = 8u;
const unsigned long GLOBAL_CYCLIC_TIMES_MAX = 255u;

const unsigned long GLOBAL_CYCLIC_FLAG_WORD = 63u;
const unsigned long GLOBAL_CYCLIC_FLAG_BIT_IN_WORD = 8u;
const unsigned long GLOBAL_CYCLIC_FLAG_BITS_COUNT = 1u;

const unsigned long GLOBAL_STOP_FLAG_WORD = 62u;
const unsigned long GLOBAL_STOP_FLAG_BIT_IN_WORD = 7u;
const unsigned long GLOBAL_STOP_FLAG_BITS_COUNT = 1u;

const unsigned long GLOBAL_SOURCE_ID_WORD = 62u;
const unsigned long GLOBAL_SOURCE_ID_BIT_IN_WORD = 4u;
const unsigned long GLOBAL_SOURCE_ID_BITS_COUNT = 4u;

const unsigned long GLOBAL_PRIORITY_ID_WORD = 62u;
const unsigned long GLOBAL_PRIORITY_ID_BIT_IN_WORD = 0u;
const unsigned long GLOBAL_PRIORITY_ID_BITS_COUNT = 4u;
const unsigned long GLOBAL_PRIORITY_ID_MIN = 1u;
const unsigned long GLOBAL_PRIORITY_ID_MAX = 10u;

const unsigned long GLOBAL_XH_TRIGGER_FLAG_WORD = 64u;
const unsigned long GLOBAL_FIRE_FLAG_BIT_IN_WORD = 10u;
const unsigned long GLOBAL_FIRE_FLAG_BITS_COUNT = 1u;
const unsigned long GLOBAL_SIG_FLAG_BITS_COUNT = 1u;
const unsigned long GLOBAL_SIG_FLAG_UP_LEAVING_BIT_IN_WORD = 0u;
const unsigned long GLOBAL_SIG_FLAG_UP_ARRIVED_BIT_IN_WORD = 1u;
const unsigned long GLOBAL_SIG_FLAG_UP_PRE_ARRIVING_BIT_IN_WORD = 2u;
const unsigned long GLOBAL_SIG_FLAG_UP_JUMP_STATION_BIT_IN_WORD = 3u;
const unsigned long GLOBAL_SIG_FLAG_UP_BACK_WAREHOUSE_BIT_IN_WORD = 4u;
const unsigned long GLOBAL_SIG_EVENT_TYPE_BITS_COUNT_IN_WORD = 5u;

const int MODBUS_XH_START_ADDRESS = 64;
const int MODBUS_XH_END_ADDRESS = 64;

const char* const PARAM_PRIORITY_ID = "(PriorityId)";
const char* const PARAM_MESSAGE_ID = "(DvaMessageId)";
const char* const PARAM_CYCLIC_TIMES = "(CyclicTimes)";
const char* const PARAM_STATION_ID = "(StationId)";
const char* const PARAM_ZONE_ID = "(ZoneId)";
const char* const PARAM_SIG_EVENT_TYPE = "(SigEventType)";
const char* const PARAM_SIG_PLATFORM_DIRECTION = "(SigPlatformDirection)";

ModbusPacket::ModbusPacket( const std::string& strPacketId, int nStart, int nEnd ) : 
m_strPacketId( strPacketId ),
m_dbwModbusPacket( nStart, nEnd ),
m_nRetrySendingTimes( 0 ),
m_bInvalidParameter( false ),
m_strInvalidParameters( "" )
{
}

ModbusPacket::ModbusPacket( const std::string& strPacketId ) :
m_strPacketId( strPacketId ),
m_dbwModbusPacket(),
m_nRetrySendingTimes( 0 ),
m_bInvalidParameter( false ),
m_strInvalidParameters( "" )
{
}

ModbusPacket::~ModbusPacket()
{
    m_dbwModbusPacket.setRange( 0, 0 );
}

TA_Base_Bus::DataBlock<WORD>& ModbusPacket::getModbusPacket()
{
    return m_dbwModbusPacket;
}

int ModbusPacket::getSendingRetryTimes()
{
    return m_nRetrySendingTimes;
}

void ModbusPacket::sendingFailed()
{
    ++m_nRetrySendingTimes;
}

// Station Id is from 1 to 40
void ModbusPacket::setStationsId( const std::vector<unsigned long>& vecStationsId )
{
    WORD wdStationsIdFirstWord = 0;
    WORD wdStationsIdSecondWord = 0;

    for ( std::vector<unsigned long>::const_iterator itLoop = vecStationsId.begin(); 
          vecStationsId.end() != itLoop; ++itLoop )
    {
        unsigned long ulBitInWord = ( *itLoop - 1 ) % 16;
        if ( 0 == ( *itLoop / 16 ))
        {
            wdStationsIdFirstWord |= ( 0x01 << ulBitInWord );
        }
        else if ( 1 == ( *itLoop / 16 ))
        {
            wdStationsIdSecondWord |= ( 0x01 << ulBitInWord );
        }
        else
        {
            m_bInvalidParameter = true;
            m_strInvalidParameters += PARAM_STATION_ID;
        }
    }

    m_dbwModbusPacket.set( GLOBAL_STATION_ID_FIRST_WORD, wdStationsIdFirstWord );
    m_dbwModbusPacket.set( GLOBAL_STATION_ID_SECOND_WORD, wdStationsIdSecondWord );
}

// Station is from 1 to 30, zone id is from 1 to 16(two words for one station)
/*
             Station 1                 
bits  15 14 13 12 11 10 09 08    07 06 05 04 03 02 01 00
zone  /  14 13 12 11 10  9  8     /  7  6  5  4  3  2  1
             Station 1                 
bits  15 14 13 12 11 10 09 08    07 06 05 04 03 02 01 00
zone  /  28 27 26 25 24 23 22     / 21 20 19 18 17 16 15
*/
void ModbusPacket::setZonesId( unsigned long ulStationId, const std::vector<unsigned long>& vecZonesId )
{
    // Check the station id
    if (( PA_STATION_ID_MIN > ulStationId ) || ( PA_STATION_ID_MAX < ulStationId ))
    {
        m_bInvalidParameter = true;
        m_strInvalidParameters += PARAM_STATION_ID;
        return;
    }

	// Two words contain one station
	unsigned long ulZoneWordIndex = GLOBAL_ZONE_ID_BEGIN_WORD + ( ulStationId - 1 ) * GLOBAL_ZONE_ID_WORD_COUNT;

	if ( GLOBAL_ZONE_ID_END_WORD < ulZoneWordIndex )
	{
		m_bInvalidParameter = true;
		m_strInvalidParameters += PARAM_STATION_ID;
		return;
	}

    unsigned short wZonesValueForFirstWord = 0u;
	unsigned short wZonesValueForSecondWord = 0u;
    unsigned long ulZoneId = 0u;
    for ( std::vector<unsigned long>::const_iterator itZoneLoop = vecZonesId.begin();
          vecZonesId.end() != itZoneLoop; ++ itZoneLoop )
    {
        ulZoneId = *itZoneLoop;
        // Check the zone id in accepted range
        if (( PA_ZONE_ID_MIN > ulZoneId ) || ( PA_ZONE_ID_MAX < ulZoneId ))
        {
            m_bInvalidParameter = true;
            m_strInvalidParameters += PARAM_ZONE_ID;
            return;
        }

		if(GLOBAL_ZONE_ID_COUNT_FOR_ONE_WORD >= ulZoneId)
		{
            wZonesValueForFirstWord |= ( 0x1 << ( ulZoneId - 1 ));
   		}
		else
		{
            wZonesValueForSecondWord |= ( 0x1 << ( (ulZoneId % GLOBAL_ZONE_ID_COUNT_FOR_ONE_WORD) - 1 ));
		}
    }

	// Update the modbus packet
	m_dbwModbusPacket.set( ulZoneWordIndex, wZonesValueForFirstWord);
	m_dbwModbusPacket.set( ulZoneWordIndex + 1, wZonesValueForSecondWord);
}

void ModbusPacket::setPriorityId( unsigned long ulPriorityId )
{
    if (( GLOBAL_PRIORITY_ID_MIN > ulPriorityId ) || ( GLOBAL_PRIORITY_ID_MAX < ulPriorityId ))
    {
        m_bInvalidParameter = true;
        m_strInvalidParameters += PARAM_PRIORITY_ID;
        return;
    }
     
    WORD wdOperatorIdWord = m_dbwModbusPacket[GLOBAL_PRIORITY_ID_WORD];

    // Update the bits
    setBitsInWord( wdOperatorIdWord, GLOBAL_PRIORITY_ID_BIT_IN_WORD, GLOBAL_PRIORITY_ID_BITS_COUNT, ulPriorityId );

    m_dbwModbusPacket.set( GLOBAL_PRIORITY_ID_WORD, wdOperatorIdWord );
}

void ModbusPacket::setDVAMessageId( unsigned long ulMessageId )
{
    if (( GLOBAL_MESSAGE_ID_MIN > ulMessageId ) || ( GLOBAL_MESSAGE_ID_MAX < ulMessageId ))
    {
        m_bInvalidParameter = true;
        m_strInvalidParameters += PARAM_MESSAGE_ID;
        return;
    }

    WORD wdMessageIdWord = static_cast<WORD>(ulMessageId);

    // Update the message id word
    m_dbwModbusPacket.set( GLOBAL_MESSAGE_ID_WORD, wdMessageIdWord );
}

void ModbusPacket::setCyclicTimes( unsigned long ulCyclicTimes )
{
    if ( GLOBAL_CYCLIC_TIMES_MAX < ulCyclicTimes )
    {
        m_bInvalidParameter = true;
        m_strInvalidParameters += PARAM_CYCLIC_TIMES;
        return;
    }

    WORD wdCyclicTimesWord = m_dbwModbusPacket[GLOBAL_CYCLIC_TIMES_WORD];

    // Update the bits
    setBitsInWord( wdCyclicTimesWord, GLOBAL_CYCLIC_TIMES_BIT_IN_WORD, GLOBAL_CYCLIC_TIMES_BITS_COUNT, ulCyclicTimes );

    m_dbwModbusPacket.set( GLOBAL_CYCLIC_TIMES_WORD, wdCyclicTimesWord );

    WORD wdCyclicFlagWord = m_dbwModbusPacket[GLOBAL_CYCLIC_FLAG_WORD];

    unsigned long ulCyclicFlag = ( 0u < ulCyclicTimes ) ? 1u : 0u;
    // Update the bits
    setBitsInWord( wdCyclicFlagWord, GLOBAL_CYCLIC_FLAG_BIT_IN_WORD, GLOBAL_CYCLIC_FLAG_BITS_COUNT, ulCyclicFlag );
    
    m_dbwModbusPacket.set( GLOBAL_CYCLIC_FLAG_WORD, wdCyclicFlagWord );
}

void ModbusPacket::setBroadcastSource( BROADCAST_TYPE_SOURCE eSourceId )
{
    WORD wdSourceIdWord = m_dbwModbusPacket[GLOBAL_SOURCE_ID_WORD];

    // Update the bits
    setBitsInWord( wdSourceIdWord, GLOBAL_SOURCE_ID_BIT_IN_WORD, GLOBAL_SOURCE_ID_BITS_COUNT, static_cast<unsigned long>(eSourceId) );

    m_dbwModbusPacket.set( GLOBAL_SOURCE_ID_WORD, wdSourceIdWord );
}

void ModbusPacket::setIsStop( bool bStop )
{
    WORD wdStopFlagWord = m_dbwModbusPacket[GLOBAL_STOP_FLAG_WORD];

    unsigned long ulStopFlag = bStop ? 1u : 0u;
    // Update the bits
    setBitsInWord( wdStopFlagWord, GLOBAL_STOP_FLAG_BIT_IN_WORD, GLOBAL_STOP_FLAG_BITS_COUNT, ulStopFlag );

    m_dbwModbusPacket.set( GLOBAL_STOP_FLAG_WORD, wdStopFlagWord );
}

void ModbusPacket::setFasTriggedBit( bool bFire )
{
	m_dbwModbusPacket.setRange( MODBUS_XH_START_ADDRESS, MODBUS_XH_END_ADDRESS );
    WORD wdXHFlagWord = m_dbwModbusPacket[GLOBAL_XH_TRIGGER_FLAG_WORD];
	
	unsigned long ulFireFlag = bFire ? 1u : 0u;
	// Update the bit
	setBitsInWord(wdXHFlagWord, GLOBAL_FIRE_FLAG_BIT_IN_WORD, GLOBAL_FIRE_FLAG_BITS_COUNT, ulFireFlag );

	m_dbwModbusPacket.set( GLOBAL_XH_TRIGGER_FLAG_WORD, wdXHFlagWord );
}

void ModbusPacket::setSigTriggedBit(unsigned long ulEventType, unsigned long ulPlatformType)
{
	m_dbwModbusPacket.setRange( MODBUS_XH_START_ADDRESS, MODBUS_XH_END_ADDRESS );
	WORD wdXHFlagWord = m_dbwModbusPacket[GLOBAL_XH_TRIGGER_FLAG_WORD];

	unsigned long ulSigFlagBit = 0u;
    unsigned long ulSigFlag = 1u;

	if( TA_IRS_Bus::up == ulPlatformType )
	{
		switch ( ulEventType )
		{
		case TA_IRS_Bus::IAtsPasCorbaDef::Departure:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_LEAVING_BIT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Arrival:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_ARRIVED_BIT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::PreArrival:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_PRE_ARRIVING_BIT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Skip:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_JUMP_STATION_BIT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::InDepot:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_BACK_WAREHOUSE_BIT_IN_WORD;
			break;
		default:
			m_bInvalidParameter = true;
			m_strInvalidParameters += PARAM_SIG_EVENT_TYPE;
            return;
		}
	}
	else if( TA_IRS_Bus::down == ulPlatformType )
	{
		switch ( ulEventType )
		{
		case TA_IRS_Bus::IAtsPasCorbaDef::Departure:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_LEAVING_BIT_IN_WORD + GLOBAL_SIG_EVENT_TYPE_BITS_COUNT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Arrival:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_ARRIVED_BIT_IN_WORD + GLOBAL_SIG_EVENT_TYPE_BITS_COUNT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::PreArrival:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_PRE_ARRIVING_BIT_IN_WORD + GLOBAL_SIG_EVENT_TYPE_BITS_COUNT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Skip:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_JUMP_STATION_BIT_IN_WORD + GLOBAL_SIG_EVENT_TYPE_BITS_COUNT_IN_WORD;
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::InDepot:
			ulSigFlagBit = GLOBAL_SIG_FLAG_UP_BACK_WAREHOUSE_BIT_IN_WORD + GLOBAL_SIG_EVENT_TYPE_BITS_COUNT_IN_WORD;
			break;
		default:
			m_bInvalidParameter = true;
			m_strInvalidParameters += PARAM_SIG_EVENT_TYPE;
			return;
		}
	}
	else
	{
		m_bInvalidParameter = true;
		m_strInvalidParameters += PARAM_SIG_PLATFORM_DIRECTION;
		return;
	}

	// Update the bit
	setBitsInWord(wdXHFlagWord, ulSigFlagBit, GLOBAL_SIG_FLAG_BITS_COUNT, ulSigFlag);

	m_dbwModbusPacket.set( GLOBAL_XH_TRIGGER_FLAG_WORD, wdXHFlagWord );
}

void ModbusPacket::cleanBitsInWord( WORD& wdDes, unsigned long ulBitStart, unsigned long ulBitsCount )
{
    if (( 0u > ulBitStart ) || ( 15u < ulBitStart ))
    {
        return;
    }

    unsigned long ulBitEnd = ulBitStart + ulBitsCount - 1u;
    if (( 0u > ulBitEnd ) || ( 15u < ulBitEnd ))
    {
        return;
    }

    WORD wdMask = 0;
    // Set the low bits to 1 according to the bits count and then left move to start bit
    wdMask |= ((( 0x01u << ulBitsCount ) - 1u ) << ulBitStart );

    wdMask = (~wdMask);
    wdDes &= wdMask;
}

void ModbusPacket::setBitsInWord( WORD& wdDes, unsigned long ulBitStart, unsigned long ulBitsCount, unsigned long ulValue )
{
    cleanBitsInWord( wdDes, ulBitStart, ulBitsCount );

    if (( 0u > ulBitStart ) || ( 15u < ulBitStart ))
    {
        m_bInvalidParameter = true;
        return;
    }
    
    unsigned long ulBitEnd = ulBitStart + ulBitsCount - 1u;
    if (( 0u > ulBitEnd ) || ( 15u < ulBitEnd ))
    {
        m_bInvalidParameter = true;
        return;
    }

    unsigned long ulMax = (( 0x01u << ulBitsCount ) - 1u );

    if ( ulValue > ulMax )
    {
        m_bInvalidParameter = true;
        return;
    }
    WORD wdValue = static_cast<WORD>(ulValue) << ulBitStart;

    wdDes |= wdValue;
}

void ModbusPacket::updatePriorityBySessionId( const std::string& strSessionId )
{
    unsigned long ulPriority = PasHelpers::getInstance()->getPaPriorityIdBySessionId( strSessionId );
    setPriorityId( ulPriority );
}

void ModbusPacket::updatePriorityByEventType( TRIGGER_EVENT_TYPE eEventType )
{
    unsigned long ulPriority = static_cast<unsigned long>(eEventType);
    setPriorityId( ulPriority );
}

void ModbusPacket::updatePriorityId( unsigned long ulPriorityGroup )
{
    setPriorityId( ulPriorityGroup );
}

void ModbusPacket::updateDVAMsgIdByDVAMsgKey( unsigned long ulDvaMsgKey )
{
    std::vector<unsigned long> lstKeys;
    lstKeys.push_back( ulDvaMsgKey );

    std::vector<unsigned long> vecDvaMsgIds;
    PasHelpers::getInstance()->getDvaMsgIdsByDvaMsgKeys( vecDvaMsgIds, lstKeys );

    // One item should be got by one key
    if ( 1u != vecDvaMsgIds.size() )
    {
        m_bInvalidParameter = true;
        return;
    }

    setDVAMessageId( vecDvaMsgIds[0] );
}

void ModbusPacket::updateStationZoneIdsByZoneKeys( const std::vector<unsigned long>& vecZoneKeys )
{
    StationIdToZoneIdsMap mapStationZoneIds;
    PasHelpers::getInstance()->getStationZoneIdsByZoneKeys( mapStationZoneIds, vecZoneKeys );

    std::vector<unsigned long> vecStationIds;
    for ( StationIdToZoneIdsIt itLoop = mapStationZoneIds.begin(); mapStationZoneIds.end() != itLoop; ++itLoop )
    {
        setZonesId( itLoop->first, itLoop->second );
        vecStationIds.push_back( itLoop->first );
    }

    // No station in protocol now
    //setStationsId( vecStationIds );
}

bool ModbusPacket::isParameterInvalid()
{
    return m_bInvalidParameter;
}

std::string ModbusPacket::getInvalidParameter()
{
    return m_strInvalidParameters;
}

std::string ModbusPacket::getPacketId()
{
    return m_strPacketId;
}
} // namespace TA_IRS_App
