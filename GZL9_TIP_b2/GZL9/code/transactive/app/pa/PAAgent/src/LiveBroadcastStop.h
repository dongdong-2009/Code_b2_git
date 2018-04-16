// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LIVEBROADCASTSTOP__INCLUDED_)
#define LIVEBROADCASTSTOP__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class LiveBroadcastStop : public ModbusPacket  
{
public:
	LiveBroadcastStop( const std::string& strPacketId );
	virtual ~LiveBroadcastStop();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
