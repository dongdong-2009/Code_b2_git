// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DVAPLAYBACKSTOP__INCLUDED_)
#define DVAPLAYBACKSTOP__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class DvaPlaybackStop : public ModbusPacket  
{
public:
	DvaPlaybackStop( const std::string& strPacketId );
	virtual ~DvaPlaybackStop();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
