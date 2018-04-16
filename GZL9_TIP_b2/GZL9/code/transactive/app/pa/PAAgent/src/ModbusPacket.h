#if !defined(MODBUSPACKET__INCLUDED_)
#define MODBUSPACKET__INCLUDED_

#include <list>
#include <vector>
#include <string>
#include "bus/modbus_comms/src/DataBlock.h"

namespace TA_IRS_App
{

enum BROADCAST_TYPE_SOURCE
{
	LIVE_BROADCAST_START_SOURCE 	= 0x0,
	DVA_BROADCAST_START_SOURCE 		= 0x1,
    BACKGROUND_MUSIC_START_SOURCE 	= 0x2,
	LIVE_BROADCAST_STOP_SOURCE 		= 0x3,
	DVA_BROADCAST_STOP_SOURCE 		= 0x4,
	BACKGROUND_MUSIC_STOP_SOURCE 	= 0x5,
	ZONE_MONITORING_START_SOURCE 	= 0x6,
	ZONE_MONITORING_STOP_SOURCE 	= 0x7
};

enum TRIGGER_EVENT_TYPE
{
    FAS_TRIGGER_BROADCAST = 0x1,
    SIG_TRIGGER_BROADCAST = 0x7
};

class ModbusPacket
{
public:
    ModbusPacket( const std::string& strPacketId );
    ModbusPacket( const std::string& strPacketId, int nStart, int nEnd );
    virtual ~ModbusPacket();

    TA_Base_Bus::DataBlock<WORD>& getModbusPacket();

    int getSendingRetryTimes();

    bool isParameterInvalid();

    std::string getInvalidParameter();

    std::string getPacketId();

    void sendingFailed();

    void updatePriorityId( unsigned long ulPriorityGroup );
    void updatePriorityByEventType( TRIGGER_EVENT_TYPE eEventType );
    void updatePriorityBySessionId( const std::string& strSessionId );
    
    void updateDVAMsgIdByDVAMsgKey( unsigned long ulDvaMsgKey );
    void updateDVAMsgIdByDVAMsgKeys( std::vector<unsigned long>& vecDvaMsgKeys );
    
    void updateStationZoneIdsByZoneKeys( const std::vector<unsigned long>& vecZoneKeys );

	void setFasTriggedBit(bool bFire);
	void setSigTriggedBit(unsigned long ulEventType, unsigned long ulPlatformType);

protected:
    void cleanBitsInWord( WORD& wdDes, unsigned long ulBitStart, unsigned long ulBitsCount );
    void setBitsInWord( WORD& wdDes, unsigned long ulBitStart, unsigned long ulBitsCount, unsigned long ulValue );
    
    // Modbus holding register packet method, can be override by Child Class

    virtual void setStationsId( const std::vector<unsigned long>& vecStationsId );
    
    virtual void setZonesId( unsigned long ulStationId, const std::vector<unsigned long>& vecZonesId );
    
    virtual void setPriorityId( unsigned long ulOperatorId );
    
    virtual void setDVAMessageId( unsigned long ulIndex, unsigned long ulMessageId );
    
    virtual void setCyclicTimes( unsigned long ulCyclicTimes );
    
    virtual void setBroadcastSource( BROADCAST_TYPE_SOURCE eBroadcastSource );
    
    virtual void setIsStop( bool bStop );

    virtual void setDwellTime( unsigned long ulDwellInSec );

private:
    const ModbusPacket& operator=( const ModbusPacket& );
    ModbusPacket( const ModbusPacket& );
    ModbusPacket();

protected:
    std::string m_strPacketId;

    TA_Base_Bus::DataBlock<WORD> m_dbwModbusPacket;

    int m_nRetrySendingTimes;
    
    bool m_bInvalidParameter;

    std::string m_strInvalidParameters;
};

} // namespace TA_IRS_App

#endif