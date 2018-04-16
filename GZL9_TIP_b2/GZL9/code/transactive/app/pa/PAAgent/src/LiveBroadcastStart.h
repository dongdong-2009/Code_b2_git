// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LIVEBROADCASTSTART__INCLUDED_)
#define LIVEBROADCASTSTART__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class LiveBroadcastStart : public ModbusPacket  
{
public:
	LiveBroadcastStart( const std::string& strPacketId );
	virtual ~LiveBroadcastStart();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
