/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/ITelegramSender.h $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This is implemented by the socket class/ the class managing the connection to the hardware.
  * If the agent is in control mode, packets will be sent, if in monitor, no packets will be sent,
  * this ensures only the control agent responds.
  */

#if !defined(AFX_ITELEGRAMSENDER_H__9126BF8C_7EA7_4681_810A_670F3C33319F__INCLUDED_)
#define AFX_ITELEGRAMSENDER_H__9126BF8C_7EA7_4681_810A_670F3C33319F__INCLUDED_

#include "boost/shared_ptr.hpp"
#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"


class ITelegramSender
{
public:
    virtual ~ITelegramSender() {};

	/**
	  * sendPacket
	  * 
      * will queue the packet to send
	  * 
	  * @return virtual void 
	  * @param : boost::shared_ptr<Telegram> packetData
	  */
    virtual void sendPacket(boost::shared_ptr<Telegram> packetData) = 0;
};

#endif // !defined(AFX_ITELEGRAMSENDER_H__9126BF8C_7EA7_4681_810A_670F3C33319F__INCLUDED_)
