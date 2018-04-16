// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TRIGGEREDDVABROADCAST__INCLUDED_)
#define TRIGGEREDDVABROADCAST__INCLUDED_

#include <vector>

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class TriggeredDVABroadcast
{
public:
	TriggeredDVABroadcast( time_t tmStartTime, TRIGGER_EVENT_TYPE eEventType,
        const std::string& strBroadcastId, unsigned long ulDvaMsgKey, 
        const std::vector<unsigned long>& lstZoneKeys, unsigned long ulCyclicTimes );
	TriggeredDVABroadcast( unsigned long ulSigEventType, unsigned long ulPlatFormType);
	TriggeredDVABroadcast( const bool bOnFire );
	virtual ~TriggeredDVABroadcast();

    virtual void executeBroadcast();

    virtual void executeSigBroadcast();
    virtual void executeFasBroadcast();

    time_t getStartTime();
    std::string getBroadcastId();

private:
    TriggeredDVABroadcast();

private:
    std::string m_strBroadcastId;
    TRIGGER_EVENT_TYPE m_eEventType;
    unsigned long m_ulDvaMsgKey;
    std::vector<unsigned long> m_lstZoneKeys;
    unsigned long m_ulCyclicTimes;
    time_t m_tmStartTime;
	unsigned long m_ulSigEventType;
	unsigned long m_ulPlatFormType;
    bool m_bOnFire;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
