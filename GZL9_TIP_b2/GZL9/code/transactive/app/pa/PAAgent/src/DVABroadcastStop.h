// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DVABROADCASTSTOP__INCLUDED_)
#define DVABROADCASTSTOP__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class DVABroadcastStop : public ModbusPacket  
{
public:
	DVABroadcastStop( const std::string& strPacketId );
	virtual ~DVABroadcastStop();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
