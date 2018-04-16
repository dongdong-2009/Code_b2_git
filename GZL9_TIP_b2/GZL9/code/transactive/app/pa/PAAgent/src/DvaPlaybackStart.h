// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DVAPLAYBACKSTART__INCLUDED_)
#define DVAPLAYBACKSTART__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class DvaPlaybackStart : public ModbusPacket  
{
public:
    DvaPlaybackStart( const std::string& strPacketId );
	virtual ~DvaPlaybackStart();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
