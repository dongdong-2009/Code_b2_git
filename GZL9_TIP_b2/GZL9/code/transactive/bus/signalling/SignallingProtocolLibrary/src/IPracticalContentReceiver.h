
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\IPracticalContentReceiver.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/7/11 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * Receives a IPracticalContentReceiver telegram
  */

#ifndef _I_PRACTICAL_CONTENT_RECEIVER_H_
#define _I_PRACTICAL_CONTENT_RECEIVER_H_
#include "boost/shared_ptr.hpp"

class IPracticalContentReceiver 
{

public:
	virtual ~IPracticalContentReceiver(){};

 /**
  * receiveTelegram
  * 
  * The telegram to receive and process.
  * The object pointed to cannot be null and the boost pointer takes care of the memory.
  * 
  * @return virtual void 
  * @param : boost::shared_ptr<Telegram> telegram
  */
	virtual void receiveTelegram(std::vector<boost::shared_ptr<Telegram> >& telegram) = 0;

};

#endif