// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DVABROADCASTSTART__INCLUDED_)
#define DVABROADCASTSTART__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class DVABroadcastStart : public ModbusPacket
{
public:
	DVABroadcastStart( const std::string& strPacketId );
	virtual ~DVABroadcastStart();

    void updateCylicTimes( unsigned long ulTimes );

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(AFX_DVABROADCAST_H__5ED21494_6035_4A91_BC9F_0180321FBA36__INCLUDED_)
