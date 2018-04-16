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

#if !defined(BACKGROUNDMUSICSTART__INCLUDED_)
#define BACKGROUNDMUSICSTART__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class BackgroundMusicStart : public ModbusPacket  
{
public:
	BackgroundMusicStart( const std::string& strPacketId );
	virtual ~BackgroundMusicStart();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(BACKGROUNDMUSICSTART__INCLUDED_)
