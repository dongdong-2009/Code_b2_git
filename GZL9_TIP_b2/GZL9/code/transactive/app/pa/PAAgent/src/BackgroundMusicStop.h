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

#if !defined(BACKGROUNDMUSICSTOP__INCLUDED_)
#define BACKGROUNDMUSICSTOP__INCLUDED_

#include "app/pa/PAAgent/src/ModbusPacket.h"

namespace TA_IRS_App
{

class BackgroundMusicStop : public ModbusPacket  
{
public:
	BackgroundMusicStop( const std::string& strPacketId );
	virtual ~BackgroundMusicStop();

private:
    bool m_bIsMasterBroadcast;
};

}

#endif // !defined(BACKGROUNDMUSICSTOP__INCLUDED_)
