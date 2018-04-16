/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\bus\signalling\SignallingProtocolLibrary\src\ITrainPisInfoReceiver.h $
  * @author:  Lucky
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * Receives a Train Location telegram
  */

#ifndef _ITRAIN_PIS_INFO_RECEIVER_H_
#define _ITRAIN_PIS_INFO_RECEIVER_H_
#include "boost/shared_ptr.hpp"

class ITrainPisInfoReceiver
{
public:
	virtual ~ITrainPisInfoReceiver(){};

 /**
  * receiveTelegram
  * 
  * The telegram to receive and process.
  * The object pointed to cannot be null and the boost pointer takes care of the memory.
  * 
  * @return virtual void 
  * @param : boost::shared_ptr<RadioTelegram> telegram
  */
	virtual void receiveTelegram(std::vector<boost::shared_ptr<Telegram> >& telegram) = 0;

};

#endif